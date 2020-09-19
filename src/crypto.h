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

#ifndef CRYPTO_H
#define CRYPTO_H

#include <QByteArray>
#include <QString>

class crypto
{
public:
	static QByteArray getRandomData( int s ) ;
	static QByteArray hmac_key( const QString& keyFile,const QString& password ) ;
	static QByteArray hmac_key( const QString& password ) ;
	static QByteArray sha256( const QString& ) ;
	static QByteArray sha256( const QByteArray& ) ;
};

#endif
