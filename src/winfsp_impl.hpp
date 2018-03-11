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

#ifndef WINFSP_IMPL_H
#define WINFSP_IMPL_H

#include <vector>
#include <memory>

#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace SiriKali{
namespace Winfsp{

struct winFsp{

	QString className ;
	QString instanceName ;
	QString command ;
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

//#ifndef Q_OS_WIN
#if 1
class ActiveInstances::impl
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

//#ifdef Q_OS_WIN
#if 0
#include <winfsp/launch.h>
#include <Windows.h>
#include <Winreg.h>

#include <array>
#include <algorithm>

class ActiveInstances::impl
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

#endif

}

}

#endif
