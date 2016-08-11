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

static QString _tr( const QString& n,const QString& a,const QStringList& l )
{
	auto e = QObject::tr( "%1\"%2\" Installed Version Is : %3.\nLatest Version Is : %4." ) ;
	return e.arg( n,a,l.at( 0 ),l.at( 1 ) ) ;
}

static QString _versions()
{
	auto _get_version = []( const QString& exe,const QString& args ){

		auto e = utility::executableFullPath( exe ) ;

		if( !e.isEmpty() ){

			auto a = utility::Task::run( e + args ).await().splitOutput( ' ' ) ;

			if( a.size() > 1 ){

				auto r = a.at( 1 ) ;

				r.remove( ';' ) ;
				r.remove( 'v' ) ;

				return r ;
			}
		}

		return QString( "\"\"" ) ;
	} ;

	auto a = _tr( "\n\n\n","gocryptfs",{ _get_version( "gocryptfs"," --version" ),"\"\"" } ) ;

	auto b = _tr( "\n\n\n","securefs",{ _get_version( "securefs"," version" ),"\"\"" } ) ;

	return a + b ;
}

static void _show( QObject * obj,bool autocheck,QWidget * w,QString l,const QStringList& e = QStringList() )
{
	DialogMsg msg( w ) ;

	l.remove( '\n' ) ;

	if( l.isEmpty() ){

		if( !autocheck ){

			auto e = QObject::tr( "Failed To Check For Updates." ) ;
			msg.ShowUIOK( QObject::tr( "ERROR" ),e ) ;
		}
	}else{
		if( autocheck ){

			auto _mismatch = [ & ]{

				if( e.size() == 2 ){

					return e.at( 0 ) != e.at( 1 ) ;
				}else{
					return false ;
				}
			} ;

			if( ( l != "Not Found" && l != THIS_VERSION ) || _mismatch() ){

				l = _tr( "\n","SiriKali",{ THIS_VERSION,l } ) ;

				if( !e.isEmpty() ){

					l += _tr( "\n\n\n","cryfs",e ) ;
				}

				l += _versions() ;

				msg.ShowUIInfo( QObject::tr( "Update Available" ),l + "\n" ) ;
			}
		}else{
			if( l != "Not Found" ){

				l = _tr( "\n","SiriKali",{ THIS_VERSION,l } ) ;

				if( !e.isEmpty() ){

					l += _tr( "\n\n\n","cryfs",e ) ;
				}

				l += _versions() ;

				msg.ShowUIInfo( QObject::tr( "Version Info" ),l + "\n" ) ;
			}else{
				auto e = QObject::tr( "Failed To Check For Updates." ) ;

				msg.ShowUIOK( QObject::tr( "ERROR" ),e ) ;
			}
		}
	}

	obj->deleteLater() ;
}

static QNetworkRequest _request( bool e )
{
	QUrl url ;
	QNetworkRequest request ;

	if( e ){
		url.setUrl( "https://raw.githubusercontent.com/mhogomchungu/SiriKali/master/version" ) ;
		request.setRawHeader( "Host","raw.githubusercontent.com" ) ;
	}else{
		url.setUrl( "https://www.cryfs.org/version_info.json" ) ;
		request.setRawHeader( "Host","www.cryfs.org" ) ;
	}

	auto u = "Mozilla/5.0 (X11; Linux x86_64; rv:46.0) Gecko/20100101 Firefox/46.0" ;

	request.setRawHeader( "User-Agent",u ) ;
	request.setRawHeader( "Accept-Encoding","text/plain" ) ;

	request.setUrl( url ) ;

	return request ;
}

void checkForUpdates::show( const QByteArray& cryfs,const QByteArray& cryfs_gui )
{
	auto f = [](){

		auto exe = utility::executableFullPath( "cryfs" ) ;

		if( exe.isEmpty() ){

			return QByteArray() ;
		}else{
			return Task::await< QByteArray >( [ & ](){

				auto e = utility::Task( exe,-1,[](){

					QProcessEnvironment env ;

					env.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
					env.insert( "CRYFS_FRONTEND","noninteractive" ) ;

					return env ;

				}() ).output().split( ' ' ) ;

				if( e.size() >= 3 ){

					return e.at( 2 ).split( '\n' ).first() ;
				}else{
					return QByteArray() ;
				}
			} ) ;
		}
	}() ;

	auto d = [ & ](){

		try{
			auto e = nlohmann::json::parse( cryfs.constData() ) ;

			auto r = e.find( "version_info" ).value().find( "current" ).value() ;

			return QString::fromStdString( r ) ;

		}catch( ... ){

			return QString() ;
		}
	}() ;

	if( f.isEmpty() || d.isEmpty() ){

		_show( this,m_autocheck,m_widget,cryfs_gui ) ;
	}else{
		_show( this,m_autocheck,m_widget,cryfs_gui,{ f,d } ) ;
	}
}

checkForUpdates::checkForUpdates( QWidget * widget,bool autocheck ) :
	m_autocheck( autocheck ),m_widget( widget )
{
	using reply_t = NetworkAccessManager::NetworkReply ;

	m_networkAccessManager.get( _request( true ),[ this ]( reply_t r ){

                auto k = r.release() ;

		m_networkAccessManager.get( _request( false ),[ this,k ]( reply_t e ){

			reply_t r = k ;

			this->show( e->readAll(),r->readAll() ) ;
		} ) ;
	} ) ;
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
