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

class gcrypt_md_handle
{
public:
	gcrypt_md_handle( int algo,int flags )
	{
		if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){

			gcry_check_version( nullptr ) ;
			gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
		}

		m_algo = algo ;
		m_error_status = gcry_md_open( &m_handle,algo,flags ) ;
	}

	void write( const void * buffer,size_t size )
	{
		gcry_md_write( m_handle,buffer,size ) ;
	}

	void write( const QByteArray& buffer )
	{
		gcry_md_write( m_handle,buffer.constData(),buffer.size() ) ;
	}

	bool setKey( const QString& e )
	{
		m_error_status = gcry_md_setkey( m_handle,e.toLatin1().constData(),e.size() ) ;

		return m_error_status == GPG_ERR_NO_ERROR ;
	}

	operator bool()
	{
		return m_error_status == GPG_ERR_NO_ERROR ;
	}

	QByteArray result()
	{
		auto key = reinterpret_cast< const char * >( gcry_md_read( m_handle,0 ) ) ;
		auto len = static_cast< int >( gcry_md_get_algo_dlen( m_algo ) ) ;

		return { key,len } ;
	}

	~gcrypt_md_handle()
	{
		gcry_md_close( m_handle ) ;
	}
private:
	int m_algo ;
	int m_error_status ;
	gcry_md_hd_t m_handle ;
};

static inline QByteArray hmac_key( const QString& keyFile,const QString& password )
{
	auto _getKey = []( gcrypt_md_handle& handle,const QString& keyFile ){

		QFile f( keyFile ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			const int size = 1024 ;
			char buffer[ size ] ;

			while( true ){

				auto e = f.read( buffer,size ) ;

				if( e <= 0 ){
					break ;
				}else{
					handle.write( buffer,e ) ;
				}
			}

			return handle.result().toHex() ;
		}else{
			return QByteArray() ;
		}
	} ;

	gcrypt_md_handle handle( GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC ) ;

	if( handle ){

		if( handle.setKey( password ) ){

			return _getKey( handle,keyFile ) ;
		}
	}

	return QByteArray() ;
}

static inline QByteArray sha512( const QByteArray& e )
{
	gcrypt_md_handle handle( GCRY_MD_SHA512,0 ) ;

	if( handle ){

		handle.write( e ) ;

		return handle.result().toHex() ;
	}else{
		return QByteArray() ;
	}
}

} //namespace plugins

#endif // PLUGINS_H
