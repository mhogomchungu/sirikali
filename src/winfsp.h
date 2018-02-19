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

#ifndef SIRI_WINFSP_H
#define SIRI_WINFSP_H

#include <QtGlobal>

#include <QString>
#include <QStringList>
#include <QList>
#include <vector>
#include <memory>
#include <QProcess>

#include "task.hpp"

namespace SiriKali{
namespace Winfsp{

Task::process::result FspLaunchStop( const QString& className,
				     const QString& instanceName,
				     const QStringList& opts ) ;

Task::process::result FspLaunchStart( const QString& className,
				      const QString& instanceName,
				      const QStringList& opts,
				      const QByteArray& password ) ;

Task::process::result FspLaunchStart( const QString& exe,const QByteArray& password ) ;

Task::process::result FspLaunchStop( const QString& mountPath ) ;

QString readRegister( const char * path,const char * key ) ;

QStringList mountedVolumes() ;

}
}

#if QT_VERSION < QT_VERSION_CHECK( 5,4,0 )

/*
 * Debian 8 uses an old version of Qt that does not have this class.
 * Adding it here to make these old version of Qt happy.
 *
 * This struct is used only in windows version of the project and we use
 * a much recent version of Qt on windows.
 */
struct QStorageInfo{

	static QList<QStorageInfo> mountedVolumes()
	{
		return QList<QStorageInfo>() ;
	}

	bool operator==( const QStorageInfo& e )
	{
		Q_UNUSED( e ) ;
		return false ;
	}
} ;

#else

#include <QStorageInfo>

#endif

#ifdef _WIN32

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

const static short POLLPRI = 0 ;

int poll( struct pollfd * a,int b,int c ) ;

#else

#include <poll.h>

#endif

#endif
