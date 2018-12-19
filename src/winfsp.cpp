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

#include "winfsp.h"
#include "utility.h"
#include "winfsp_impl.hpp"
#include "mountinfo.h"

#include <utility>
#include <tuple>

namespace SiriKali{
namespace Winfsp{

struct Process
{
	Process( const engines::engine::args& args,QProcess * p ) :
		args( args ),instance( p )
	{
	}
	~Process()
	{
		instance->deleteLater() ;
	}
	engines::engine::args args ;
	QProcess * instance ;
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
	std::vector< SiriKali::Winfsp::mountOptions > mountOptions() ;
private:
	std::tuple< bool,QByteArray,QByteArray > getProcessOutput( QProcess&,const QString& ) ;
	std::vector< Process > m_instances ;
	std::function< void() > m_updateVolumeList = [](){

		utility::debug() << "booooooo!!!" ;
	} ;
} ;

static SiriKali::Winfsp::instances& _instances()
{
	static SiriKali::Winfsp::instances s ;
	return s ;
}

bool babySittingBackends()
{
	return true ;
}

void updateVolumeList( std::function< void() > function )
{
	_instances().updateVolumeList( std::move( function ) ) ;
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

int SiriKali::Winfsp::terminateProcess( unsigned long pid )
{
	if( signalCtrl( pid,CTRL_C_EVENT ) ){

		return 0 ;
	}else{
		return 1 ;
	}
}

#else

int SiriKali::Winfsp::terminateProcess( unsigned long pid )
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

QString SiriKali::Winfsp::engineInstalledDir( const QString& e )
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

QStringList SiriKali::Winfsp::engineInstalledDirs()
{
	auto m = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}" ;

	return { _readRegistry( "SOFTWARE\\ENCFS","InstallDir" ),
		 _readRegistry( "SOFTWARE\\SSHFS-Win","InstallDir" ),
		 _readRegistry( "SOFTWARE\\SECUREFS","InstallDir" ),
		 _readRegistry( m,"InstallLocation" )} ;
}

SiriKali::Winfsp::ActiveInstances::ActiveInstances() :
	m_handle( new SiriKali::Winfsp::ActiveInstances::impl() )
{
}

SiriKali::Winfsp::ActiveInstances::~ActiveInstances()
{
}

bool SiriKali::Winfsp::ActiveInstances::valid() const
{
	return m_handle->valid() ;
}

const std::vector< SiriKali::Winfsp::winFsp >& SiriKali::Winfsp::ActiveInstances::values() const
{
	return m_handle->values() ;
}

std::vector< QStringList > SiriKali::Winfsp::ActiveInstances::commands() const
{
	return m_handle->commands() ;
}

std::vector< QStringList > SiriKali::Winfsp::instances::commands() const
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

void SiriKali::Winfsp::instances::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

template< typename Function >
static QByteArray _read_data( QProcess& exe,size_t max,Function function )
{
	size_t counter = 0 ;

	QByteArray m ;
	QByteArray s ;

	while( true ){

		s = exe.readAllStandardOutput() ;

		if( !s.isEmpty() ){

			m += s ;
		}

		if( function( m ) ){

			break ;

		}else if( counter < max ){

			counter++ ;

			utility::Task::suspendForOneSecond() ;
		}else{
			break ;
		}
	}

	return m ;
}

std::tuple< bool,QByteArray,QByteArray >
SiriKali::Winfsp::instances::getProcessOutput( QProcess& exe,const QString& type )
{
	QByteArray m ;

	if( type == "cryfs" ){

		m = _read_data( exe,100,[]( const QString& e ){

			return e.contains( "Error" ) || e.contains( "Mounting filesystem." ) ;
		} ) ;
	}else{
		m = _read_data( exe,10,[]( const QString& e ){

			return e.contains( "\n" ) ;
		} ) ;
	}

	auto a = utility::containsAtleastOne( m,"init","has been started","Mounting filesystem" ) ;

	if( !m.startsWith( "cygfuse: initialization failed:" ) && a ){

		return std::make_tuple( true,QByteArray(),QByteArray() ) ;
	}else{
		if( type == "encfs" ){

			return std::make_tuple( false,m,QByteArray() ) ;
		}else{
			return std::make_tuple( false,QByteArray(),m ) ;
		}
	}
}

static QProcessEnvironment _update_environment( const QString& type )
{
	auto env = utility::systemEnvironment() ;

	auto m = env.value( "PATH" ) ;

	auto path = [ & ](){

		if( type == "sshfs" ){

			return SiriKali::Winfsp::engineInstalledDir( "sshfs" ) + "\\bin;" + m ;

		}else if( type == "encfs" ){

			return SiriKali::Winfsp::engineInstalledDir( "encfs" ) + "\\bin;" + m ;

		}else if( type == "cryfs" ){

			return SiriKali::Winfsp::engineInstalledDir( "cryfs" ) + "\\bin;" + m ;
		}else{
			return m ;
		}
	}() ;

	env.insert( "PATH",path ) ;

	return env ;
}

Task::process::result SiriKali::Winfsp::FspLaunchRun( const engines::engine::args& args,
						      const QByteArray& password,
						      const engines::engine::options& opts )
{
	if( utility::equalsAtleastOne( opts.type,"encfs","cryfs" ) ){

		if( opts.type == "encfs" ){

			if( utility::createFolder( opts.cipherFolder ) ){

				/*
				 * We are intentionally not logging here because the called function
				 * below will do the logging.
				 */
				return SiriKali::Winfsp::FspLaunchStart( args,password,opts ) ;
			}else{
				auto s = Task::process::result( "","Failed To Create Cipher Folder",1,0,true ) ;

				utility::logCommandOutPut( s,args.cmd ) ;

				return s ;
			}

		}else if( opts.type == "cryfs" ){

			if( utility::createFolder( opts.cipherFolder ) ){

				if( utility::createFolder( opts.plainFolder ) ){

					/*
					 * We are intentionally not logging here because the called function
					 * below will do the logging.
					 */
					return SiriKali::Winfsp::FspLaunchStart( args,password,opts ) ;
				}else{
					utility::removeFolder( args.cipherPath ) ;

					auto s = Task::process::result( "","Failed To Create Plain Folder",1,0,true ) ;

					utility::logCommandOutPut( s,args.cmd ) ;

					return s ;
				}
			}else{
				auto s = Task::process::result( "","Failed To Create Cipher Folder",1,0,true ) ;

				utility::logCommandOutPut( s,args.cmd ) ;

				return s ;
			}
		}else{
			auto s = Task::process::result( "","Unknown Engine Encountered",1,0,true ) ;

			utility::logCommandOutPut( s,args.cmd ) ;

			return s ;
		}

	}else{
		auto s = Task::process::run( args.cmd,password ).get() ;

		utility::logCommandOutPut( s,args.cmd ) ;

		return s ;
	}
}

Task::process::result SiriKali::Winfsp::instances::add( const engines::engine::args& args,
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

		if( std::get< 0 >( m ) ){

			exe->closeReadChannel( QProcess::StandardError ) ;
			exe->closeReadChannel( QProcess::StandardOutput ) ;

			m_instances.emplace_back( args,exe.release() ) ;

			m_updateVolumeList() ;

			return Task::process::result( 0 ) ;
		}else{
			exe->waitForFinished() ;

			return Task::process::result( std::get< 1 >( m ),
						      std::get< 2 >( m ),
						      exe->exitCode(),
						      exe->exitStatus(),
						      true ) ;
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


Task::process::result SiriKali::Winfsp::instances::remove( const QString& mountPoint )
{
	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		const auto& s = m_instances[ i ] ;

		if( s.args.mountPath == mountPoint ){

			const auto& e = s.instance ;

			auto cmd = e->program() ;

			QString exe ;

			auto env = [ & ](){

				if( cmd.endsWith( "encfs.exe" ) ){

					return _update_environment( "encfs" ) ;
				}else{
					return utility::systemEnvironment() ;
				}
			}() ;

			if( cmd.endsWith( "encfs.exe" ) ){

				exe = "\"" + cmd + "\" -u " + s.args.mountPath ;

			}else if( cmd.endsWith( "securefs.exe" ) ){

				exe = "sirikali.exe terminateProcess-" + QString::number( e->processId() ) ;
			}else{
				exe = "taskkill /F /PID " + QString::number( e->processId() ) ;
			}

			auto m = Task::process::run( exe,{},-1,"",env ).get() ;

			auto r = [ & ](){

				if( m.success() ) {

					m_instances.erase( m_instances.begin() + i ) ;

					m_updateVolumeList() ;

					if( cmd.endsWith( "cryfs.exe" ) ){

						auto a = _make_path( s.args.mountPath,encode::False ) ;

						utility::removeFolder( a,5 ) ;
					}

					return Task::process::result( 0 ) ;
				}else{
					return Task::process::result( "","Failed To Terminate A Process",1,0,true ) ;
				}
			}() ;

			utility::logCommandOutPut( r,exe ) ;

			return r ;
		}
	}

	return Task::process::result() ;
}

QString SiriKali::Winfsp::instances::volumeProperties( const QString& mountPath )
{
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

QString SiriKali::Winfsp::volumeProperties( const QString& mountPath )
{
	return _instances().volumeProperties( mountPath ) ;
}

Task::process::result SiriKali::Winfsp::FspLaunchStart( const QString& className,
							const QString& instanceName,
							const QStringList& args,
							const QByteArray& password )
{
	Q_UNUSED( className ) ;
	Q_UNUSED( instanceName ) ;

	Q_UNUSED( args ) ;
	Q_UNUSED( password ) ;

	return Task::process::result() ;
}

Task::process::result SiriKali::Winfsp::FspLaunchStop( const QString& className,
						       const QString& instanceName,
						       const QStringList& opts )
{
	Q_UNUSED( className ) ;
	Q_UNUSED( instanceName ) ;
	Q_UNUSED( opts ) ;

	return Task::process::result() ;
}

std::vector< SiriKali::Winfsp::mountOptions > SiriKali::Winfsp::instances::mountOptions()
{
	std::vector< SiriKali::Winfsp::mountOptions > mOpts ;

	for( const auto& it : m_instances ){

		const auto& m = it.args ;

		auto a = _make_path( m.cipherPath,encode::True ) ;
		auto b = _make_path( m.mountPath,encode::True ) ;

		mOpts.emplace_back( m.mode,m.subtype,a,b,m.fuseOptions ) ;
	}

	return mOpts ;
}

std::vector< SiriKali::Winfsp::mountOptions > SiriKali::Winfsp::getMountOptions()
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _instances().mountOptions() ;
	}else{
		return {} ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStop( const QString& m )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _instances().remove( m ) ;
	}else{
		QString x ;
		QStringList y( m ) ;

		return SiriKali::Winfsp::FspLaunchStop( x,x,y ) ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStart( const engines::engine::args& args,
							const QByteArray& password,
							const engines::engine::options& opts )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		if( opts.type == "cryfs" ){

			if( utility::createFolder( opts.plainFolder ) ){

				auto s = _instances().add( args,password,opts ) ;

				if( !s.success() ){

					utility::removeFolder( opts.plainFolder ) ;
				}

				return s ;
			}else{
				auto s = Task::process::result( "","Failed To Create Mount Folder",1,0,true ) ;

				utility::logCommandOutPut( s,args.cmd ) ;

				return s ;
			}
		}else{
			return _instances().add( args,password,opts ) ;
		}

	}else{
		QStringList e ;
		e.append( args.cmd ) ;

		return SiriKali::Winfsp::FspLaunchStart( QString(),QString(),e,password ) ;
	}
}
