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
#include <QDir>

#include <fileapi.h>

#include <array>

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
		auto c = *this ;
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
	bool retry( const counter& c ) const
	{
		return c.Continue() && m_stdError.contains( "Named pipe connection wait failed" ) ;
	}
private:
	int m_exitCode ;
	QProcess::ExitStatus m_exitStatus ;
	QByteArray m_stdOut ;
	QByteArray m_stdError ;
};

struct context
{
	context() = delete ;
	context( const context& ) = delete ;
	context( context&& ) = delete ;
	context( const QCoreApplication& app ) :
		args( app.arguments() ),
		cryptFolder( this->arg( "--cipherPath" ) ),
		mountPoint( this->arg( "--mountPath" ) ),
		configPath( this->arg( "--config" ) ),
		cppcryptfsctl( this->arg( "--cppcryptfsctl-path" ) ),
		readOnly( this->arg( "-o" ).startsWith( "ro" ) ),
		quitCppcryptfs( this->contains( "--exit" ) )
	{
	}
	bool contains( const QString& e ) const
	{
		return args.contains( e ) ;
	}
	QStringList args ;
	QString cryptFolder ;
	QString mountPoint ;
	QString configPath ;
	QString cppcryptfsctl ;
	QByteArray password ;
	bool readOnly ;
	bool quitCppcryptfs ;
private:
	QString arg( const char * arg )
	{
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
	}
};

template< typename Function,typename Ctx >
static void _run_delayed( int waitTime,Ctx& ctx,Function function )
{
	QTimer::singleShot( waitTime,[ &ctx,function = std::move( function ) ](){

		function( ctx ) ;
	} ) ;
}

static void _quit( int s )
{
	QCoreApplication::exit( s ) ;
}

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

	if( !password.isEmpty() ){

		QObject::connect( cmd,&QProcess::started,[ cmd,password ](){

			cmd->write( password ) ;
		} ) ;
	}

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

static bool _is_driveLetter( const QString& m )
{
	return m.size() == 2 ;
}

static QString _deviceName( const context& ctx )
{
	std::array< wchar_t,4096 > mountPoint ;
	std::array< wchar_t,4096 > buffer ;

	std::fill( mountPoint.begin(),mountPoint.end(),'\0' ) ;
	std::fill( buffer.begin(),buffer.end(),'\0' ) ;

	if( _is_driveLetter( ctx.mountPoint ) ){

		ctx.mountPoint.toWCharArray( mountPoint.data() ) ;

		auto s = QueryDosDeviceW( mountPoint.data(),buffer.data(),buffer.size() ) ;

		return QString::fromWCharArray( buffer.data(),s ) ;
	}else{
		auto m = ctx.mountPoint + "\\" ;

		m.toWCharArray( mountPoint.data() ) ;

		auto s = GetVolumeNameForVolumeMountPointW( mountPoint.data(),buffer.data(),buffer.size() ) ;

		if( s ){

			return QString::fromWCharArray( buffer.data() ) ;
		}else{
			return {} ;
		}
	}
}

