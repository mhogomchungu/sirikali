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
#include <QVector>
#include <QObject>
#include <QWidget>

#include "networkAccessManager.hpp"
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

static void _show( bool autocheck,QWidget * w,const QVector< QStringList >& l )
{
	bool show = false ;
	QString e = "\n\n" ;

	for( const auto& it : l ){

		e += _tr( it ) + "\n\n" ;

		const auto& a = it.at( 1 ) ;

		const auto& b = it.at( 2 ) ;

		if( a != "N/A" && b != "N/A" ){

			show = ( a != b ) ;
		}
	}

	if( autocheck ){

		if( show ){

			DialogMsg( w ).ShowUIInfo( QObject::tr( "Version Info" ),true,e + "\n" ) ;
		}
	}else{
		DialogMsg( w ).ShowUIInfo( QObject::tr( "Version Info" ),true,e + "\n" ) ;
	}
}

static QString _version( const siritask::volumeType& e )
{
	if( e == "cryfs" ){

		auto exe = e.executableFullPath() ;

		if( !exe.isEmpty() ){

			return Task::await< QString >( [ & ]()->QString{

				auto e = utility::Task( exe,-1,[](){

					QProcessEnvironment env ;

					env.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
					env.insert( "CRYFS_FRONTEND","noninteractive" ) ;

					return env ;

				}() ).stdOut().split( ' ' ) ;

				if( e.size() >= 3 ){

					return e.at( 2 ).split( '\n' ).first() ;
				}else{
					return "N/A" ;
				}
			} ) ;
		}

	}else if( e.isOneOf( "gocryptfs","securefs","encfs" ) ){

		auto exe = e.executableFullPath() ;

		if( !exe.isEmpty() ){

			auto args = [ & ]{

				if( e.isOneOf( "gocryptfs","encfs" ) ){

					return " --version" ;
				}else{
					return " version" ;
				}
			}() ;

			auto a = utility::Task::run( exe + args ).await().splitOutput( ' ',e != "encfs" ) ;

			if( a.size() > 2 ){

				auto r = [ & ]()->QString{

					if( e == "encfs" ){

						return a.at( 2 ) ;
					}else{
						return a.at( 1 ).split( '\n' ).first() ;
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

static QString _version( const QByteArray& data )
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

	for( const auto& it : nlohmann::json::parse( data.constData() ) ){

		auto e = it.find( "tag_name" ) ;

		if( e != it.end() ){

			auto r = QString::fromStdString( e.value() ).remove( 'v' ) ;

			if( _found_release( r ) ){

				return r ;
			}
		}
	}

	return "N/A" ;
}

static QStringList _version( NetworkAccessManager& m,const QString& exe,const QString& e )
{	
	auto f = _version( exe ) ;

	if( f == "N/A" ){

		return { exe,"N/A","N/A" } ;
	}else {
		QUrl url( "https://api.github.com/repos/" + e + "/releases" ) ;

		QNetworkRequest networkRequest( url ) ;

		networkRequest.setRawHeader( "Host","api.github.com" ) ;
		networkRequest.setRawHeader( "Accept-Encoding","text/plain" ) ;

		try{
			return { exe,f,_version( m.get( networkRequest )->readAll() ) } ;

		}catch( ... ){

			return { exe,f,"N/A" } ;
		}
	}
}

static void _check( QWidget * widget,bool autocheck )
{
	_show( autocheck,widget,[ & ]()->QVector< QStringList >{

		NetworkAccessManager m ;

		auto a = _version( m,"SiriKali","mhogomchungu/sirikali" ) ;

		auto b = _version( m,"cryfs","cryfs/cryfs" ) ;

		auto c = _version( m,"gocryptfs","rfjakob/gocryptfs" ) ;

		auto d = _version( m,"securefs","netheril96/securefs" ) ;

		auto e = _version( m,"encfs","vgough/encfs" ) ;

		return { a,b,c,d,e } ;
	}() ) ;
}

void checkForUpdates::check( QWidget * widget,bool e )
{
	if( e ){

		if( utility::autoCheck() ){

			_check( widget,true ) ;
		}
	}else{
		_check( widget,false ) ;
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
