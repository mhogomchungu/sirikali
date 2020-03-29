/*
 *
 *  Copyright (c) 2018
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

#include "win.h"
#include "utility.h"
#include "mountinfo.h"
#include "settings.h"

#include <utility>
#include <tuple>
#include <QtGlobal>

#include <QString>

namespace SiriKali{
namespace Windows{
static const char * _backEndTimedOut = "SiriKali::Windows::BackendTimedOut" ;

#ifdef Q_OS_WIN

#include <windows.h>

static int _terminateProcess( unsigned long pid )
{
	FreeConsole() ;

	if( AttachConsole( pid ) == TRUE ) {

		/*
		 * Add a fake Ctrl-C handler for avoid instant kill in this console
		 * WARNING: do not revert it or current program will also killed
		 */

		SetConsoleCtrlHandler( nullptr,true ) ;

		if( GenerateConsoleCtrlEvent( CTRL_C_EVENT,0 ) == TRUE ){

			return 0 ;
		}
	}

	return 1 ;
}

static HKEY _reg_open_key( const char * subKey )
{
	HKEY hkey = HKEY_LOCAL_MACHINE ;
	HKEY m ;
	REGSAM wow64 = KEY_QUERY_VALUE | KEY_WOW64_64KEY ;
	REGSAM wow32 = KEY_QUERY_VALUE | KEY_WOW64_32KEY ;
	unsigned long x = 0 ;

	if( RegOpenKeyExA( hkey,subKey,x,wow64,&m ) == ERROR_SUCCESS ){

		return m ;

	}else if( RegOpenKeyExA( hkey,subKey,x,wow32,&m ) == ERROR_SUCCESS ){

		return m ;
	}else{
		return nullptr ;
	}
}

static void _reg_close_key( HKEY hkey )
{
	if( hkey != nullptr ){

		RegCloseKey( hkey ) ;
	}
}

static QByteArray _reg_get_value( HKEY hkey,const char * key )
{
	if( hkey != nullptr ){

		DWORD dwType = REG_SZ ;

		std::array< char,4096 > buffer ;

		std::fill( buffer.begin(),buffer.end(),'\0' ) ;

		auto e = reinterpret_cast< BYTE * >( buffer.data() ) ;
		auto m = static_cast< DWORD >( buffer.size() ) ;

		if( RegQueryValueEx( hkey,key,nullptr,&dwType,e,&m ) == ERROR_SUCCESS ){

			return { buffer.data(),static_cast< int >( m ) } ;
		}
	}

	return {} ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	auto s = utility2::unique_rsc( _reg_open_key,_reg_close_key,subKey ) ;

	return _reg_get_value( s.get(),key ) ;
}

#else

static int _terminateProcess( unsigned long pid )
{
	Q_UNUSED( pid )
	return 1 ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	Q_UNUSED( subKey )
	Q_UNUSED( key )
	return QString() ;
}

#endif

class Process
{
public:
	template< typename T >
	Process( const SiriKali::Windows::opts& opts,T x ) :
		m_args( opts.args ),
		m_engine( opts.engine ),
		m_environment( m_engine->getProcessEnvironment() ), //we get this variable now because sshfs backend can change it under us.
		m_instance( std::move( x ) )
	{
	}
	Process( Process&& ) = default ;
	Process& operator=( Process&& ) = default ;
	const QString& mountPoint() const
	{
		return m_args.mountPath ;
	}
	const QProcessEnvironment& env() const
	{
		return m_environment ;
	}
	//const engines::engine& engine() const
	//{
	//	return m_engine.get() ;
	//}
	const engines::engine::args& args() const
	{
		return m_args ;
	}
	QProcess& exe() const
	{
		return *m_instance ;
	}
	QStringList arguments() const
	{
		return this->exe().arguments() ;
	}
	bool running() const
	{
		return this->exe().state() == QProcess::Running ;
	}
	bool notRunning() const
	{
		return this->exe().state() == QProcess::NotRunning ;
	}
private:
	engines::engine::args m_args ;
	engines::engine::Wrapper m_engine ;
	QProcessEnvironment m_environment ;
	std::unique_ptr< QProcess,void(*)( QProcess * ) > m_instance ;
} ;

struct result
{
	engines::engine::error type ;
	QByteArray outPut ;
} ;

class volumes
{
public:
	static SiriKali::Windows::volumes& get()
	{
		static SiriKali::Windows::volumes s ;
		return s ;
	}
	Task::process::result add( const SiriKali::Windows::opts& ) ;
	Task::process::result remove(const QString& unMountCommand,const QString& mountPoint ) ;
	std::vector< QStringList > commands() const ;
	QString volumeProperties( const QString& mountPath ) ;
	void updateVolumeList( std::function< void() > ) ;
	std::vector< SiriKali::Windows::mountOptions > mountOptions() ;
	bool mountPointTaken( const QString& e ) ;
private:
	std::vector< Process > m_instances ;
	std::function< void() > m_updateVolumeList ;
} ;

