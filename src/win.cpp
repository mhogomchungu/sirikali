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

#include <utility>
#include <tuple>

namespace SiriKali{
namespace Windows{

static const char * _backEndTimedOut = "SiriKali::Windows::BackendTimedOut" ;

static void _deleteLater( QObject * e )
{
	e->deleteLater() ;
}

struct Process
{
	Process( const engines::engine::args& args,QProcess * p ) :
		args( args ),instance( p,_deleteLater )
	{
	}
	Process( Process&& ) = default ;
	Process& operator=( Process&& ) = default ;
	engines::engine::args args ;
	std::unique_ptr< QProcess,void(*)( QObject * ) > instance ;
} ;

struct result
{
	result() : success( false ),timeOut( true )
	{
	}
	result( bool s ) : success( s ),timeOut( false )
	{
	}
	result( QByteArray o,QByteArray e ) :
		success( false ),timeOut( false ),
		stdOut( std::move( o ) ),stdError( std::move( e ) )
	{
	}
	bool success ;
	bool timeOut ;
	QByteArray stdOut ;
	QByteArray stdError ;
} ;

class instances
{
public:
	Task::process::result add( const engines::engine::args& args,
				   const QByteArray& password,
				   const engines::engine::options& opts ) ;
	Task::process::result remove( const QString& mountPoint ) ;
	std::vector< QStringList > commands() const ;
	QString volumeProperties( const QString& mountPath ) ;
	void updateVolumeList( std::function< void() > ) ;
	std::vector< SiriKali::Windows::mountOptions > mountOptions() ;
private:
	result getProcessOutput( QProcess&,const QString& ) ;
	std::vector< Process > m_instances ;
	std::function< void() > m_updateVolumeList ;
} ;

static SiriKali::Windows::instances& _instances()
{
	static SiriKali::Windows::instances s ;
	return s ;
}

void updateVolumeList( std::function< void() > function )
{
	_instances().updateVolumeList( std::move( function ) ) ;
}

bool backEndTimedOut( const QString& e )
{
	return e == SiriKali::Windows::_backEndTimedOut ;
}

}

}

#ifdef Q_OS_WIN

#include <windows.h>

int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a ) ;
	Q_UNUSED( b ) ;
	Q_UNUSED( c ) ;

	return 0 ;
}

class regOpenKey{
public:
	regOpenKey( const char * subKey,HKEY hkey = HKEY_LOCAL_MACHINE )
	{
		HKEY m ;
		REGSAM wow64 = KEY_QUERY_VALUE | KEY_WOW64_64KEY ;
		REGSAM wow32 = KEY_QUERY_VALUE | KEY_WOW64_32KEY ;

		if( this->success( RegOpenKeyExA,hkey,subKey,0,wow64,&m ) ){

			m_hkey = m ;

		}else if( this->success( RegOpenKeyExA,hkey,subKey,0,wow32,&m ) ){

			m_hkey = m ;
		}else{
			m_hkey = nullptr ;
		}
	}
	regOpenKey( const regOpenKey& ) = delete ;
	regOpenKey& operator=( const regOpenKey& ) = delete ;
	regOpenKey( regOpenKey&& other )
	{
		this->closeKey() ;
		m_hkey = other.m_hkey ;
		other.m_hkey = nullptr ;
	}
	regOpenKey& operator=( regOpenKey&& other )
	{
		this->closeKey() ;
		m_hkey = other.m_hkey ;
		other.m_hkey = nullptr ;
		return *this ;
	}
	operator bool()
	{
		return m_hkey != nullptr ;
	}
	QByteArray getValue( const char * key )
	{
		if( m_hkey != nullptr ){

			DWORD dwType = REG_SZ ;

			std::array< char,4096 > buffer ;

			std::fill( buffer.begin(),buffer.end(),'\0' ) ;

			auto e = reinterpret_cast< BYTE * >( buffer.data() ) ;
			auto m = static_cast< DWORD >( buffer.size() ) ;

			if( this->success( RegQueryValueEx,m_hkey,key,nullptr,&dwType,e,&m ) ){

				return { buffer.data(),static_cast< int >( m ) } ;
			}
		}

		return {} ;
	}
	HKEY handle()
	{
		return m_hkey ;
	}
	~regOpenKey()
	{
		this->closeKey() ;
	}
private:
	template< typename Function,typename ... Args >
	bool success( Function&& function,Args&& ... args )
	{
		return function( std::forward< Args >( args ) ... ) == ERROR_SUCCESS ;
	}
	void closeKey()
	{
		RegCloseKey( m_hkey ) ;
	}
	HKEY m_hkey ;
};

