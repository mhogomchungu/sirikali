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

#include <utility>

namespace SiriKali{
namespace Winfsp{

struct winFsp{

	QString className ;
	QString instanceName ;
	QString command ;
} ;

class manageInstances
{
public:
	Task::process::result addInstance( const QString& args,const QByteArray& password ) ;
	Task::process::result removeInstance( const QString& mountPoint ) ;
	std::vector< QStringList > commands() const ;
	void updateVolumeList( std::function< void() > ) ;
private:
	std::pair< bool,QByteArray > waitForStarted( QProcess * ) ;
	std::vector< QProcess * > m_instances ;
	std::function< void() > m_updateVolumeList ;
} ;

class ActiveInstances
{
public:
	ActiveInstances() ;
	~ActiveInstances() ;
	bool valid() const ;
	const std::vector< SiriKali::Winfsp::winFsp >& values() const ;
	std::vector< QStringList > commands() const ;
private:
	class impl ;
	std::unique_ptr< impl > m_handle ;
} ;

static SiriKali::Winfsp::manageInstances _winfsInstances ;
static const bool _babySittingSecurefs = true ;

bool babySittingBackends()
{
	return utility::platformIsWindows() && _babySittingSecurefs ;
}

void updateVolumeList( std::function< void() > function )
{
	_winfsInstances.updateVolumeList( std::move( function ) ) ;
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

//#ifdef Q_OS_WIN
#if 0
#include <winfsp/launch.h>
#include <Windows.h>
#include <Winreg.h>

#include <array>
#include <algorithm>

class SiriKali::Winfsp::ActiveInstances::impl
{
public:
	impl()
	{
		for( const auto& it : this->nameList() ){

			this->addToList( it ) ;
		}
	}
	bool valid() const
	{
		return m_error == 0 ;
	}
	const std::vector< SiriKali::Winfsp::winFsp >& values() const
	{
		return m_entries ;
	}
	std::vector< QStringList > commands() const
	{
		return {} ;
	}
private:
	void addToList( const SiriKali::Winfsp::winFsp& e )
	{
		std::array< WCHAR,BUFFER_SIZE > buffer ;

		auto size = static_cast< ULONG >( buffer.size() ) ;

		std::array< WCHAR,BUFFER_SIZE > className ;
		std::array< WCHAR,BUFFER_SIZE > instanceName ;

		e.className.toWCharArray( className.data() ) ;
		e.instanceName.toWCharArray( instanceName.data() ) ;

		FspLaunchGetInfo( className.data(),
				  instanceName.data(),
				  buffer.data(),
				  &size,
				  &m_error) ;

		size = size / sizeof( WCHAR ) ;

		if( m_error == 0 ){

			this->build( m_entries,buffer.data(),size,3 ) ;
		}
	}
	std::vector< SiriKali::Winfsp::winFsp > nameList()
	{
		std::array< WCHAR,BUFFER_SIZE > buffer ;

		auto size = static_cast< ULONG >( buffer.size() ) ;

		std::vector< SiriKali::Winfsp::winFsp > entries ;

		FspLaunchGetNameList( buffer.data(),&size,&m_error) ;

		size = size / sizeof( WCHAR ) ;

		if( m_error == 0 ){

			this->build( entries,buffer.data(),size,2 ) ;
		}

		return entries ;
	}
	std::pair< WCHAR *,ULONG > component( WCHAR * buffer,ULONG size,ULONG e )
	{
		ULONG i = e ;

		while( true ){

			if( e >= size ){

				return { nullptr,0 } ;

			}else if( buffer[ e ] == 0 ){

				return { &buffer[ i ],e + 1 } ;
			}else{
				e++ ;
			}
		}
	}
	void build( std::vector< SiriKali::Winfsp::winFsp >& entries,
		     WCHAR * buffer,ULONG size,int count )
	{
		ULONG i = 0 ;

		while( true ){

			auto s = this->component( buffer,size,i ) ;

			if( s.first == nullptr ){

				return ;
			}

			auto e = this->component( buffer,size,s.second ) ;

			if( e.first == nullptr ){

				return ;
			}

			if( count == 2 ){

				auto a = QString::fromWCharArray( s.first ) ;
				auto b = QString::fromWCharArray( e.first ) ;

				entries.emplace_back( SiriKali::Winfsp::winFsp{ a,b,QString() } ) ;

				i = e.second ;
			}else{
				auto m = this->component( buffer,size,e.second ) ;

				if( m.first == nullptr ){

					return ;
				}

				auto a = QString::fromWCharArray( s.first ) ;
				auto b = QString::fromWCharArray( e.first ) ;
				auto c = QString::fromWCharArray( m.first ) ;

				entries.emplace_back( SiriKali::Winfsp::winFsp{ a,b,c } ) ;

				i = m.second ;
			}
		}
	}
	static const size_t BUFFER_SIZE = 4096 ;
	ULONG m_error ;
	std::vector< SiriKali::Winfsp::winFsp > m_entries ;
} ;

#else

class SiriKali::Winfsp::ActiveInstances::impl
{
public:
	impl()
	{
	}
	bool valid() const
	{
		return false ;
	}
	const std::vector< SiriKali::Winfsp::winFsp >& values() const
	{
		return m_entries ;
	}
	std::vector< QStringList > commands() const
	{
		return {} ;
	}
private:
	std::vector< SiriKali::Winfsp::winFsp > m_entries ;
};

#endif

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

			/*
			 * linux's /proc/self/mountinfo makes these substitutions and we make
			 * them too to be consistent with linux
			 */
			//m.replace( "\n","\\012" ) ;
			m.replace( " ","\\040" ) ;
			//m.replace( "\\","\\134" ) ;
			//m.replace( "\\t","\\011" ) ;
		}