bool backEndTimedOut( const QString& e )
{
	return e == SiriKali::Windows::_backEndTimedOut ;
}

}

}

int SiriKali::Windows::terminateProcess( unsigned long pid )
{
	return _terminateProcess( pid ) ;
}

QString SiriKali::Windows::engineInstalledDir( const engines::engine& m )
{
	const auto& key   = m.windowsInstallPathRegistryKey() ;
	const auto& value = m.windowsInstallPathRegistryValue() ;

	if( key.isEmpty() || value.isEmpty() ){

		return {} ;
	}

	return SiriKali::Windows::engineInstalledDir( key,value ) ;
}

QString SiriKali::Windows::engineInstalledDir( const QString& key,const QString& value )
{
	return _readRegistry( key.toLatin1().constData(),value.toLatin1().constData() ) ;
}

QStringList SiriKali::Windows::engineInstalledDirs()
{
	QStringList s ;

	for( const auto& it : engines::instance().supportedEngines() ){

		auto a = SiriKali::Windows::engineInstalledDir( it.get() ) ;

		if( !a.isEmpty() ){

			s.append( a ) ;
		}
	}

	return s ;
}

std::vector< QStringList > SiriKali::Windows::volumes::commands() const
{
	std::vector< QStringList > s ;

	for( const auto& it : m_instances ){

		auto e = it.arguments() ;

		for( auto& m : e ){

			m = mountinfo::encodeMountPath( m ) ;
		}

		s.emplace_back( std::move( e ) ) ;
	}

	return s ;
}

void SiriKali::Windows::volumes::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

template< typename Function >
static SiriKali::Windows::result _read( QProcess& exe,Function function )
{
	QByteArray m ;
	QByteArray s ;

	int counter = 1 ;
	int notRunningCounter = 0 ;
	int maxNotRunningCounter = 2 ;

	engines::engine::error r ;

	while( true ){

		if( exe.state() == QProcess::NotRunning ){

			utility::debug() << "warning, a process is no longer running" ;

			if( notRunningCounter < maxNotRunningCounter ){

				notRunningCounter++ ;
			}else{
				m = m + "\n-------------\n\nSiriKali::Error Backend Disappeared For Some Reason (It Probably Crashed)." ;

				r = engines::engine::error::Failed ;

				break ;
			}
		}

		s = exe.readAllStandardOutput() ;

		if( !s.isEmpty() ){

			m += s ;
		}

		if( m.contains( "cygfuse: " ) || m.contains( "Cannot create WinFsp-FUSE file system:" ) ){

			r = engines::engine::error::Failed ;

			break ;
		}else{
			r = function( m ) ;

			if( r == engines::engine::error::Continue ){

				utility::Task::suspendForOneSecond() ;
				counter++ ;
			}else{
				break ;
			}
		}
	}

	utility::debug() << "Backend took " + QString::number( counter ) + " seconds to unlock a volume" ;

	return { r,std::move( m ) } ;
}

static SiriKali::Windows::result _getProcessOutput( QProcess& exe,const engines::engine& engine )
{
	int timeOut = engine.backendTimeout() ;

	if( timeOut > 0 ){

		int counter = 0 ;

		return _read( exe,[ & ]( const QString& e ){

			if( counter < timeOut ){

				counter++ ;
				return engine.errorCode( e ) ;
			}else{
				return engines::engine::error::Timeout ;
			}
		} ) ;
	}else{
		return _read( exe,[ & ]( const QString& e ){

			return engine.errorCode( e ) ;
		} ) ;
	}
}

struct terminate_process{

	QProcess& exe ;
	const QProcessEnvironment& env ;
	const QString& mountPath ;
	const QString& unMountCommand ;
};

static std::pair< Task::process::result,QString > _terminate_process( const terminate_process& e )
{
	QString exe ;

	if( e.unMountCommand == "sirikali.exe -T" ){

		exe = e.unMountCommand + QString::number( e.exe.processId() ) ;

	}else if( e.unMountCommand.isEmpty() || e.mountPath.isEmpty() ){

		exe = "taskkill /F /PID " + QString::number( e.exe.processId() ) ;
	}else{
		exe = utility::Task::makePath( e.unMountCommand ) + " " + e.mountPath ;
	}

	auto m = utility::unwrap( Task::process::run( exe,{},-1,"",e.env ) ) ;

	if( m.success() ){

		utility::waitForFinished( e.exe ) ;
	}

	return { std::move( m ),std::move( exe ) } ;
}

