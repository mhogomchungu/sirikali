/*
 *
 *  Copyright (c) 2022
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <iostream>
#include <QProcess>
#include <QTimer>

#include "task.hpp"

template<typename Function,typename ... Args>
using result_of = std::result_of_t<Function(Args ...)> ;

template< typename BackGroundTask,typename UiThreadResult >
static void _runInBgThread( BackGroundTask bgt,UiThreadResult fgt )
{
	class Thread : public QThread
	{
	public:
		Thread( BackGroundTask&& bgt,UiThreadResult&& fgt ) :
			m_bgt( std::move( bgt ) ),
			m_fgt( std::move( fgt ) )
		{
			connect( this,&QThread::finished,this,&Thread::then,Qt::QueuedConnection ) ;

			this->start() ;
		}
		void run() override
		{
			m_data = m_bgt() ;
		}
		void then()
		{
			m_fgt( std::move( m_data ) ) ;

			this->deleteLater() ;
		}
	private:
		BackGroundTask m_bgt ;
		UiThreadResult m_fgt ;

		result_of< BackGroundTask > m_data ;
	};

	new Thread( std::move( bgt ),std::move( fgt ) ) ;
}

static QString _cmdArgumentValue( const QStringList& l,const QString& arg )
{
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		if( l.at( i ) == arg ){

			if( i + 1 < j ){

				return l.at( i + 1 ) ;
			}else{
				return {} ;
			}
		}
	}

	return {} ;
}

static void _checkMounted( const QString& exe,const QByteArray& cf )
{
	auto s = Task::process::run( exe,QStringList{ "-l" } ).await().std_out() ;

	const auto e = s.split( '\n' ) ;

	bool found = false ;

	for( const auto& it : e ){

		if( it.contains( cf ) ){

			found = true ;

			break ;
		}
	}

	if( found ){

		QTimer::singleShot( 2000,[ = ](){

			_checkMounted( exe,cf ) ;
		} ) ;
	}else{
		QCoreApplication::exit( 0 ) ;
	}
}

template< typename Then >
static void _read_password( Then then )
{
	_runInBgThread( [](){

		QByteArray s ;
		int e ;

		const int m = 4096 ;

		for( int i = 0 ; i < m ; i++ ){

			e = std::getchar() ;

			if( e == '\n' || e == -1 ){

				break ;
			}else{
				s += static_cast< char >( e ) ;
			}
		}


		return s ;

	},std::move( then ) ) ;
}

static void _mount( QCoreApplication&,const QStringList& l,const QByteArray& password )
{
	auto cryptFolder   = _cmdArgumentValue( l,"--cipherPath" ) ;
	auto mountPoint    = _cmdArgumentValue( l,"--mountPath" ) ;
	auto config        = _cmdArgumentValue( l,"--config" ) ;
	auto cppcryptfsctl = _cmdArgumentValue( l,"--cppcryptfsctl-path" ) ;
	auto mode          = _cmdArgumentValue( l ,"-o" ) ;

	QStringList args{ "-p",password,"-d",mountPoint,"-m",cryptFolder,"-c",config } ;

	if( mode.startsWith( "ro" ) ){

		args.append( "-r" ) ;
	}

	Task::process::run( cppcryptfsctl,args ).then( [ cryptFolder,cppcryptfsctl ]( const auto& m ){

		if( m.success() ){

			std::cout << "Mount Success" << std::endl ;

			std::cout << m.std_error().toStdString() << std::endl ;

			auto cf = cryptFolder.toUtf8() ;

			QTimer::singleShot( 2000,[ = ](){

				_checkMounted( cppcryptfsctl,cf ) ;
			} ) ;
		}else{
			auto s = QString::number( m.exit_code() ).toStdString() ;
			std::cout << "Failed To Mount: Error Code: " + s << std::endl ;

			std::cout << m.std_error().toStdString() << std::endl ;
			std::cout << m.std_out().toStdString() << std::endl ;

			QCoreApplication::exit( m.exit_code() ) ;
		}
	} ) ;
}

static void _umount( const QStringList& args )
{
	auto mountPoint  = _cmdArgumentValue( args,"--mountPath" ) ;
	auto exe         = _cmdArgumentValue( args,"--cppcryptfsctl-path" ) ;

	Task::process::run( exe,QStringList{ "-u",mountPoint } ).then( []( const auto& m ){

		if( m.success() ){

			std::cout << "Umount Success" << std::endl ;
		}else{
			auto s = QString::number( m.exit_code() ).toStdString() ;
			std::cout << "Failed To Unmount: Error Code: " + s << std::endl ;

			std::cout << m.std_error().toStdString() << std::endl ;
			std::cout << m.std_out().toStdString() << std::endl ;
		}

		QCoreApplication::exit( m.exit_code() ) ;
	} ) ;
}

static void _create( const QStringList& args,const QByteArray& password )
{
	auto cipherPath = _cmdArgumentValue( args,"--cipherPath" ) ;
	auto exe        = _cmdArgumentValue( args,"--cppcryptfsctl-path" ) ;

	if( password.isEmpty() ){

		std::cout << "Create Failed: Error Code: 1 " << std::endl ;
		std::cout << "Failure Reason: Empty Password" << std::endl ;
	}

	QStringList opts ;

	if( args.contains( "--siv" ) ){

		opts.append( "--siv" ) ;
	}

	if( args.contains( "--plaintext" ) ){

		opts.append( "--plaintext" ) ;
	}

	if( args.contains( "--reverse" ) ){

		opts.append( "--reverse" ) ;
	}

	opts.append( "--init=" + cipherPath ) ;

	Task::process::run( exe,opts,-1,password ).then( []( const auto& m ){

		if( m.success() ){

			std::cout << "Create Success" << std::endl ;
		}else{
			auto s = QString::number( m.exit_code() ).toStdString() ;
			std::cout << "Create Failed: Error Code: " + s << std::endl ;

			std::cout << m.std_error().toStdString() << std::endl ;
			std::cout << m.std_out().toStdString() << std::endl ;
		}

		QCoreApplication::exit( m.exit_code() ) ;
	} ) ;
}

static void run( QCoreApplication& app,const QStringList& args )
{
	if( args.contains( "--mount" ) ){

		_read_password( [ & ]( const QByteArray& password ){

			_mount( app,args,password ) ;
		} ) ;

	}else if( args.contains( "--umount" ) ){

		_umount( args ) ;

	}else if( args.contains( "--create" ) ){

		_read_password( [ & ]( const QByteArray& password ){

			_create( args,password ) ;
		} ) ;
	}else{
		std::cout << "Unknown command" << std::endl ;

		QCoreApplication::exit( 255 ) ;
	}
}

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	auto args = a.arguments() ;

	QTimer::singleShot( 0,[ & ](){

		run( a,args ) ;
	} ) ;

	return a.exec() ;
}
