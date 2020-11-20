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
#include <QTimer>
#include <QObject>

#include "zulupolkit.h"

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	auto s = QCoreApplication::arguments() ;

	if( s.last().startsWith( "fork" ) ){

		s.removeLast() ;

		auto exe = s.first() ;

		s.removeFirst() ;

		if( QProcess::startDetached( exe,s ) ){

			return 0 ;
		}else{
			return 1 ;
		}
	}else{
		zuluPolkit e( s ) ;

		auto m = new QTimer() ;

		m->setSingleShot( true ) ;

		QObject::connect( m,&QTimer::timeout,[ & ](){

			e.start() ;

			m->deleteLater() ;
		} ) ;

		m->start( 100 ) ;

		return a.exec() ;
	}
}
