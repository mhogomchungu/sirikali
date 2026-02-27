/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "favorites.h"

#include "utility.h"
#include "settings.h"
#include "crypto.h"

#include <QDir>
#include <QFile>
#include <QCryptographicHash>

utility::qstring_result favorites::configPath()
{
	QString m = settings::instance().ConfigLocation() + "/favorites/" ;

	if( utility::pathExists( m ) ){

		return m ;
	}else{
		if( QDir().mkpath( m ) ){

			return m ;
		}else{
			return {} ;
		}
	}
}

QString favorites::createPath( const QString& m,const favorites::entry& e,favorites::PathFormat pathFormat )
{
	auto a = utility::split( e.volumePath,'@' ).last() ;

	a = utility::split( a,'/' ).last() ;

	a.replace( ":","" ) ;

	if( pathFormat == favorites::PathFormat::New ){

		auto b = e.volumePath + e.mountPointPath ;

		if( e.readOnlyMode.True() ){

			b += "ro" ;
		}else{
			b += "rw" ;
		}

		return m + a + "-" + crypto::sha256( b ) + ".json" ;

	}else if( pathFormat == favorites::PathFormat::Old ){

		auto b = a + e.mountPointPath ;

		if( e.readOnlyMode.True() ){

			b += "ro" ;
		}else{
			b += "rw" ;
		}

		return m + a + "-" + crypto::sha256( b ) + ".json" ;

	}else if( pathFormat == favorites::PathFormat::Older ){

		auto b = e.volumePath + e.mountPointPath ;

		return m + a + "-" + crypto::sha256( b ) + ".json" ;

	}else if( pathFormat == favorites::PathFormat::Oldest ){

		auto b = a + e.mountPointPath ;

		return m + a + "-" + crypto::sha256( b.toLatin1() ) + ".json" ;
	}else{
		auto a = utility::split( e.volumePath,'/' ).last() ;
		auto b = a + e.mountPointPath ;

		return m + a + "-" + crypto::sha256( b.toLatin1() ) + ".json" ;
	}
}

void favorites::updateFavorites( favorites::entry& m )
{
	const auto mOpts = utility::split( m.mountOptions,',' ) ;

	QStringList ss ;

	auto _remove_quotes = []( QString s ){

		if( s.startsWith( '"' ) ){

			s = s.mid( 1 ) ;
		}

		if( s.endsWith( '"' ) ){

			s = utility::removeLast( s,1 ) ;
		}

		return s ;
	} ;

	for( const auto& it : mOpts ){

		if( it.startsWith( "IdentityAgent=" ) ){

			m.identityAgent = _remove_quotes( it.mid( 14 ) ) ;

		}else if( it.startsWith( "IdentityFile=" ) ){

			m.identityFile = _remove_quotes( it.mid( 13 ) ) ;

		}else if( it.contains( "UseNetworkDrive=no",Qt::CaseInsensitive ) ){

			m.reverseMode = false ;

		}else if( it.contains( "UseNetworkDrive=yes",Qt::CaseInsensitive ) ){

			m.reverseMode = true ;

		}else if( !it.isEmpty() ){

			ss.append( it ) ;
		}
	}

	if( !ss.isEmpty() ){

		m.mountOptions = ss.join( ',' ) ;
	}
}

utility2::result< favorites::entry > favorites::fromPath( const QString& path )
{
	utility::logger logger ;

	SirikaliJson json( QFile( path ),logger.function() ) ;

	favorites::entry m ;

	if( json.passed() ){

		m.internalConfigPath   = QDir( path ).absolutePath() ;

		m.reverseMode          = json.getBool( "reverseMode" ) ;
		m.volumeNeedNoPassword = json.getBool( "volumeNeedNoPassword" ) ;
		m.mountPointPath       = json.getString( "mountPointPath" ) ;
		m.configFilePath       = json.getString( "configFilePath" ) ;
		m.idleTimeOut          = json.getString( "idleTimeOut" ) ;
		m.preMountCommand      = json.getString( "preMountCommand" ) ;
		m.postMountCommand     = json.getString( "postMountCommand" ) ;
		m.preUnmountCommand    = json.getString( "preUnmountCommand" ) ;
		m.postUnmountCommand   = json.getString( "postUnmountCommand" ) ;
		m.keyFile              = json.getString( "keyFilePath" ) ;
		m.identityFile         = json.getString( "identityFile" ) ;
		m.identityAgent        = json.getString( "identityAgent" ) ;
		m.mountOptions         = json.getString( "mountOptions" ) ;
		m.passwordSource       = json.getString( "passwordSource" ) ;

		if( m.passwordSource.isEmpty() ){

			m.passwordSource = "Password" ;
		}

		m.password             = json.getByteArray( "password" ) ;

		m.volumePath           = json.getString( "volumePath" ) ;

		favorites::updateFavorites( m ) ;

		favorites::triState::readTriState( json,m.readOnlyMode,"mountReadOnly" ) ;
		favorites::triState::readTriState( json,m.autoMount,"autoMountVolume" ) ;

		return m ;
	}else{
		return {} ;
	}
}

QString favorites::findPath( const favorites::entry& e )
{
	using tty = std::array< favorites::PathFormat,5 > ;

	tty paths{ PathFormat::New,PathFormat::Old,PathFormat::Older,PathFormat::Oldest,PathFormat::Ancient } ;

	auto m = favorites::configPath() ;

	if( m.has_value() ){

		for( const auto& it : paths ){

			auto s = favorites::createPath( m.value(),e,it ) ;

			if( !s.isEmpty() && utility::pathExists( s ) ){

				return s ;
			}
		}
	}

	return {} ;
}

favorites::favorites()
{
	this->reload() ;
}