static QString _readRegistry( const char * subKey,const char * key )
{
	return regOpenKey( subKey ).getValue( key ) ;
}

// SiriKali took below code from "https://stackoverflow.com/questions/813086/can-i-send-a-ctrl-c-sigint-to-an-application-on-windows"

// Inspired from http://stackoverflow.com/a/15281070/1529139
// and http://stackoverflow.com/q/40059902/1529139
static bool signalCtrl(DWORD dwProcessId, DWORD dwCtrlEvent)
{
    bool success = false;
    DWORD thisConsoleId = GetCurrentProcessId();
    // Leave current console if it exists
    // (otherwise AttachConsole will return ERROR_ACCESS_DENIED)
    bool consoleDetached = (FreeConsole() != FALSE);

    if (AttachConsole(dwProcessId) != FALSE)
    {
	// Add a fake Ctrl-C handler for avoid instant kill is this console
	// WARNING: do not revert it or current program will be also killed
	SetConsoleCtrlHandler(nullptr, true);
	success = (GenerateConsoleCtrlEvent(dwCtrlEvent, 0) != FALSE);
	FreeConsole();
    }

    if (consoleDetached)
    {
	// Create a new console if previous was deleted by OS
	if (AttachConsole(thisConsoleId) == FALSE)
	{
	    int errorCode = GetLastError();
	    if (errorCode == 31) // 31=ERROR_GEN_FAILURE
	    {
		AllocConsole();
	    }
	}
    }
    return success;
}

int SiriKali::Windows::terminateProcess( unsigned long pid )
{
	if( signalCtrl( pid,CTRL_C_EVENT ) ){

		return 0 ;
	}else{
		return 1 ;
	}
}

#else

int SiriKali::Windows::terminateProcess( unsigned long pid )
{
	Q_UNUSED( pid ) ;
	return 1 ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	Q_UNUSED( subKey ) ;
	Q_UNUSED( key ) ;
	return QString() ;
}

#endif

QString SiriKali::Windows::engineInstalledDir( const QString& e )
{
	if( utility::equalsAtleastOne( e,"encfs","encfsctl" ) ){

		return _readRegistry( "SOFTWARE\\ENCFS","InstallDir" ) ;

	}else if( e == "sshfs" ){

		return _readRegistry( "SOFTWARE\\SSHFS-Win","InstallDir" ) ;

	}else if( e == "securefs" ){

		return _readRegistry( "SOFTWARE\\SECUREFS","InstallDir" ) ;

	}else if( e == "cryfs" ){

		return _readRegistry( "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}","InstallLocation" ) ;
	}else{
		return QString() ;
	}
}

QStringList SiriKali::Windows::engineInstalledDirs()
{
	auto m = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}" ;

	return { _readRegistry( "SOFTWARE\\ENCFS","InstallDir" ),
		 _readRegistry( "SOFTWARE\\SSHFS-Win","InstallDir" ),
		 _readRegistry( "SOFTWARE\\SECUREFS","InstallDir" ),
		 _readRegistry( m,"InstallLocation" ) } ;
}

std::vector< QStringList > SiriKali::Windows::instances::commands() const
{
	std::vector< QStringList > s ;

	for( const auto& it : m_instances ){

		auto e = it.instance->arguments() ;

		for( auto& m : e ){

			m = mountinfo::encodeMountPath( m ) ;
		}

		s.emplace_back( std::move( e ) ) ;
	}

	return s ;
}

