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

#include <QFile>

#include "utility.h"
#include "dialogmsg.h"
#include "siritask.h"
#include "version.h"

#include "3rdParty/json.hpp"

static QString _tr( const QStringList& l )
{
	auto e = QObject::tr( "%1\"%2\" Installed Version Is : %3.\nLatest Version Is : %4." ) ;
	return e.arg( "",l.at( 0 ),l.at( 1 ),l.at( 2 ) ) ;
}

static void _show( QObject * obj,bool autocheck,QWidget * w,const QVector< QStringList >& l )
{
	DialogMsg msg( w ) ;

	bool show = false ;
	QString e ;

	for( const auto& it : l ){

		e += _tr( it ) + "\n\n" ;

		show = ( it.at( 1 ) != it.at( 2 ) ) ;
	}

	if( autocheck ){

		if( show ){

			msg.ShowUIInfo( QObject::tr( "Version Info" ),e + "\n" ) ;
		}
	}else{
		msg.ShowUIInfo( QObject::tr( "Version Info" ),e + "\n" ) ;
	}

	obj->deleteLater() ;
}

static QString _get_app_version( const QString& e )
{
	if( e == "cryfs" ){

		auto exe = utility::executableFullPath( e ) ;

		if( !exe.isEmpty() ){

			return Task::await< QString >( [ & ]()->QString{

				auto e = utility::Task( exe,-1,[](){

					QProcessEnvironment env ;

					env.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
					env.insert( "CRYFS_FRONTEND","noninteractive" ) ;

					return env ;

				}() ).output().split( ' ' ) ;

				if( e.size() >= 3 ){

					return e.at( 2 ).split( '\n' ).first() ;
				}else{
					return QString() ;
				}
			} ) ;
		}

	}else if( utility::equalsAtleastOne( e,"gocryptfs","securefs","encfs" ) ){

		auto exe = utility::executableFullPath( e ) ;

		if( !exe.isEmpty() ){

			auto args = [ & ]{

				if( utility::equalsAtleastOne( e,"gocryptfs","encfs" ) ){

					return " --version" ;
				}else{
					return " version" ;
				}
			}() ;

			auto a = utility::Task::run( exe + args ).await().splitOutput( ' ',e != "encfs" ) ;

			if( a.size() > 1 ){

				auto r = [ & ]()->QString{

					if( e == "encfs" ){

						return a.at( 2 ) ;
					}else{
						return a.at( 1 ) ;
					}
				}() ;

				r.remove( ';' ) ;
				r.remove( 'v' ) ;
				r.remove( '\n' ) ;

				return r ;
			}
		}

	}else if( e == "SiriKali" ){

		return THIS_VERSION ;
	}

	return "N/A" ;
}

static QStringList _get_versions( NetworkAccessManager& m,const QStringList& e )
{
	const auto& exe  = e.at( 0 ) ;
	const auto& link = e.at( 1 ) ;
	const auto& host = e.at( 2 ) ;

	auto f = _get_app_version( exe ) ;

	if( f == "N/A" ){

		return { exe,"N/A","N/A" } ;
	}

	auto _request = [ & ]( const QStringList& s ){

		QNetworkRequest e( QUrl( s.first() ) ) ;

		e.setRawHeader( "Host",s.at( 1 ).toLatin1() ) ;
		e.setRawHeader( "Accept-Encoding","text/plain" ) ;

		return e ;
	} ;

	return { exe,f,[ & ]()->QString{

		try{
			if( exe == "cryfs" ){

				auto data = m.get( _request( { link,host } ) )->readAll() ;
				auto json = nlohmann::json::parse( data.constData() ) ;

				auto it = json.find( "version_info" ) ;

				if( it != json.end() ){

					auto e = it.value() ;

					auto r = e.find( "current" ) ;

					if( r != e.end() ){

						return QString::fromStdString( r.value() ) ;
					}
				}
			}else{
				auto data = m.get( _request( { link,host } ) )->readAll() ;

				for( const auto& it : nlohmann::json::parse( data.constData() ) ){

					auto e = it.find( "tag_name" ) ;

					if( e != it.end() ){

						auto r = QString::fromStdString( e.value() ).remove( 'v' ) ;

						/*
						 * Try to filter out release candidates,betas and alphas
						 * because
						 * we only care about final releases.
						 */
						if( utility::containsAtleastOne( r,"-","rc","beta","alpha"
										 ,"b","a","m" ) ){

							continue ;
						}

						return r ;
					}
				}
			}

		}catch( ... ){}

		return "N/A" ;
	}() } ;
}

checkForUpdates::checkForUpdates( QWidget * widget,bool autocheck ) :
	m_autocheck( autocheck ),m_widget( widget )
{
	_show( this,m_autocheck,m_widget,[ & ]()->QVector< QStringList >{

		auto _build = []( const QString& e ){

			return "https://api.github.com/repos/" + e + "/releases" ;
		} ;

		auto& m = m_networkAccessManager ;

		auto a = _get_versions( m,{ "SiriKali",
					    _build( "mhogomchungu/sirikali" ),
					    "api.github.com" } ) ;

		auto b = _get_versions( m,{ "cryfs",
					    "https://www.cryfs.org/version_info.json",
					    "www.cryfs.org" } ) ;

		auto c = _get_versions( m,{ "gocryptfs",
					    _build( "rfjakob/gocryptfs" ),
					    "api.github.com" } ) ;

		auto d = _get_versions( m,{ "securefs",
					    _build( "netheril96/securefs" ),
					    "api.github.com" } ) ;

		auto e = _get_versions( m,{ "encfs",
					    _build( "vgough/encfs" ),
					    "api.github.com" } ) ;

		return { a,b,c,d,e } ;
	}() ) ;
}

void checkForUpdates::instance( QWidget * widget,bool e )
{
	if( e ){

		if( utility::autoCheck() ){

			new checkForUpdates( widget,true ) ;
		}
	}else{
		new checkForUpdates( widget,false ) ;
	}
}

bool checkForUpdates::autoCheck()
{
	return utility::autoCheck() ;
}

void checkForUpdates::autoCheck( bool e )
{
	return utility::autoCheck( e ) ;
}

checkForUpdates::~checkForUpdates()
{
}
