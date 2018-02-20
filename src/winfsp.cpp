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
	QStringList commands() const ;
private:
	std::vector< std::pair< QProcess *,QString > > m_instances ;
} ;

class ActiveInstances
{
public:
	ActiveInstances() ;
	~ActiveInstances() ;
	bool valid() const ;
	const std::vector< SiriKali::Winfsp::winFsp >& values() const ;
	QStringList commands() const ;
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

}

}

#ifdef _WIN32

#include <winfsp/launch.h>
#include <Windows.h>
#include <Winreg.h>

#include <array>
#include <algorithm>

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
	QStringList commands() const
	{
		QStringList s ;

		for( const auto& it : m_entries ){

			s.append( it.command ) ;
		}

		return s ;
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
	std::pair< WCHAR *,int > component( WCHAR * buffer,ULONG size,ULONG e )
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

QString SiriKali::Winfsp::readRegister( const char * path,const char * key )
{
	Q_UNUSED( path ) ;
	Q_UNUSED( key ) ;

	return QString() ;
}

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
	QStringList commands() const
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

QStringList SiriKali::Winfsp::ActiveInstances::commands() const
{
	return m_handle->commands() ;
}

QStringList SiriKali::Winfsp::manageInstances::commands() const
{
	QStringList s ;

	for( const auto& it : m_instances ){

		s.append( it.second ) ;
	}

	return s ;
}

Task::process::result SiriKali::Winfsp::manageInstances::addInstance( const QString& args,
								      const QByteArray& password )
{
	auto exe = new QProcess() ;

	exe->start( args ) ;
	exe->waitForStarted() ;
	exe->write( password + "\n" ) ;
	exe->closeWriteChannel() ;

	QByteArray data ;

	size_t counter = 0 ;

	while( true ){

		data += exe->readAllStandardError() ;

		if( data.contains( "\n" ) ){

			if( data.contains( "init" ) ){

				m_instances.emplace_back( exe,args ) ;

				return Task::process::result( 0 ) ;
			}else{
				return Task::process::result( "",data,1,0,true ) ;
			}
		}else{
			if( counter < 10 ){

				counter++ ;
				utility::Task::suspendForOneSecond() ;
			}else{
				return Task::process::result( "",data,1,0,true ) ;
			}
		}
	}
}

Task::process::result SiriKali::Winfsp::manageInstances::removeInstance( const QString& mountPoint )
{
	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		auto e = m_instances[ i ].first ;

		auto m = "\"" + e->arguments().at( 8 ) + "\"" ;

		if( m == mountPoint ){

			e->terminate() ;

			utility::Task::suspend( 3 ) ;

			e->deleteLater() ;

			m_instances.erase( m_instances.begin() + i ) ;

			return Task::process::result( 0 ) ;
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

QStringList SiriKali::Winfsp::commands()
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