Task::process::result SiriKali::Windows::volumes::add( const SiriKali::Windows::opts& opts )
{
	auto exe = utility2::unique_qptr< QProcess >() ;

	exe->setProcessEnvironment( opts.engine.getProcessEnvironment() ) ;
	exe->setProcessChannelMode( QProcess::MergedChannels ) ;
	exe->start( opts.args.cmd ) ;
	exe->waitForStarted() ;
	exe->write( opts.password + "\n" ) ;
	exe->closeWriteChannel() ;

	auto m = _getProcessOutput( *exe,opts.engine ) ;

	auto s = [ & ](){

		if( m.type == engines::engine::error::Timeout ){

			_terminate_process( { *exe,opts.engine.getProcessEnvironment(),QString(),QString() } ) ;

			return Task::process::result( SiriKali::Windows::_backEndTimedOut,
						      QByteArray(),
						      -1,
						      0,
						      true ) ;

		}else if( m.type == engines::engine::error::Success ){

			exe->closeReadChannel( QProcess::StandardError ) ;
			exe->closeReadChannel( QProcess::StandardOutput ) ;

			m_instances.emplace_back( opts,std::move( exe ) ) ;

			m_updateVolumeList() ;

			return Task::process::result( 0 ) ;
		}else{
			utility::waitForFinished( *exe ) ;

			return Task::process::result( QByteArray(),
						      m.outPut,
						      exe->exitCode(),
						      exe->exitStatus(),
						      true ) ;
		}
	}() ;

	utility::logCommandOutPut( s,opts.args.cmd ) ;

	return s ;
}

enum class encode{ True,False } ;
static QString _make_path( QString e,encode s )
{
	if( e.startsWith( "\"" ) ){

		e.remove( 0,1 ) ;
	}

	if( e.endsWith( "\"" ) ){

		e.truncate( e.size() -1 ) ;
	}

	if( s == encode::True ){

		return mountinfo::encodeMountPath( e ) ;
	}else{
		return e ;
	}
}

Task::process::result
SiriKali::Windows::volumes::remove( const QString& unMountCommand,const QString& mountPoint )
{
	for( auto it = m_instances.begin() ; it != m_instances.end() ; it++ ){

		const auto& s = *it ;

		if( s.mountPoint() == mountPoint ){

			auto m = _terminate_process( { s.exe(),s.env(),s.mountPoint(),unMountCommand } ) ;

			auto r = [ & ](){

				if( m.first.success() ) {

					m_instances.erase( it ) ;

					m_updateVolumeList() ;

					return Task::process::result( 0 ) ;
				}else{
					return Task::process::result( "","Failed To Terminate A Process",1,0,true ) ;
				}
			}() ;

			utility::logCommandOutPut( r,m.second ) ;

			return r ;
		}
	}

	return Task::process::result() ;
}

QString SiriKali::Windows::volumes::volumeProperties( const QString& mm )
{
	auto mountPath = "\"" + mm + "\"" ;

	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		const auto& e = m_instances[ i ] ;

		if( mountPath == e.mountPoint() ){

			auto m = QObject::tr( "Mount Options:\n\n" ) ;

			for( const auto& it : e.arguments() ){

				if( it != "-o" ){

					m += it + "\n" ;
				}
			}

			return m ;
		}
	}

	return QString() ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::volumes::mountOptions()
{
	std::vector< SiriKali::Windows::mountOptions > mOpts ;

	auto _remove_inactive = [ & ]{

		for( auto it = m_instances.begin() ; it != m_instances.end() ; it++ ){

			if( ( *it ).notRunning() ){

				m_instances.erase( it ) ;

				return true ;
			}
		}

		return false ;
	} ;

	while( _remove_inactive() ){}

	for( const auto& it : m_instances ){

		const auto& m = it.args() ;

		auto a = _make_path( m.cipherPath,encode::True ) ;
		auto b = _make_path( m.mountPath,encode::True ) ;

		mOpts.emplace_back( m.mode,m.subtype,a,b,m.fuseOptions ) ;
	}

	return mOpts ;
}

bool SiriKali::Windows::volumes::mountPointTaken( const QString& ee )
{
	auto e = "\"" + QDir::toNativeSeparators( ee ) + "\"" ;

	for( const auto& it : m_instances ){

		if( QDir::toNativeSeparators( it.mountPoint() ) == e ){

			return true ;
		}
	}

	return false ;
}

QString SiriKali::Windows::volumeProperties( const QString& mountPath )
{
	return volumes::get().volumeProperties( mountPath ) ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::getMountOptions()
{
	return volumes::get().mountOptions() ;
}

Task::process::result SiriKali::Windows::unmount( const QString& unMountCommand,const QString& m )
{
	return volumes::get().remove( unMountCommand,m ) ;
}

Task::process::result SiriKali::Windows::run( const opts& s )
{
	if( s.create ){

		if( s.engine.autoMountsOnCreate() ){

			return SiriKali::Windows::volumes::get().add( s ) ;
		}else{
			auto m = utility::unwrap( Task::process::run( s.args.cmd,s.password ) ) ;

			utility::logCommandOutPut( m,s.args.cmd ) ;

			return m ;
		}
	}else{
		return SiriKali::Windows::volumes::get().add( s ) ;
	}
}

void SiriKali::Windows::updateVolumeList( std::function< void() > function )
{
	volumes::get().updateVolumeList( std::move( function ) ) ;
}

bool SiriKali::Windows::mountPointTaken( const QString& e )
{
	return volumes::get().mountPointTaken( e ) ;
}
