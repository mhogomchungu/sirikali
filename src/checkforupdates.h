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

#include "network_access_manager.hpp"
#include "utility.h"
#include "dialogmsg.h"
#include "siritask.h"
#include "version.h"
#include "json.h"

#include <utility>
#include <array>

class checkUpdates : public QObject
{
	Q_OBJECT
public:
	void run( bool e ) ;

	checkUpdates( QWidget * widget ) ;
private:
	void check( bool ) ;

	void showResult() ;

	QString InstalledVersion( const QString& ) ;
	QString latestVersion( const QByteArray& data ) ;

	using backends_t = std::array< std::pair< const char *,const char * >,6 > ;

	void checkForUpdate( backends_t::size_type position = 0 ) ;

	QWidget * m_widget ;

	QNetworkRequest m_networkRequest ;

	NetworkAccessManager m_network ;

	QVector< QStringList > m_results ;

	int m_timeOut ;

	bool m_autocheck ;

	bool m_running ;

	backends_t m_backends = { {

		{ "sirikali","https://api.github.com/repos/mhogomchungu/sirikali/releases" },
		{ "cryfs","https://api.github.com/repos/cryfs/cryfs/releases" },
		{ "gocryptfs","https://api.github.com/repos/rfjakob/gocryptfs/releases" },
		{ "securefs","https://api.github.com/repos/netheril96/securefs/releases" },
		{ "encfs","https://api.github.com/repos/vgough/encfs/releases" },
		{ "ecryptfs-simple","https://api.github.com/repos/mhogomchungu/ecryptfs-simple/releases" }
	} } ;
} ;

#endif // CHECKFORUPDATES_H
