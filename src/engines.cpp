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

bool engines::engine::known() const
{
	return !this->unknown() ;
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

const engines::engine& engines::getByName( const engines::engine::options& e ) const
{
	return this->getByName( e.type ) ;
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

engines::engine::cmdStatus::cmdStatus()
{
}

engines::engine::cmdStatus::cmdStatus( const engines::engine::cmdStatus& s,const QString& e )
{
	m_status = s.status() ;
	m_exitCode = s.exitCode() ;

	if( e.isEmpty() ){

		this->message( s.msg() ) ;
	}else{
		this->message( e ) ;
	}
}

engines::engine::cmdStatus::cmdStatus(engines::engine::status s,int c,const QString& e ) :
	m_exitCode( c ),m_status( s )
{
	this->message( e ) ;
}

engines::engine::cmdStatus::cmdStatus( engines::engine::status s,const QString& e ) :
	m_status( s )
{
	this->message( e ) ;
}

engines::engine::cmdStatus::cmdStatus( int s,const QString& e ) :
	m_exitCode( s )
{
	this->message( e ) ;
}

engines::engine::status engines::engine::cmdStatus::status() const
{
	return m_status ;
}

bool engines::engine::cmdStatus::operator==( engines::engine::status s ) const
{
	return m_status == s ;
}

bool engines::engine::cmdStatus::operator!=( engines::engine::status s ) const
{
	return m_status != s ;
}

engines::engine::cmdStatus& engines::engine::cmdStatus::setExitCode( int s )
{
	m_exitCode = s ;
	return *this ;
}

engines::engine::cmdStatus& engines::engine::cmdStatus::setStatus( engines::engine::status s )
{
	m_status = s ;
	return *this ;
}

engines::engine::cmdStatus& engines::engine::cmdStatus::setMessage( const QString& e )
{
	this->message( e ) ;
	return *this ;
}

const QString& engines::engine::cmdStatus::msg() const
{
	return m_message ;
}

QString engines::engine::cmdStatus::report( const QString& cmd ) const
{
	auto s = QString::number( m_exitCode ) ;

	QString e ;

	e += "-------------------------" ;
	e += QString( "\nBackend Generated Output:\nExit Code: %1" ).arg( s ) ;

	if( !m_message.isEmpty() ){

		e += QString( "\nExit String: \"%1\"" ).arg( m_message ) ;
	}

	if( !cmd.isEmpty() ){

		e += "\nCommand Sent To Backend: " + cmd ;
	}

	e+= "\n-------------------------" ;

	return e ;
}

int engines::engine::cmdStatus::exitCode() const
{
	return m_exitCode ;
}

void engines::engine::cmdStatus::message( const QString& e )
{
	m_message = e ;

	while( true ){

		if( m_message.endsWith( '\n' ) ){

			m_message.truncate( m_message.size() - 1 ) ;
		}else{
			break ;
		}
	}
}

engines::engine::Options::Options( QStringList s,bool r ) :
	options( std::move( s ) ),reverseMode( r ),success( true )
{
}

engines::engine::Options::Options( QStringList s ) :
	options( std::move( s ) ),reverseMode( false ),success( true )
{
}

engines::engine::Options::Options( bool r ) :
	reverseMode( r ),success( true )
{
}

engines::engine::Options::Options() : success( false )
{
}

engines::engine::options::options( const favorites::entry& e,const QString& volumeKey ) :
	cipherFolder( e.volumePath ),
	plainFolder( e.mountPointPath ),
	key( volumeKey ),
	idleTimeout( e.idleTimeOut ),
	configFilePath( e.configFilePath ),
	type( QString() ),
	ro( e.readOnlyMode ? e.readOnlyMode.onlyRead() : false ),
	reverseMode( e.reverseMode ),
	mountOptions( e.sanitizedMountOptions() )
{
}

engines::engine::options::options( const QString& cipher_folder,
				   const QString& plain_folder,
				   const QString& volume_key,
				   const QString& idle_timeout,
				   const QString& config_file_path,
				   const QString& volume_type,
				   bool unlock_in_read_only,
				   bool unlock_in_reverse_mode,
				   const QString& mount_options,
				   const QString& create_options ) :
	cipherFolder( cipher_folder ),
	plainFolder( plain_folder ),
	key( volume_key ),
	idleTimeout( idle_timeout ),
	configFilePath( config_file_path ),
	type( volume_type ),
	ro( unlock_in_read_only ),
	reverseMode( unlock_in_reverse_mode ),
	mountOptions( favorites::entry::sanitizedMountOptions( mount_options ) ),
	createOptions( create_options )
{
}
