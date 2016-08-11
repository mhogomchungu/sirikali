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
#ifndef PLUGINS_H
#define PLUGINS_H

#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QObject>
#include <QFile>
#include <QVector>
#include <QDebug>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

namespace plugins
{

enum class plugin{ hmac_key } ;

static inline QByteArray hmac_key( const QString& keyFile,const QString& password )
{
	auto _getKey = []( gcry_md_hd_t handle,const QString& keyFile ){

		QFile f( keyFile ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			const int size = 1024 ;
			char buffer[ size ] ;

			while( true ){

				auto e = f.read( buffer,size ) ;

				if( e <= 0 ){
					break ;
				}else{
					gcry_md_write( handle,buffer,e ) ;
				}
			}

			auto key = reinterpret_cast< const char * >( gcry_md_read( handle,0 ) ) ;
			auto len = static_cast< int >( gcry_md_get_algo_dlen( GCRY_MD_SHA256 ) ) ;

			QByteArray r( key,len ) ;

			return r.toHex() ;
		}else{
			return QByteArray() ;
		}
	} ;

	if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){

		gcry_check_version( nullptr ) ;
		gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	}

	QByteArray key ;
	gcry_md_hd_t handle ;

	auto r = gcry_md_open( &handle,GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC ) ;

	if( r == GPG_ERR_NO_ERROR ){

		auto e = password.toLatin1() ;

		r = gcry_md_setkey( handle,e.constData(),e.size() ) ;

		if( r == GPG_ERR_NO_ERROR ){

			key = _getKey( handle,keyFile ) ;
		}

		gcry_md_close( handle ) ;
	}

	return key ;
}

} //namespace plugins

#endif // PLUGINS_H
