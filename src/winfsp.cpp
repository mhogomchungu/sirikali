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

class manageInstances
{
public:
	Task::process::result addInstance( const QString& args,
					   const QByteArray& password,
					   const siritask::options& opts ) ;
	Task::process::result removeInstance( const QString& mountPoint ) ;
	std::vector< QStringList > commands() const ;
	QString volumeProperties( const QString& mountPath ) ;
	void updateVolumeList( std::function< void() > ) ;
private:
	std::tuple< bool,QByteArray,QByteArray > getProcessOutput( QProcess&,bool ) ;
	std::vector< QProcess * > m_instances ;
	std::function< void() > m_updateVolumeList = [](){

		utility::debug() << "booooooo!!!" ;
	} ;
} ;

static SiriKali::Winfsp::manageInstances& _winfspInstances()
{
	static SiriKali::Winfsp::manageInstances _winfsInstances ;
	return _winfsInstances ;
}

bool babySittingBackends()
{
	return true ;
}

void updateVolumeList( std::function< void() > function )
{
	_winfspInstances().updateVolumeList( std::move( function ) ) ;
}

}

}

#ifdef Q_OS_WIN

int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a ) ;
	Q_UNUSED( b ) ;
	Q_UNUSED( c ) ;

	return 0 ;
}

QString SiriKali::Winfsp::readRegister( const char * path,const char * key )
{
	DWORD dwType = REG_SZ ;
	HKEY hKey = 0 ;

	std::array< char,4096 > buffer ;

	std::fill( buffer.begin(),buffer.end(),'\0' ) ;

	auto buff = reinterpret_cast< BYTE * >( buffer.data() ) ;

	auto buffer_size = static_cast< DWORD >( buffer.size() ) ;

	if( RegOpenKey( HKEY_LOCAL_MACHINE,path,&hKey ) == ERROR_SUCCESS ){

		RegQueryValueEx( hKey,key,nullptr,&dwType,buff,&buffer_size ) ;
	}

	RegCloseKey( hKey ) ;

	return QByteArray( buffer.data(),buffer_size ) ;
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

QString SiriKali::Winfsp::readRegister( const char * path,const char * key )
{
	Q_UNUSED( path ) ;
	Q_UNUSED( key ) ;

	return QString() ;
}

#endif

QString SiriKali::Winfsp::sshfsInstallDir()
{
	auto e = "SOFTWARE\\WOW6432Node\\SSHFS-Win" ;
	return SiriKali::Winfsp::readRegister( e,"InstallDir" ) ;
}

QString SiriKali::Winfsp::encfsInstallDir()
{
	auto e = "SOFTWARE\\WOW6432Node\\ENCFS" ;
	return SiriKali::Winfsp::readRegister( e,"InstallDir" ) ;
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

std::vector< QStringList > SiriKali::Winfsp::manageInstances::commands() const
{
	std::vector< QStringList > s ;

	for( const auto& it : m_instances ){

		auto e = it->arguments() ;

		for( auto& m : e ){

			m = mountinfo::encodeMountPath( m ) ;
		}

		s.emplace_back( std::move( e ) ) ;
	}

	return s ;
}

void SiriKali::Winfsp::manageInstances::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

std::tuple< bool,QByteArray,QByteArray >
SiriKali::Winfsp::manageInstances::getProcessOutput( QProcess& exe,bool encfs )
{
	size_t counter = 0 ;

	QByteArray m ;
	/*
	 * Read a full line before processing it
	 */
	while( true ){

		m += exe.readAllStandardOutput() ;

		if( m.contains( "\n" ) ){

			break ;

		}else if( counter < 10 ){

			counter++ ;
			utility::Task::suspendForOneSecond() ;
		}else{
			break ;
		}
	}

	if( utility::containsAtleastOne( m,"init","has been started" ) ){

		return std::make_tuple( true,QByteArray(),QByteArray() ) ;
	}else{
		if( encfs ){

			return std::make_tuple( false,m,QByteArray() ) ;
		}else{
			return std::make_tuple( false,QByteArray(),m ) ;
		}
	}
}

static QProcessEnvironment _update_environment( const QString& type )
{
	auto env = utility::systemEnvironment() ;

	auto m = env.value( "PATH" ) + ";" ;

	auto path = [ & ](){

		if( type == "sshfs" ){

			return m + SiriKali::Winfsp::sshfsInstallDir() + "\\bin" ;

		}else if( type == "encfs" ){

			return m + SiriKali::Winfsp::encfsInstallDir() + "\\bin" ;
		}else{
			return m ;
		}
	}() ;

	env.insert( "PATH",path ) ;

	return env ;
}

Task::process::result SiriKali::Winfsp::FspLaunchRun( const QString& exe,
						      const QByteArray& password,
						      const siritask::options& opts )
{
	if( opts.type == "encfs" ){

		if( utility::createFolder( opts.cipherFolder ) ){

			return SiriKali::Winfsp::FspLaunchStart( exe,password,opts ) ;
		}else{
			auto s = Task::process::result( "","Failed To Create Cipher Folder",1,0,true ) ;

			utility::logCommandOutPut( s,exe ) ;

			return s ;
		}
	}else{
		auto s = Task::process::run( exe,password ).get() ;

		utility::logCommandOutPut( s,exe ) ;

		return s ;
	}
}

Task::process::result SiriKali::Winfsp::manageInstances::addInstance( const QString& args,
								      const QByteArray& password,
								      const siritask::options& opts )
{
	auto exe = utility2::unique_qptr< QProcess >() ;

	auto env = _update_environment( opts.type.name() ) ;

	auto ssh_auth = opts.configFilePath ;

	if( !ssh_auth.isEmpty() ){

		env.insert( "SSH_AUTH_SOCK",ssh_auth ) ;
	}

	exe->setProcessEnvironment( env ) ;
	exe->setProcessChannelMode( QProcess::MergedChannels ) ;
	exe->start( args ) ;
	exe->waitForStarted() ;
	exe->write( password + "\n" ) ;
	exe->closeWriteChannel() ;

	auto m = this->getProcessOutput( *exe,opts.type == "encfs" ) ;

	auto s = [ & ](){

		if( std::get< 0 >( m ) ){

			exe->closeReadChannel( QProcess::StandardError ) ;
			exe->closeReadChannel( QProcess::StandardOutput ) ;

			m_instances.emplace_back( exe.release() ) ;

			m_updateVolumeList() ;

			return Task::process::result( 0 ) ;
		}else{
			exe->waitForFinished() ;

			return Task::process::result( std::get< 1 >( m ),std::get< 2 >( m ),1,0,true ) ;
		}
	}() ;

	utility::logCommandOutPut( s,args ) ;

	return s ;
}

Task::process::result SiriKali::Winfsp::manageInstances::removeInstance( const QString& e )
{
	auto mountPoint = e ;

	mountPoint.replace( "\"","" ) ;

	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		auto e = m_instances[ i ] ;

		const auto cmd = e->program() ;

		auto m = [ & ](){

			if( cmd.endsWith( "encfs.exe" ) ){

				return e->arguments().at( 2 ) ;
			}else{
				return e->arguments().last() ;
			}
		}() ;

		if( m == mountPoint ){

			QString exe ;

			auto env = [ & ](){

				if( cmd.endsWith( "encfs.exe" ) ){

					return _update_environment( "encfs" ) ;
				}else{
					return utility::systemEnvironment() ;
				}
			}() ;

			if( cmd.endsWith( "encfs.exe" ) ){

				exe = "\"" + cmd + "\" -u " + m ;

			}else if( cmd.endsWith( "securefs.exe" ) ){

				exe = "sirikali.exe terminateProcess-" + QString::number( e->processId() ) ;
			}else{
				exe = "taskkill /F /PID " + QString::number( e->processId() ) ;
			}

			auto m = Task::process::run( exe,{},-1,"",env ).await() ;

			auto s = [ & ](){

				if( m.success() ) {

					e->deleteLater() ;

					m_instances.erase( m_instances.begin() + i ) ;

					m_updateVolumeList() ;

					return Task::process::result( 0 ) ;
				}else{
					return Task::process::result( "","Failed To Terminate A Process",1,0,true ) ;
				}
			}() ;

			utility::logCommandOutPut( s,exe ) ;

			return s ;
		}
	}

	return Task::process::result() ;
}

QString SiriKali::Winfsp::manageInstances::volumeProperties( const QString& mountPath )
{
	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		auto args = m_instances[ i ]->arguments() ;

		if( mountPath == args.last() ){

			auto m = QObject::tr( "Mount Options:\n\n" ) ;

			for( const auto& it : args ){

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
	return _winfspInstances().volumeProperties( mountPath ) ;
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

std::vector< QStringList > SiriKali::Winfsp::commands()
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _winfspInstances().commands() ;
	}else{
		return SiriKali::Winfsp::ActiveInstances().commands() ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStop( const QString& m )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _winfspInstances().removeInstance( m ) ;
	}else{
		QStringList e ;
		e.append( m ) ;

		return SiriKali::Winfsp::FspLaunchStop( QString(),QString(),e ) ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStart( const QString& exe,
							const QByteArray& password,
							const siritask::options& opts )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _winfspInstances().addInstance( exe,password,opts ) ;
	}else{
		QStringList e ;
		e.append( exe ) ;

		return SiriKali::Winfsp::FspLaunchStart( QString(),QString(),e,password ) ;
	}
}
