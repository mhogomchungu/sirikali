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

#ifndef CHECKFORUPDATES_H
#define CHECKFORUPDATES_H

#include <QFile>
#include <QVector>
#include <QObject>
#include <QWidget>
#include <QTimer>

#include "3rdParty/NetworkAccessManager/networkAccessManager.hpp"
#include "utility.h"
#include "dialogmsg.h"
#include "siritask.h"
#include "version.h"
#include "json.h"

#include <utility>

class checkUpdates : public QObject
{
	Q_OBJECT
public:
	static void run( QWidget * widget,bool e )
	{
		if( e ){

			if( utility::autoCheck() ){

				new checkUpdates( widget,e ) ;
			}
		}else{
			new checkUpdates( widget,e ) ;
		}
	}

	static bool autoCheck( void )
	{
		return utility::autoCheck() ;
	}

	static void autoCheck( bool e )
	{
		utility::autoCheck( e ) ;
	}

	checkUpdates( QWidget * widget,bool autocheck ) ;

private slots:
	void timeOut() ;
private:
	void showResult() ;

	QString InstalledVersion( const siritask::volumeType& e ) ;
	QString latestVersion( const QByteArray& data ) ;

	void checkForUpdate( QVector< std::pair< QString,QString > >::size_type position ) ;

	QWidget * m_widget ;

	QNetworkReply * m_networkReply ;
	QNetworkRequest m_networkRequest ;

	NetworkAccessManager m_network ;

	QVector< QStringList > m_results ;

	QTimer m_timer ;

	bool m_autocheck ;

	QVector< std::pair< QString,QString > > m_backends = { {

		{ "sirikali","mhogomchungu/sirikali" },
		{ "cryfs","cryfs/cryfs" },
		{ "gocryptfs","rfjakob/gocryptfs" },
		{ "securefs","netheril96/securefs" },
		{ "securefs","netheril96/securefs" },
		{ "ecryptfs-simple","mhogomchungu/ecryptfs-simple" }
	} } ;
} ;

#endif // CHECKFORUPDATES_H
