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
#include "task.hpp"

#include <QFile>
#include <QMessageAuthenticationCode>

#include <random>

class rng{
public:
	rng()
	{
		m_rng.seed( m_randomDevice() ) ;
	}
	QByteArray get( int s )
	{
		std::uniform_int_distribution<> getRandom( 0,255 ) ;

		QByteArray m ;

		m.resize( s ) ;

		auto e = m.data() ;

		for( int i = 0 ; i < s ; i++ ){

			*( e + i ) = static_cast< char >( getRandom( m_rng ) ) ;
		}

		return m ;
	}
private:
	std::mt19937 m_rng ;
	std::random_device m_randomDevice ;
} ;

QByteArray crypto::getRandomData( int s )
{
	static thread_local rng m ;
	return m.get( s ) ;
}

QByteArray crypto::hmac_key( const QString& keyFile,const QString& password )
{
	return Task::await( [ & ](){

		QMessageAuthenticationCode hmac( QCryptographicHash::Sha256,password.toUtf8() ) ;

		QFile file( keyFile ) ;

		if( file.open( QIODevice::ReadOnly ) ){

			if( hmac.addData( &file ) ){

				return hmac.result().toHex() ;
			}
		}

		return QByteArray() ;
	} ) ;
}

QByteArray crypto::hmac_key( const QString& password )
{
	QMessageAuthenticationCode hmac( QCryptographicHash::Sha256,password.toUtf8() ) ;
	return hmac.result().toHex() ;
}

QByteArray crypto::sha256( const QString& e )
{
	return QCryptographicHash::hash( e.toUtf8(),QCryptographicHash::Sha256 ).toHex() ;
}

QByteArray crypto::sha256( const QByteArray& e )
{
	return QCryptographicHash::hash( e,QCryptographicHash::Sha256 ).toHex() ;
}
