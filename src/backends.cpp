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

#include "backends.h"

#include "engines/ecryptfs.h"
#include "engines/cryfs.h"
#include "engines/gocryptfs.h"
#include "engines/encfs.h"
#include "engines/sshfs.h"
#include "engines/unknown.h"
#include "engines/securefs.h"

#include "utility.h"

backEnd::engine::~engine()
{
}

backEnd::engine::engine( const QString& e ) : m_name( e.toLower() )
{
}

QString backEnd::engine::executableFullPath() const
{
	return utility::executableFullPath( m_name ) ;
}

bool backEnd::engine::isInstalled() const
{
	return !this->isNotInstalled() ;
}

bool backEnd::engine::isNotInstalled() const
{
	return this->executableFullPath().isEmpty() ;
}

bool backEnd::engine::unknown() const
{
	return m_name.isEmpty() ;
}

const backEnd& backEnd::instance()
{
	static backEnd v ;
	return v ;
}

QStringList backEnd::supported()
{
	if( utility::platformIsWindows() ){

		return { "Securefs","Encfs","Sshfs" } ;

	}else if( utility::platformIsOSX() ){

		return { "Cryfs","Gocryptfs","Securefs","Encfs" } ;
	}else{
		return { "Cryfs","Gocryptfs","Securefs","Encfs","Ecryptfs","Sshfs" } ;
	}
}

backEnd::backEnd()
{
	m_backends.emplace_back( std::make_unique< unknown >() ) ;
	m_backends.emplace_back( std::make_unique< securefs >() ) ;
	m_backends.emplace_back( std::make_unique< gocryptfs >() ) ;
	m_backends.emplace_back( std::make_unique< cryfs >() ) ;
	m_backends.emplace_back( std::make_unique< encfs >() ) ;
	m_backends.emplace_back( std::make_unique< ecryptfs >() ) ;
	m_backends.emplace_back( std::make_unique< sshfs >() ) ;
}

const backEnd::engine& backEnd::getByName( const siritask::options& e ) const
{
	return this->getByName( e.type.name() ) ;
}

template< typename T,typename Function >
const backEnd::engine& _get_engine( const T& engine,const QString& e,Function function )
{
	auto data = engine.data() ;

	for( size_t i = 1 ; i < engine.size() ; i++ ){

		const auto& s = *( data + i ) ;

		const auto& m = function( *s ) ;

		for( int z = 0 ; z < m.size() ; z++ ){

			if( e == m.at( z ) ){

				return *s ;
			}
		}
	}

	return **data ;
}

const backEnd::engine& backEnd::getByFuseName( const QString& e ) const
{
	return _get_engine( m_backends,e,[]( const backEnd::engine& s ){ return s.fuseNames() ; } ) ;
}

const backEnd::engine& backEnd::getByName( const QString& e ) const
{
	return _get_engine( m_backends,e.toLower(),[]( const backEnd::engine& s ){ return s.names() ; } ) ;
}
