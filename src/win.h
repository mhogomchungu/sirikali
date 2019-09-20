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

#include "task.hpp"
#include "siritask.h"
#include "engines.h"

namespace SiriKali{
namespace Windows{

struct opts{

	const engines::engine::args& args ;
	const engines::engine::options& options ;
	const engines::engine& engine ;
	const QString& password ;
} ;

Task::process::result mount( const SiriKali::Windows::opts& ) ;

Task::process::result create( const SiriKali::Windows::opts& ) ;

Task::process::result unmount( const QString& unMountCommand,const QString& mountPath ) ;

QString volumeProperties( const QString& mountPath ) ;

int terminateProcess( unsigned long pid ) ;

QString engineInstalledDir( const QString& ) ;
QString engineInstalledDir( const QString& key,const QString& value ) ;

QStringList engineInstalledDirs() ;

bool mountPointTaken( const QString& ) ;

void updateVolumeList( std::function< void() > ) ;

bool backEndTimedOut( const QString& ) ;

struct mountOptions
{
	mountOptions( const QString& a,const QString& b,const QString& c,
		      const QString& d,const QString& e ) :
		mode( a ),subtype( b ),cipherFolder( c ),mountPointPath( d ),fuseOptions( e )
	{
	}
	QString mode ;
	QString subtype ;
	QString cipherFolder ;
	QString mountPointPath ;
	QString fuseOptions ;
};

std::vector< mountOptions > getMountOptions() ;

}
}

#if QT_VERSION < QT_VERSION_CHECK( 5,4,0 )

/*
 * Debian 8 uses an old version of Qt that does not have this class.
 * Adding it here to make these old versions of Qt happy.
 *
 * This struct is used only in windows and MACOS version of the project and we use
 * much recent versions of Qt on these platforms.
 */
struct QStorageInfo
{
	static QList<QStorageInfo> mountedVolumes()
	{
		return QList<QStorageInfo>() ;
	}

	bool operator==( const QStorageInfo& e )
	{
		Q_UNUSED( e ) ;
		return false ;
	}

	QString rootPath() const
	{
		return QString() ;
	}

	QByteArray device() const
	{
		return QByteArray() ;
	}

	QByteArray fileSystemType() const
	{
		return QByteArray() ;
	}

	bool isReadOnly() const
	{
		return false ;
	}
} ;

#else

#include <QStorageInfo>

#endif

#ifdef Q_OS_WIN

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

const static short POLLPRI = 0 ;

static inline int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a )
	Q_UNUSED( b )
	Q_UNUSED( c )

	return 0 ;
}

#else

#include <poll.h>

#endif

#endif