void favorites::reload()
{
	m_favorites.clear() ;

	const auto m = favorites::configPath() ;

	if( m.has_value() ){

		const auto& a = m.value() ;

		const auto s = QDir( a ).entryList( QDir::Filter::Files | QDir::Filter::Hidden ) ;

		for( const auto& it : s ){

			auto m = favorites::fromPath( a + it ) ;

			if( m.has_value() ){

				m_favorites.emplace_back( std::move( m.RValue() ) ) ;
			}
		}
	}else{
		utility::debug() << "Failed To Get Favorites List" ;
	}
}

favorites::error favorites::add( const favorites::entry& e )
{
	auto m = favorites::configPath() ;

	if( !m.has_value() ){

		utility::debug() << "Failed To Get Favorites Path" ;

		return error::FAILED_TO_CREATE_ENTRY ;
	}

	auto a = favorites::createPath( m.value(),e,PathFormat::New ) ;

	utility::logger logger ;

	SirikaliJson json( logger.function() ) ;

	json[ "volumePath" ]           = e.volumePath ;
	json[ "mountPointPath" ]       = e.mountPointPath ;
	json[ "configFilePath" ]       = e.configFilePath ;
	json[ "keyFilePath" ]          = e.keyFile ;
	json[ "idleTimeOut" ]          = e.idleTimeOut ;
	json[ "mountOptions" ]         = e.mountOptions ;
	json[ "preMountCommand" ]      = e.preMountCommand ;
	json[ "postMountCommand" ]     = e.postMountCommand ;
	json[ "preUnmountCommand" ]    = e.preUnmountCommand ;
	json[ "postUnmountCommand" ]   = e.postUnmountCommand ;
	json[ "reverseMode" ]          = e.reverseMode ;
	json[ "volumeNeedNoPassword" ] = e.volumeNeedNoPassword ;
	json[ "identityAgent" ]        = e.identityAgent ;
	json[ "identityFile" ]         = e.identityFile ;
	json[ "passwordSource" ]       = e.passwordSource ;

	json[ "password" ]             = e.password ;

	favorites::triState::writeTriState( json,e.readOnlyMode,"mountReadOnly" ) ;
	favorites::triState::writeTriState( json,e.autoMount,"autoMountVolume" ) ;

	if( utility::pathExists( a ) ){

		utility::debug() << "Favorite Entry Already Exist" ;
		return error::ENTRY_ALREADY_EXISTS ;
	}else{
		if( json.passed() && json.toFile( a ) ){

			while( true ){

				if( utility::pathExists( a ) ){

					break ;
				}else{
					utility::debug() << "Waiting for a file to show up to the file system" ;
					utility::Task::suspendForOneSecond() ;
				}
			}

			this->reload() ;
			return error::SUCCESS ;
		}else{
			utility::debug() << "Failed To Create Favorite Entry" ;
			return error::FAILED_TO_CREATE_ENTRY ;
		}
	}
}

const std::vector< favorites::entry >& favorites::readFavorites() const
{
	return m_favorites ;
}

utility2::result_ref< const favorites::entry& > favorites::readFavoriteByPath( const QString& configPath ) const
{
	auto path = QDir( configPath ).absolutePath() ;

	for( const auto& it : m_favorites ){

		if( it.internalConfigPath == path ){

			return it ;
		}
	}

	return {} ;
}

utility2::result< favorites::entry > favorites::readFavoriteByFileSystemPath( const QString& path ) const
{
	return this->fromPath( path ) ;
}

utility2::result_ref< const favorites::entry& > favorites::readFavorite( const QString& volumePath,
									 const QString& mountPath ) const
{
	if( mountPath.isEmpty() ){

		for( const auto& it : m_favorites ){

			if( it.volumePath == volumePath ){

				return it ;
			}
		}
	}else{
		for( const auto& it : m_favorites ){

			if( it.volumePath == volumePath && it.mountPointPath == mountPath ){

				return it ;
			}
		}
	}

	return {} ;
}

void favorites::replaceFavorite( const favorites::entry& old,const favorites::entry& New )
{
	this->removeFavoriteEntry( old ) ;
	this->add( New ) ;
}

void favorites::removeFavoriteEntry( const favorites::entry& e )
{
	auto s = favorites::findPath( e ) ;

	if( s.isEmpty() ){

		utility::debug() << "Failed To Find To Remove Favorite Entry: " + e.volumePath ;
	}else{
		if( QFile::remove( s ) ){

			this->reload() ;
		}else{
			utility::debug() << "Failed To Remove Favorite Entry: " + e.volumePath ;
		}
	}
}

favorites::entry::entry()
{
}

favorites::entry::entry( const QString& e,const QString& mountPath ) :
	volumePath( e ),mountPointPath( mountPath )
{
}

bool favorites::entry::match( const QString& volumePath,const QString& mountPointPath ) const
{
	auto m = this->volumePathOpenMode() ;

	if( mountPointPath.isEmpty() ){

		return m == volumePath ;
	}else{
		return m == volumePath && this->mountPointPath == mountPointPath ;
	}
}

QString favorites::entry::volumePathOpenMode() const
{
	if( this->readOnlyMode.True() ){

		return this->volumePath + "(ro)" ;
	}else{
		return this->volumePath + "(rw)" ;
	}
}

void favorites::volEntry::setAutoMount( bool s )
{
	auto m = m_tmpFavorite.get() ;

	if( m ){

		utility::debug() << "Changing a setting of a temporary favorite: " + m->volumePath ;

		m->autoMount = s ;
	}else{
		QString aa = "Creating a copy of a favorite at: %1\nto change a setting" ;

		utility::debug() << aa.arg( m_favorite->volumePath ) ;

		auto a = *m_favorite ;
		a.autoMount = s ;

		m_favorite = this->entry( std::move( a ) ) ;
	}
}