void SiriKali::Windows::instances::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

template< typename Function >
static std::pair< bool,QByteArray > _read( QProcess& exe,Function function )
{
	QByteArray m ;
	QByteArray s ;

	int counter = 1 ;

	auto _log_and_return = [ & ]( std::pair< bool,QByteArray > x ){

		utility::debug() << "Backend took " + QString::number( counter ) + " seconds to finish" ;

		return x ;
	} ;

	while( true ){

		s = exe.readAllStandardOutput() ;

		if( !s.isEmpty() ){

			m += s ;
		}

		auto r = function( m ) ;

		if( r.first ){

			return _log_and_return( { true,QByteArray() } ) ;

		}else if( r.second ){

			return _log_and_return( { false,m } ) ;
		}else{
			utility::Task::suspendForOneSecond() ;
			counter++ ;
		}
	}

	// Should not get here
	return { true,QByteArray() } ;
}

std::pair< bool,QByteArray > _get_output( QProcess& exe,const QString& type )
{
	if( type == "cryfs" ){

		return _read( exe,[]( const QString& e )->std::pair< bool,bool >{

			return { false,e.contains( "Error" ) || e.contains( "Mounting filesystem." ) } ;
		} ) ;

	}else if( type == "encfs" ){

		int timeout = 20 ;
		int counter = 0 ;

		return _read( exe,[ & ]( const QString& e )->std::pair< bool,bool >{

			bool Continue = counter < timeout ;

			counter++ ;

			if( Continue ){

				return { false,e.contains( "\n" ) } ;
			}else{
				return { true,false } ;
			}
		} ) ;
	}else{
		return _read( exe,[]( const QString& e )->std::pair< bool,bool >{

			return { false,e.contains( "\n" ) } ;
		} ) ;
	}
}

SiriKali::Windows::result SiriKali::Windows::instances::getProcessOutput( QProcess& exe,const QString& type )
{
	std::pair< bool,QByteArray > m = _get_output( exe,type ) ;

	if( m.first ){

		/*
		 * Time out has occured
		 */
		return {} ;
	}

	const auto& mm = m.second ;

	auto a = utility::containsAtleastOne( mm,"init","has been started","Mounting filesystem" ) ;

	if( !mm.startsWith( "cygfuse: initialization failed:" ) && a ){

		return { true } ;
	}else{
		if( type == "encfs" ){

			return { QByteArray(),mm } ;
		}else{
			return { mm,QByteArray() } ;
		}
	}
}

static QProcessEnvironment _update_environment( const QString& type )
{
	auto env = utility::systemEnvironment() ;

	auto m = env.value( "PATH" ) ;

	auto path = [ & ](){

		if( type == "sshfs" ){

			return SiriKali::Windows::engineInstalledDir( "sshfs" ) + "\\bin;" + m ;

		}else if( type == "encfs" ){

			return SiriKali::Windows::engineInstalledDir( "encfs" ) + "\\bin;" + m ;

		}else if( type == "cryfs" ){

			return SiriKali::Windows::engineInstalledDir( "cryfs" ) + "\\bin;" + m ;
		}else{
			return m ;
		}
	}() ;

	env.insert( "PATH",path ) ;

	return env ;
}

Task::process::result SiriKali::Windows::create( const engines::engine::args& args,
						 const QByteArray& password,
						 const engines::engine::options& opts,
						 bool autoMountsOnSuccess )
{
	if( autoMountsOnSuccess ){

		return SiriKali::Windows::mount( args,password,opts ) ;
	}else{
		auto s = utility::unwrap( Task::process::run( args.cmd,password ) ) ;

		utility::logCommandOutPut( s,args.cmd ) ;

		return s ;
	}
}

