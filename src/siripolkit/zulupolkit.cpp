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
#include "../task.h"

#include "../3rdParty/json.hpp"

#include <termios.h>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <QTimer>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>

namespace utility
{
	QStringList split( const QString& e,char token )
	{
		return e.split( token,QString::SkipEmptyParts ) ;
	}
	class Task
	{
	public :
		static ::Task::future< utility::Task >& run( const QString& exe )
		{
			return ::Task::run< utility::Task >( [ exe ](){ return utility::Task( exe ) ; } ) ;
		}
		static void wait( int s )
		{
			sleep( s ) ;
		}
		static void waitForOneSecond( void )
		{
			sleep( 1 ) ;
		}
		static void waitForTwoSeconds( void )
		{
			sleep( 2 ) ;
		}
		static void suspendForOneSecond( void )
		{
			utility::Task::suspend( 1 ) ;
		}
		static void suspend( int s )
		{
			QTimer t ;

			QEventLoop l ;

			QObject::connect( &t,SIGNAL( timeout() ),&l,SLOT( quit() ) ) ;

			t.start( 1000 * s ) ;

			l.exec() ;
		}
		static QString makePath( QString e )
		{
			e.replace( "\"","\"\"\"" ) ;

			return "\"" + e + "\"" ;
		}
		Task()
		{
		}
		Task( const QString& exe,int waitTime = -1,const QProcessEnvironment& env = QProcessEnvironment(),
		      const QByteArray& password = QByteArray(),const std::function< void() >& f = [](){} )
		{
			this->execute( exe,waitTime,env,password,f ) ;
		}
		Task( const QString& exe,const QProcessEnvironment& env,const std::function< void() >& f )
		{
			this->execute( exe,-1,env,QByteArray(),f ) ;
		}
		QStringList splitOutput( char token ) const
		{
			return utility::split( m_stdOut,token ) ;
		}
		void stdOut( const QByteArray& r )
		{
			m_stdOut = r ;
		}
		const QByteArray& stdOut() const
		{
			return m_stdOut ;
		}
		const QByteArray& stdError() const
		{
			return m_stdError ;
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		int exitStatus() const
		{
			return m_exitStatus ;
		}
		bool success() const
		{
			return m_exitCode == 0 && m_exitStatus == QProcess::NormalExit && m_finished == true ;
		}
		bool failed() const
		{
			return !this->success() ;
		}
		bool finished() const
		{
			return m_finished ;
		}
		bool ok() const
		{
			return this->splitOutput( '\n' ).size() > 12 ;
		}
	private:
		void execute( const QString& exe,int waitTime,const QProcessEnvironment& env,
			      const QByteArray& password,const std::function< void() >& f )
		{
			class Process : public QProcess{
			public:
				Process( const std::function< void() >& f ) : m_function( f )
				{
				}
			protected:
				void setupChildProcess()
				{
					m_function() ;
				}
			private:
				std::function< void() > m_function ;
			} p( f ) ;

			p.setProcessEnvironment( env ) ;

			p.start( exe ) ;

			if( !password.isEmpty() ){

				p.waitForStarted() ;

				p.write( password + '\n' ) ;

				p.closeWriteChannel() ;
			}

			m_finished   = p.waitForFinished( waitTime ) ;
			m_exitCode   = p.exitCode() ;
			m_exitStatus = p.exitStatus() ;
			m_stdOut     = p.readAllStandardOutput() ;
			m_stdError   = p.readAllStandardError() ;
		}

		QByteArray m_stdOut ;
		QByteArray m_stdError ;

		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};
}

static bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old;
	current->c_lflag &= ~ECHO;

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
	QDir().remove( m_socketPath ) ;
}

void zuluPolkit::start()
{
	if( m_arguments.size() > 1 ){

		m_token = this->readStdin() ;

		m_socketPath = m_arguments.at( 1 ) ;

		QDir().remove( m_socketPath ) ;

		m_server.listen( m_socketPath ) ;
	}
}

void zuluPolkit::gotConnection()
{
	try{
		std::unique_ptr< QLocalSocket > s( m_server.nextPendingConnection() ) ;

		s->waitForReadyRead() ;

		auto json = nlohmann::json::parse( s->readAll().constData() ) ;

		auto password = QString::fromStdString( json[ "password" ].get< std::string >() ) ;
		auto token    = QString::fromStdString( json[ "cookie" ].get< std::string >() ) ;
		auto command  = QString::fromStdString( json[ "command" ].get< std::string >() ) ;

		if( token != m_token ){

			return ;
		}

		if( command == "exit" ){

			QCoreApplication::quit() ;
		}else{
			auto e = utility::Task( command,
						-1,
						QProcessEnvironment::systemEnvironment(),
						password.toLatin1(),
						[](){} ) ;

			nlohmann::json json ;

			json[ "stdOut" ]     = e.stdOut().constData() ;
			json[ "stdError" ]   = e.stdError().constData() ;
			json[ "exitCode" ]   = e.exitCode() ;
			json[ "exitStatus" ] = e.exitStatus() ;
			json[ "finished" ]   = e.finished() ;

			s->write( json.dump().c_str() ) ;

			s->waitForBytesWritten() ;
		}

	}catch( ... ){}
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
