/*
 *
 *  Copyright ( c ) 2017
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "zulupolkit.h"
#include "siriPolkit.h"
#include "task.hpp"
#include "../utility2.h"
#include "json.h"

#include <termios.h>
#include <memory>
#include <iostream>
#include <vector>

#include <QCoreApplication>
#include <QFile>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static std::vector< const char * > executableSearchPaths()
{
	return { "/usr/local/bin/",
		"/usr/local/sbin/",
		"/usr/bin/",
		"/usr/sbin/",
		"/bin/",
		"/sbin/",
		"/opt/local/bin/",
		"/opt/local/sbin/",
		"/opt/bin/",
		"/opt/sbin/" } ;
}

static inline QString executableFullPath( const QString& e )
{
	QString exe ;

	for( const auto& it : executableSearchPaths() ){

		exe = it + e ;

		if( QFile::exists( exe ) ){

			return exe ;
		}
	}

	return QString() ;
}

static bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old ;
	current->c_lflag &= ~ECHO ;

	if( tcsetattr( 1,TCSAFLUSH,current ) != 0 ){

		return false ;
	}else{
		return true ;
	}
}

zuluPolkit::zuluPolkit( const QStringList& s ) : m_arguments( s )
{
	connect( &m_server,SIGNAL( newConnection() ),this,SLOT( gotConnection() ) ) ;
}

zuluPolkit::~zuluPolkit()
{
	QFile::remove( m_socketPath ) ;
}

static void _set_path_writable_by_others( const QString& e )
{
	QFile f( e ) ;

	for( decltype( umask( 0 ) ) i = 0 ; i < 1000 ; i++ ){

		umask( i ) ;

		f.open( QIODevice::WriteOnly ) ;

		auto s = f.permissions() ;

		f.close() ;
		f.remove() ;

		if( s & QFileDevice::WriteOther ){

			break ;
		}
	}
}

void zuluPolkit::start()
{
	if( m_arguments.size() > 1 ){

		m_cookie = this->readStdin() ;

		m_socketPath = m_arguments.at( 1 ) ;

		bool ok ;

		auto uid = QProcessEnvironment::systemEnvironment().value( "PKEXEC_UID" ).toInt( &ok ) ;

		if( ok ){

			auto e = m_socketPath ;
			e.remove( "/siriPolkit.socket" ) ;

			auto s = e.toLatin1() ;

			if( chown( s.constData(),uid,uid ) ){}
			if( chmod( s.constData(),0700 ) ){}
		}

		QFile::remove( m_socketPath ) ;

		auto s = umask( 0 ) ;

		_set_path_writable_by_others( m_socketPath ) ;

		m_server.listen( m_socketPath ) ;

		umask( s ) ;
	}
}

static void _respond( QLocalSocket& s,const char * e )
{
	nlohmann::json json ;

	json[ "stdOut" ]     = e ;
	json[ "stdError" ]   = e ;
	json[ "exitCode" ]   = 255 ;
	json[ "exitStatus" ] = 255 ;
	json[ "finished" ]   = true ;

	s.write( json.dump().c_str() ) ;

	s.waitForBytesWritten() ;
}

static void _respond( QLocalSocket& s,const Task::process::result& e )
{
	nlohmann::json json ;

	json[ "stdOut" ]     = e.std_out().constData() ;
	json[ "stdError" ]   = e.std_error().constData() ;
	json[ "exitCode" ]   = e.exit_code() ;
	json[ "exitStatus" ] = e.exit_status() ;
	json[ "finished" ]   = e.finished() ;

	s.write( json.dump().c_str() ) ;

	s.waitForBytesWritten() ;
}

void zuluPolkit::gotConnection()
{
	std::unique_ptr< QLocalSocket > s( m_server.nextPendingConnection() ) ;

	auto& m = *s ;

	try{
		m.waitForReadyRead() ;

		auto json = nlohmann::json::parse( m.readAll().constData() ) ;

		auto password = QString::fromStdString( json[ "password" ].get< std::string >() ) ;
		auto cookie   = QString::fromStdString( json[ "cookie" ].get< std::string >() ) ;
		auto command  = QString::fromStdString( json[ "command" ].get< std::string >() ) ;

		auto su = executableFullPath( "su" ) ;

		auto e = su + " - -c \"'" + executableFullPath( "ecryptfs-simple" ) ;
		auto f = su + " - -c \"" + executableFullPath( "ecryptfs-simple" ) ;

		if( cookie == m_cookie ){

			if( command == "exit" ){

				return QCoreApplication::quit() ;

			}else if( command.startsWith( e ) || command.startsWith( f ) ){

				return _respond( m,Task::process::run( command,password.toLatin1() ).get() ) ;
			}else{
				_respond( m,"SiriPolkit: Invalid Command" ) ;
			}
		}else{
			_respond( m,"SiriPolkit: Failed To Authenticate Request" ) ;
		}
	}catch( ... ){

		_respond( m,"SiriPolkit: Failed To Parse Request" ) ;
	}
}

QString zuluPolkit::readStdin()
{
	std::cout << "Token: " << std::flush ;

	struct termios old ;
	struct termios current ;

	_terminalEchoOff( &old,&current ) ;

	QString s ;
	int e ;

	int m = 1024 ;

	for( int i = 0 ; i < m ; i++ ){

		e = std::getchar() ;

		if( e == '\n' || e == -1 ){

			break ;
		}else{
			s += static_cast< char >( e ) ;
		}
	}

	tcsetattr( 1,TCSAFLUSH,&old ) ;

	std::cout << std::endl ;

	return s ;
}
