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

enum class plugin{ hmac_key,externalExecutable } ;

class crypto
{
public:
	crypto( int algo,int flags,const QString& key = QString() )
	{
		this->init() ;

		m_algo = algo ;
		m_error_status = gcry_md_open( &m_handle,algo,flags ) ;

		if( !key.isEmpty() && m_error_status == GPG_ERR_NO_ERROR ){

			this->setKey( key ) ;
		}
	}

	crypto()
	{
		this->init() ;
	}

	QByteArray getRandomData( size_t s )
	{
		QByteArray buffer( s,'\0' ) ;

		gcry_randomize( buffer.data(),buffer.size(),GCRY_STRONG_RANDOM ) ;

		return buffer ;
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

	~crypto()
	{
		gcry_md_close( m_handle ) ;
	}
private:
	void init()
	{
		if( gcry_control( GCRYCTL_INITIALIZATION_FINISHED_P ) == 0 ){

			gcry_check_version( nullptr ) ;
			gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
		}
	}

	int m_algo ;
	int m_error_status ;
	gcry_md_hd_t m_handle = nullptr ;
};

static inline QByteArray hmac_key( const QString& keyFile,const QString& password )
{
	auto _getKey = []( crypto& handle,const QString& keyFile ){

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

	crypto handle( GCRY_MD_SHA256,GCRY_MD_FLAG_HMAC,password ) ;

	if( handle ){

		return _getKey( handle,keyFile ) ;
	}

	return QByteArray() ;
}

static inline QByteArray getRandomData( size_t s )
{
	return crypto().getRandomData( s ) ;
}

} //namespace plugins

#endif // PLUGINS_H
