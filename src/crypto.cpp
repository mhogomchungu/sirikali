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

#include "crypto.h"

#include <QFile>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

class cryptoImpl
{
public:
	cryptoImpl( int algo,unsigned int flags,const QString& key = QString() )
	{
		this->init_grypt() ;

		m_algo = algo ;
		m_error_status = gcry_md_open( &m_handle,algo,flags ) ;

		if( m_error_status == GPG_ERR_NO_ERROR ){

			this->setKey( key ) ;
		}
	}

	cryptoImpl()
	{
		this->init_grypt() ;
	}

	QByteArray getRandomData( int s )
	{
		QByteArray buffer ;
		buffer.resize( s ) ;

		gcry_randomize( buffer.data(),
				static_cast< unsigned long >( buffer.size() ),
				GCRY_STRONG_RANDOM ) ;

		return buffer ;
	}

	void write( const void * buffer,size_t size )
	{
		gcry_md_write( m_handle,buffer,size ) ;
	}

	void write( const QByteArray& buffer )
	{
		gcry_md_write( m_handle,buffer.constData(),static_cast< size_t >( buffer.size() ) ) ;
	}

	bool setKey( const QString& e )
	{
		m_error_status = gcry_md_setkey( m_handle,e.toLatin1().constData(),static_cast< size_t >( e.size() ) ) ;

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

	~cryptoImpl()
	{
		gcry_md_close( m_handle ) ;
	}
private:
	void init_grypt()
	{
		if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){

			gcry_check_version( nullptr ) ;
			gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
		}
	}

	int m_algo ;
	unsigned int m_error_status ;
	gcry_md_hd_t m_handle = nullptr ;
};

QByteArray crypto::getRandomData( int s )
{
	return cryptoImpl().getRandomData( s ) ;
}

QByteArray crypto::hmac_key(const QString& keyFile,const QString& password )
{
	auto _getKey = []( cryptoImpl& handle,const QString& keyFile ){

		QFile f( keyFile ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			const int size = 1024 ;
			char buffer[ size ] ;

			while( true ){

				auto e = f.read( buffer,size ) ;

				if( e <= 0 ){
					break ;
				}else{
					handle.write( buffer,static_cast< size_t >( e ) ) ;
				}
			}

			return handle.result().toHex() ;
		}else{
			return QByteArray() ;
		}
	} ;

	cryptoImpl handle( GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC,password ) ;

	if( handle ){

		return _getKey( handle,keyFile ) ;
	}

	return QByteArray() ;
}
