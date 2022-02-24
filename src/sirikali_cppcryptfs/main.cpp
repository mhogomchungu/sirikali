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
#include <QThread>

class counter
{
public:
	counter( int s ) : m_max( s )
	{
	}
	bool Continue() const
	{
		return m_counter < m_max ;
	}
	counter next() const
	{
		counter c = *this ;
		c.m_counter++ ;
		return c ;
	}
private:
	int m_max ;
	int m_counter = 0 ;
};

class result
{
public:
	result( QProcess& p ) :
		m_exitCode( p.exitCode() ),
		m_exitStatus( p.exitStatus() ),
		m_stdOut( p.readAllStandardOutput() ),
		m_stdError( p.readAllStandardError() )
	{
	}
	const QByteArray& stdOut() const
	{
		return m_stdOut ;
	}
	const QByteArray& stdError() const
	{
		return m_stdError ;
	}
	bool success() const
	{
		return m_exitCode == 0 && m_exitStatus == QProcess::ExitStatus::NormalExit ;
	}
	int exitCode() const
	{
		return m_exitCode ;
	}
	bool pipeConnectionFailed() const
	{
		return m_stdError.contains( "Named pipe connection wait failed" ) ;
	}
private:
	int m_exitCode ;
	QProcess::ExitStatus m_exitStatus ;
	QByteArray m_stdOut ;
	QByteArray m_stdError ;
};

struct arguments
{
	arguments( QStringList l ) : args( std::move( l ) )
	{
		auto _arg = [ this ]( const QString& arg )->QString{

			int j = args.size() ;

			for( int i = 0 ; i < j ; i++ ){

				if( args.at( i ) == arg ){

					if( i + 1 < j ){

						return args.at( i + 1 ) ;
					}else{
						return {} ;
					}
				}
			}

			return {} ;
		} ;

		cryptFolder   = _arg( "--cipherPath" ) ;
		mountPoint    = _arg( "--mountPath" ) ;
		configPath    = _arg( "--config" ) ;
		cppcryptfsctl = _arg( "--cppcryptfsctl-path" ) ;
		readOnly      = _arg( "-o" ).startsWith( "ro" ) ;
		quitCppcryptfs = this->contains( "--exit" ) ;
	}
	bool contains( const QString& e ) const
	{
		return args.contains( e ) ;
	}
	QString cryptFolder ;
	QString mountPoint ;
	QString configPath ;
	QString cppcryptfsctl ;
	bool readOnly ;
	bool quitCppcryptfs ;
private:
	QStringList args ;
};

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
			m_pointer = new ( &m_storage ) bgt_t( m_bgt() ) ;
		}
		void then()
		{
			m_fgt( std::move( *m_pointer ) ) ;

			m_pointer->~bgt_t() ;

			this->deleteLater() ;
		}
	private:
		BackGroundTask m_bgt ;
		UiThreadResult m_fgt ;		

#if __cplusplus >= 201703L
		using bgt_t = std::invoke_result_t< BackGroundTask > ;
		alignas( bgt_t ) std::byte m_storage[ sizeof( bgt_t ) ] ;
#else
		using bgt_t = std::result_of_t< BackGroundTask() > ;
		typename std::aligned_storage< sizeof( bgt_t ),alignof( bgt_t ) >::type m_storage ;
#endif
		bgt_t * m_pointer ;
	};

	new Thread( std::move( bgt ),std::move( fgt ) ) ;
}

template< typename Result >
static void _run( const QString& exe,const QStringList& args,const QByteArray& password,Result result )
{
	auto cmd = new QProcess() ;

	QObject::connect( cmd,&QProcess::started,[ cmd,password ](){

		cmd->write( password ) ;
	} ) ;

	using cc = void( QProcess::* )( int,QProcess::ExitStatus ) ;

	auto s = static_cast< cc >( &QProcess::finished ) ;

	QObject::connect( cmd,s,[ cmd,result = std::move( result ) ]( int,QProcess::ExitStatus ){

		result( *cmd ) ;

		cmd->deleteLater() ;
	} ) ;

	cmd->start( exe,args ) ;
}

