/*
 *
 *  Copyright (c) 2015
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


#include "checkforupdates.h"

#if HAS_NETWORK_SUPPORT

#include "settings.h"
#include "json_parser.hpp"
#include "engines.h"

checkUpdates::checkUpdates( QWidget * widget,checkforupdateswindow::functions ff ) :
	m_widget( widget ),
	m_timeOut( settings::instance().networkTimeOut() * 1000 ),
	m_network( m_timeOut ),
	m_running( false ),
	m_functions( std::move( ff ) )
{
	m_networkRequest.setRawHeader( "Host","api.github.com" ) ;
	m_networkRequest.setRawHeader( "Accept-Encoding","text/plain" ) ;

	const auto& engines = engines::instance() ;

	m_backends.emplace_back( engines.getUnKnown() ) ;

	for( const auto& it : engines.supportedEngines() ){

		if( !it->releaseURL().isEmpty() ){

			m_backends.emplace_back( it ) ;
		}
	}

	m_backendsInUse = &m_backends ;
}

void checkUpdates::check()
{
	m_running = true ;

	auto& m = checkforupdateswindow::instance( m_widget,m_functions,m_network ) ;

	connect( this,&checkUpdates::update,&m,&checkforupdateswindow::add ) ;
	connect( this,&checkUpdates::done,&m,&checkforupdateswindow::doneUpdating ) ;

	connect( &m,&checkforupdateswindow::closed,[ this ](){

		if( m_backendsInUse == &m_backendsInstallable ){

			emit doneCheckingUpdates() ;
		}

		m_backendsInUse = &m_backends ;
	} ) ;

	m.show() ;

	m_functions.first() ;

	this->checkForUpdate() ;
}

void checkUpdates::checkIfInstalled()
{
	auto basePath = engines::defaultBinPath() ;

	QDir().mkpath( basePath ) ;

	QStringList apps ;

	if( utility::platformIsFlatPak() ){

		auto src = "/app/bin/securefs" ;

		if( QFile::exists( src ) ){

			auto dst = basePath + "securefs" ;

			if( !QFile::exists( dst ) ){

				utility::copyFile( src,dst ) ;
			}
		}
	}

	if( settings::instance().autodownloadMissingEngines() && utility::canDownload() ){

		if( !QFile::exists( basePath + "securefs" ) ){

			apps.append( "Securefs" ) ;
		}

		if( !QFile::exists( basePath + "gocryptfs" ) ){

			apps.append( "Gocryptfs" ) ;
		}
	}

	if( apps.isEmpty() ){

		emit doneCheckingUpdates() ;
	}else{
		for( const auto& it : utility::asConst( m_backends ) ){

			for( const auto& xt : utility::asConst( apps ) ){

				if( it->name() == xt ){

					if( checkforupdateswindow::updatable( it->name() ) ){

						m_backendsInstallable.emplace_back( it ) ;
					}
				}
			}
		}

		if( m_backendsInstallable.size() ){

			m_backendsInUse = &m_backendsInstallable ;

			this->run() ;
		}else{
			emit doneCheckingUpdates() ;
		}
	}
}

void checkUpdates::run()
{
	if( !m_running ){

		this->check() ;
	}
}

QString checkUpdates::InstalledVersion( const engines::engine& e )
{
	if( e.unknown() ){

		return utility::SiriKaliVersion() ;
	}else{
		auto s = e.installedVersion().get() ;

		if( s.valid() ){

			return s.toString() ;
		}else{
			return "N/A" ;
		}
	}
}

checkUpdates::versionData checkUpdates::latestVersion( const QByteArray& data )
{
	auto _found_release = []( const QString& e ){

		for( const auto& it : e ){

			/*
			 * A release version has version in format of "A.B.C"
			 *
			 * ie it only has dots and digits. Presence of any other
			 * character makes the release assumed to be a beta/alpha
			 * or prerelease version(something like "A.B.C-rc1" or
			 * "A.B.C.beta6"
			 */
			if( it != '.' && !( it >= '0' && it <= '9' ) ){

				return false ;
			}
		}

		return true ;
	} ;

	QJsonParseError error ;
	auto doc = QJsonDocument::fromJson( data,&error ) ;

	if( error.error == QJsonParseError::NoError ){

		const auto array = doc.array() ;

		for( const auto& it : array ){

			auto obj = it.toObject() ;

			auto tag_name = obj.value( "tag_name" ).toString() ;

			tag_name.remove( 'v' ) ;
			tag_name.remove( "sshfs-" ) ;

			if( _found_release( tag_name ) ){

				return { tag_name,std::move( obj ) } ;
			}
		}
	}

	return { "N/A",QJsonObject() } ;
}

void checkUpdates::checkForUpdate( size_t position )
{
	if( position == m_backendsInUse->size() ){

		m_running = false ;

		emit done( m_backendsInUse == &m_backendsInstallable ) ;
	}else{
		const auto& s = m_backendsInUse->data()[ position ] ;

		position++ ;

		auto f = this->InstalledVersion( s.get() ) ;

		if( f == "N/A" && m_backendsInUse == &m_backends ){

			checkforupdateswindow::args args ;

			args.engineName       = s->known() ? s->name() : "SiriKali" ; ;
			args.installedVersion = "N/A" ;
			args.onLineVersion    = "N/A" ;

			emit update( args ) ;

			this->checkForUpdate( position ) ;
		}else {
			if( s->known() ){

				m_networkRequest.setUrl( QUrl( s->releaseURL() ) ) ;
			}else{
				m_networkRequest.setUrl( QUrl( "https://api.github.com/repos/mhogomchungu/sirikali/releases" ) ) ;
			}

			m_network.get( m_networkRequest,[ =,&s ]( const utils::network::reply& reply ){

				this->networkReply( position,s.get(),f,reply ) ;
			} ) ;
		}
	}
}

void checkUpdates::networkReply( int position,
				 const engines::engine& s,
				 const QString& installedVersion,
				 const utils::network::reply& reply )
{
	if( reply.success() ){

		checkforupdateswindow::args args ;

		auto m = this->latestVersion( reply.data() ) ;

		args.executableName     = s.executableName() ;
		args.engineName         = s.known() ? s.name() : "SiriKali" ;
		args.installedVersion   = installedVersion ;
		args.executableFullPath = s.executableFullPath() ;
		args.onLineVersion      = m.version ;
		args.url                = s.releaseURL() ;
		args.data               = m.data ;

		emit update( args ) ;

	}else if( reply.timeOut() ){

		checkforupdateswindow::args args ;

		args.engineName = s.known() ? s.name() : "SiriKali" ;

		auto ss    = QString::number( m_timeOut / 1000 ) ;
		args.error = QObject::tr( "Network Request Failed To Respond Within %1 Seconds." ).arg( ss ) ;

		emit update( args ) ;
	}else{
		checkforupdateswindow::args args ;

		args.engineName = s.known() ? s.name() : "SiriKali" ; ;
		args.error      = reply.errorString() ;

		emit update( args ) ;
	}

	this->checkForUpdate( position ) ;
}

#endif
