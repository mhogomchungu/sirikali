/*
 *
 *  Copyright ( c ) 2017
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QMetaObject>
#include <QProcess>

#include "zulupolkit.h"

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	auto s = QCoreApplication::arguments() ;

	if( s.last().startsWith( "fork" ) ){

		s.removeLast() ;

		if( QProcess::startDetached( s.join( " " ) ) ){

			return 0 ;
		}else{
			return 1 ;
		}
	}else{
		zuluPolkit e( s ) ;

		QMetaObject::invokeMethod( &e,"start",Qt::QueuedConnection ) ;

		return a.exec();
	}
}