template< typename Result >
static void _run( const QString& exe,const QStringList& args,Result r )
{
	_run( exe,args,{},std::move( r ) ) ;
}

static void _checkMounted( const arguments& args )
{
	_run( args.cppcryptfsctl,{ "-l" },[ &args ]( const result& s ){

		const auto e = s.stdOut().split( '\n' ) ;

		auto cf = args.cryptFolder.toUtf8() ;

		bool found = false ;

		for( const auto& it : e ){

			if( it.contains( cf ) ){

				found = true ;

				break ;
			}
		}

		if( found ){

			QTimer::singleShot( 2000,[ &args ](){

				_checkMounted( args ) ;
			} ) ;
		}else{
			QCoreApplication::exit( 0 ) ;
		}
	} ) ;
}

static void _msg( const result& r,const char * msg )
{
	std::cout << "status: " << msg << "\n" ;
	std::cout << "std out: " + r.stdOut().trimmed().toStdString() << "\n" ;
	std::cout << "std error: " + r.stdError().trimmed().toStdString() << std::endl ;
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

static void _mount( const arguments& arr,const QByteArray& password,counter c )
{
	QStringList args{ "-p",password,"-d",arr.mountPoint,"-m",arr.cryptFolder,"-c",arr.configPath } ;

	if( arr.readOnly ){

		args.append( "-r" ) ;
	}

	_run( arr.cppcryptfsctl,args,[ &arr,password,c ]( const result& m ){

		if( m.success() ){

			_msg( m,"Mount Success" ) ;

			QTimer::singleShot( 2000,[ &arr ](){

				_checkMounted( arr ) ;
			} ) ;
		}else{
			_msg( m,"Failed To Mount" ) ;

			if( c.Continue() && m.pipeConnectionFailed() ){

				return _mount( arr,password,c.next() ) ;
			}

			QCoreApplication::exit( m.exitCode() ) ;
		}
	} ) ;
}

static void _umount( const arguments& args,counter c )
{
	QStringList opts{ "-u",args.mountPoint } ;

	if( args.quitCppcryptfs ){

		opts.append( "--exit" ) ;
	}

	_run( args.cppcryptfsctl,opts,[ &args,c ]( const result& m ){

		if( m.success() ){

			_msg( m,"Umount Success" ) ;
		}else{
			_msg( m,"Failed To Unmount" ) ;

			if( c.Continue() && m.pipeConnectionFailed() ){

				return _umount( args,c.next() ) ;
			}
		}

		QCoreApplication::exit( m.exitCode() ) ;
	} ) ;
}

static void _create( const arguments& args,const QByteArray& password,counter c )
{
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

	opts.append( "--init=" + args.cryptFolder ) ;

	_run( args.cppcryptfsctl,opts,password,[ &args,password,c ]( const result& m ){

		if( m.success() ){

			_msg( m,"Create Success" ) ;
		}else{
			_msg( m,"Failed To Create" ) ;

			if( c.Continue() && m.pipeConnectionFailed() ){

				return _create( args,password,c.next() ) ;
			}
		}

		QCoreApplication::exit( m.exitCode() ) ;
	} ) ;
}

static void _run( const arguments& args )
{
	if( args.contains( "--mount" ) ){

		_read_password( [ &args ]( const QByteArray& password ){

			_mount( args,password,3 ) ;
		} ) ;

	}else if( args.contains( "--umount" ) ){

		_umount( args,3 ) ;

	}else if( args.contains( "--create" ) ){

		_read_password( [ & ]( const QByteArray& password ){

			_create( args,password,3 ) ;
		} ) ;
	}else{
		std::cout << "Unknown command" << std::endl ;

		QCoreApplication::exit( 255 ) ;
	}
}

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	arguments args( a.arguments() ) ;

	QTimer::singleShot( 0,[ & ](){

		_run( args ) ;
	} ) ;

	return a.exec() ;
}