static void _hang_until_unmounted( const context& ctx,const QString& id )
{
	_run_delayed( 1000,ctx,[ id ]( const context& ctx ){

		auto d = _deviceName( ctx ) ;

		if( d != id ){

			_quit( 0 ) ;
		}else{
			_hang_until_unmounted( ctx,id ) ;
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

static void _mount( const context& ctx,counter c = 3 )
{
	QStringList args{ "-p",ctx.password,"-d",ctx.mountPoint,"-m",ctx.cryptFolder,"-c",ctx.configPath } ;

	if( ctx.readOnly ){

		args.append( "-r" ) ;
	}

	_run( ctx.cppcryptfsctl,args,[ &ctx,c ]( const result& m ){

		if( m.success() ){

			_msg( m,"Mount Success" ) ;

			_hang_until_unmounted( ctx,_deviceName( ctx ) ) ;
		}else{
			_msg( m,"Failed To Mount" ) ;

			if( m.retry( c ) ){

				_mount( ctx,c.next() ) ;
			}else{
				_quit( m.exitCode() ) ;
			}
		}
	} ) ;
}

static void _info( const context& ctx,counter c = 3 )
{
	_run( ctx.cppcryptfsctl,{ "-i",ctx.mountPoint },[ &ctx,c ]( const result& m ){

		if( m.success() ){

			auto s = m.stdOut().split( '\n' ) ;

			for( int i = 0 ; i < s.size() ; i++ ){

				auto& e = s[ i ] ;

				int k = e.indexOf( ':' ) ;

				if( k != -1 && k + 2 < e.size() ){

					k += 2 ;

					while( e[ k ] == ' ' ){

						e.remove( k,1 ) ;
					}
				}

				std::cout << e.constData() << "\n" ;
			}

			std::cout << std::endl ;
		}else{
			if( m.retry( c ) ){

				return _info( ctx,c.next() ) ;
			}
		}

		_quit( m.exitCode() ) ;
	} ) ;
}

static void _umount( const context& ctx,counter c = 3 )
{
	QStringList opts{ "-u",ctx.mountPoint } ;

	if( ctx.quitCppcryptfs ){

		opts.append( "--exit" ) ;
	}

	_run( ctx.cppcryptfsctl,opts,[ &ctx,c ]( const result& m ){

		if( m.success() ){

			if( !_is_driveLetter( ctx.mountPoint ) ){

				QDir d ;
				d.rmdir( ctx.mountPoint ) ;
				d.mkdir( ctx.mountPoint ) ;
			}

			_msg( m,"Umount Success" ) ;
		}else{
			_msg( m,"Failed To Unmount" ) ;

			if( m.retry( c ) ){

				return _umount( ctx,c.next() ) ;
			}
		}

		_quit( m.exitCode() ) ;
	} ) ;
}

static void _create( const context& ctx,counter c = 3 )
{
	QStringList opts ;

	if( ctx.contains( "--siv" ) ){

		opts.append( "--siv" ) ;
	}

	if( ctx.contains( "--plaintext" ) ){

		opts.append( "--plaintext" ) ;
	}

	if( ctx.contains( "--reverse" ) ){

		opts.append( "--reverse" ) ;
	}

	opts.append( "--init=" + ctx.cryptFolder ) ;

	_run( ctx.cppcryptfsctl,opts,ctx.password,[ &ctx,c ]( const result& m ){

		if( m.success() ){

			_msg( m,"Create Success" ) ;
		}else{
			_msg( m,"Failed To Create" ) ;

			if( m.retry( c ) ){

				return _create( ctx,c.next() ) ;
			}
		}

		_quit( m.exitCode() ) ;
	} ) ;
}

static void _exec( context& ctx )
{
	if( ctx.contains( "--mount" ) ){

		_read_password( [ &ctx ]( QByteArray password ){

			ctx.password = std::move( password ) ;

			_mount( ctx ) ;
		} ) ;

	}else if( ctx.contains( "--umount" ) ){

		_umount( ctx ) ;

	}else if( ctx.contains( "--create" ) ){

		_read_password( [ & ]( const QByteArray& password ){

			ctx.password = password + "\n" + password ;

			_create( ctx ) ;
		} ) ;

	}else if( ctx.contains( "--info" ) ){

		_info( ctx ) ;

	}else if( ctx.contains( "--deviceName" ) ){

		std::cout << _deviceName( ctx ).toStdString() << std::endl ;

		_quit( 0 ) ;

	}else if( ctx.contains( "--checkMounted" ) ){

		_hang_until_unmounted( ctx,_deviceName( ctx ) ) ;
	}else{
		std::cout << "Unknown command" << std::endl ;

		_quit( 255 ) ;
	}
}

int main( int argc,char * argv[] )
{
	QCoreApplication app( argc,argv ) ;

	context ctx( app ) ;

	_run_delayed( 0,ctx,_exec ) ;

	return app.exec() ;
}
