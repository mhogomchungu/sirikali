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

#include "engines.h"

#include "engines/ecryptfs.h"
#include "engines/cryfs.h"
#include "engines/gocryptfs.h"
#include "engines/encfs.h"
#include "engines/sshfs.h"
#include "engines/unknown.h"
#include "engines/securefs.h"

#include "utility.h"

engines::engine::~engine()
{
}

engines::engine::engine( const QString& e ) : m_name( e.toLower() )
{
}

QString engines::engine::executableFullPath() const
{
	return utility::executableFullPath( m_name ) ;
}

bool engines::engine::isInstalled() const
{
	return !this->isNotInstalled() ;
}

bool engines::engine::isNotInstalled() const
{
	return this->executableFullPath().isEmpty() ;
}

bool engines::engine::unknown() const
{
	return m_name.isEmpty() ;
}

const engines& engines::instance()
{
	static engines v ;
	return v ;
}

QStringList engines::supported()
{
	if( utility::platformIsWindows() ){

		return { "Securefs","Encfs","Sshfs" } ;

	}else if( utility::platformIsOSX() ){

		return { "Cryfs","Gocryptfs","Securefs","Encfs" } ;
	}else{
		return { "Cryfs","Gocryptfs","Securefs","Encfs","Ecryptfs","Sshfs" } ;
	}
}

engines::engines()
{
	m_backends.emplace_back( std::make_unique< unknown >() ) ;
	m_backends.emplace_back( std::make_unique< securefs >() ) ;
	m_backends.emplace_back( std::make_unique< gocryptfs >() ) ;
	m_backends.emplace_back( std::make_unique< cryfs >() ) ;
	m_backends.emplace_back( std::make_unique< encfs >() ) ;
	m_backends.emplace_back( std::make_unique< ecryptfs >() ) ;
	m_backends.emplace_back( std::make_unique< sshfs >() ) ;
}

const engines::engine& engines::getByName( const siritask::options& e ) const
{
	return this->getByName( e.type.name() ) ;
}

template< typename T,typename Function >
const engines::engine& _get_engine( const T& engines,const QString& e,Function function )
{
	const auto data = engines.data() ;

	for( size_t i = 1 ; i < engines.size() ; i++ ){

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

const engines::engine& engines::getByFuseName( const QString& e ) const
{
	return _get_engine( m_backends,e,[]( const engines::engine& s ){ return s.fuseNames() ; } ) ;
}

const engines::engine& engines::getByName( const QString& e ) const
{
	return _get_engine( m_backends,e.toLower(),[]( const engines::engine& s ){ return s.names() ; } ) ;
}