		s.emplace_back( std::move( e ) ) ;
	}

	return s ;
}

void SiriKali::Winfsp::manageInstances::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

std::pair< bool,QByteArray > SiriKali::Winfsp::manageInstances::waitForStarted( QProcess * exe )
{
	size_t counter = 0 ;

	QByteArray data ;

	while( true ){

		data += exe->readAllStandardError() ;

		if( data.contains( "\n" ) ){

			if( utility::containsAtleastOne( data,"init","The service securefs has been started" ) ){

				return { true,QByteArray() } ;
			}else{
				return { false,data } ;
			}
		}else{
			if( counter < 10 ){

				counter++ ;
				utility::Task::suspendForOneSecond() ;
			}else{
				return { false,data } ;
			}
		}
	}
}

Task::process::result SiriKali::Winfsp::manageInstances::addInstance( const QString& args,
								      const QByteArray& password )
{
	auto exe = new QProcess() ;

	exe->start( args ) ;
	exe->waitForStarted() ;
	exe->write( password + "\n" ) ;
	exe->closeWriteChannel() ;

	auto m = this->waitForStarted( exe ) ;

	if( m.first ){

		m_instances.emplace_back( exe ) ;

		m_updateVolumeList() ;

		return Task::process::result( 0 ) ;
	}else{
		return Task::process::result( "",m.second,1,0,true ) ;
	}
}

Task::process::result SiriKali::Winfsp::manageInstances::removeInstance( const QString& mountPoint )
{
	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		auto e = m_instances[ i ] ;

		auto m = "\"" + e->arguments().at( 8 ) + "\"" ;

		if( m == mountPoint ){

			auto exe = "sirikali.exe terminateProcess-" + QString::number( e->processId() ) ;

			auto m = Task::process::run( exe ).await() ;

			if( m.success() ) {

				e->deleteLater() ;

				m_instances.erase( m_instances.begin() + i ) ;

				m_updateVolumeList() ;

				return Task::process::result( 0 ) ;
			}else{
				return Task::process::result( "","",1,0,true ) ;
			}
		}
	}

	return Task::process::result() ;
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

		return _winfsInstances.commands() ;
	}else{
		return SiriKali::Winfsp::ActiveInstances().commands() ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStop( const QString& m )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _winfsInstances.removeInstance( m ) ;
	}else{
		QStringList e ;
		e.append( m ) ;

		return SiriKali::Winfsp::FspLaunchStop( QString(),QString(),e ) ;
	}
}

Task::process::result SiriKali::Winfsp::FspLaunchStart( const QString& exe,const QByteArray& password )
{
	if( SiriKali::Winfsp::babySittingBackends() ){

		return _winfsInstances.addInstance( exe,password ) ;
	}else{
		QStringList e ;
		e.append( exe ) ;

		return SiriKali::Winfsp::FspLaunchStart( QString(),QString(),e,password ) ;
	}
}