static std::pair< Task::process::result,QString > _terminate_process( QProcess& e )
{
	auto cmd = e.program() ;

	QString exe ;

	auto env = [ & ](){

		if( cmd.endsWith( "encfs.exe" ) ){

			return _update_environment( "encfs" ) ;
		}else{
			return utility::systemEnvironment() ;
		}
	}() ;

	//if( cmd.endsWith( "encfs.exe" ) ){

	//	exe = "\"" + cmd + "\" -u " + s.args.mountPath ;

	//}
	if( cmd.endsWith( "securefs.exe" ) ){

		exe = "sirikali.exe terminateProcess-" + QString::number( e.processId() ) ;
	}else{
		exe = "taskkill /F /PID " + QString::number( e.processId() ) ;
	}

	return { utility::unwrap( Task::process::run( exe,{},-1,"",env ) ),exe } ;
}

Task::process::result SiriKali::Windows::instances::add( const engines::engine::args& args,
							 const QByteArray& password,
							 const engines::engine::options& opts )
{
	auto exe = utility2::unique_qptr< QProcess >() ;

	exe->setProcessEnvironment( _update_environment( opts.type ) ) ;
	exe->setProcessChannelMode( QProcess::MergedChannels ) ;
	exe->start( args.cmd ) ;
	exe->waitForStarted() ;
	exe->write( password + "\n" ) ;
	exe->closeWriteChannel() ;

	auto m = this->getProcessOutput( *exe,opts.type ) ;

	auto s = [ & ](){

		if( m.timeOut ){

			_terminate_process( *exe ) ;

			exe->waitForFinished() ;

			return Task::process::result( SiriKali::Windows::_backEndTimedOut,
						      QByteArray(),
						      -1,
						      0,
						      false ) ;
		}else if( m.success ){

			exe->closeReadChannel( QProcess::StandardError ) ;
			exe->closeReadChannel( QProcess::StandardOutput ) ;

			m_instances.emplace_back( args,exe.release() ) ;

			m_updateVolumeList() ;

			return Task::process::result( 0 ) ;
		}else{
			exe->waitForFinished() ;

			return Task::process::result( m.stdOut,
						      m.stdError,
						      exe->exitCode(),
						      exe->exitStatus(),
						      m.success ) ;
		}
	}() ;

	utility::logCommandOutPut( s,args.cmd ) ;

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

Task::process::result SiriKali::Windows::instances::remove( const QString& mountPoint )
{
	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		const auto& s = m_instances[ i ] ;

		if( s.args.mountPath == mountPoint ){

			auto m = _terminate_process( *s.instance ) ;

			auto r = [ & ](){

				if( m.first.success() ) {

					m_instances.erase( m_instances.begin() + static_cast< int >( i ) ) ;

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

QString SiriKali::Windows::instances::volumeProperties( const QString& mm )
{
	auto mountPath = "\"" + mm + "\"" ;

	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		const auto& e = m_instances[ i ] ;

		if( mountPath == e.args.mountPath ){

			auto m = QObject::tr( "Mount Options:\n\n" ) ;

			for( const auto& it : e.instance->arguments() ){

				if( it != "-o" ){

					m += it + "\n" ;
				}
			}

			return m ;
		}
	}

	return QString() ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::instances::mountOptions()
{
	std::vector< SiriKali::Windows::mountOptions > mOpts ;

	for( const auto& it : m_instances ){

		const auto& m = it.args ;

		auto a = _make_path( m.cipherPath,encode::True ) ;
		auto b = _make_path( m.mountPath,encode::True ) ;

		mOpts.emplace_back( m.mode,m.subtype,a,b,m.fuseOptions ) ;
	}

	return mOpts ;
}

QString SiriKali::Windows::volumeProperties( const QString& mountPath )
{
	return _instances().volumeProperties( mountPath ) ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::getMountOptions()
{
	return _instances().mountOptions() ;
}

Task::process::result SiriKali::Windows::unmount( const QString& m )
{
	return _instances().remove( m ) ;
}

Task::process::result SiriKali::Windows::mount( const engines::engine::args& args,
						const QByteArray& password,
						const engines::engine::options& opts )
{
	return _instances().add( args,password,opts ) ;
}
