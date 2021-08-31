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

#include "network_support.h"

#if HAS_NETWORK_SUPPORT

#include <QFile>
#include <QObject>
#include <QWidget>
#include <QTimer>

#include "network_access_manager.hpp"
#include "utility.h"
#include "dialogmsg.h"
#include "siritask.h"
#include "checkforupdateswindow.h"
#include "engines.h"

#include <utility>
#include <vector>

class checkUpdates
{
public:
	static bool hasNetworkSupport()
	{
		return true ;
	}

	void run( bool e ) ;

	checkUpdates( QWidget * widget,checkforupdateswindow::functions ) ;
private:
	void check( bool ) ;

	void showResult() ;

	QString InstalledVersion( const engines::engine& ) ;
	QString latestVersion( const QByteArray& data ) ;

	void checkForUpdate( size_t position = 0 ) ;

	QWidget * m_widget ;

	QNetworkRequest m_networkRequest ;

	NetworkAccessManager m_network ;

	QVector< QStringList > m_results ;

	int m_timeOut ;

	bool m_autocheck ;

	bool m_running ;

	std::vector< engines::engine::Wrapper > m_backends ;
	checkforupdateswindow::functions m_functions ;
} ;

#else

#include "checkforupdateswindow.h"

class checkUpdates
{
public:
	static bool hasNetworkSupport()
	{
		return false ;
	}

	void run( bool e )
	{
		Q_UNUSED( e )
	}

	checkUpdates( QWidget * widget,checkforupdateswindow::functions function )
	{
		Q_UNUSED( widget )
		Q_UNUSED( function )
	}
} ;

#endif

#endif // CHECKFORUPDATES_H
