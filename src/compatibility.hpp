/*
 *
 *  Copyright (c) 2020
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

#ifndef SIRIKALI_COMPATIBILITY_H
#define SIRIKALI_COMPATIBILITY_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK( 5,4,0 )

/*
 * Debian 8 uses an old version of Qt that does not have this class.
 * Adding it here to make these old versions of Qt happy.
 *
 * This struct is used only in MACOS version of the project and we use
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

	QString displayName() const
	{
		return QString() ;
	}

	QByteArray device() const
	{
		return QByteArray() ;
	}

	QString name() const
	{
		return QString() ;
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

#ifdef Q_OS_LINUX

#include <sys/inotify.h>

#else

const static int IN_CREATE = 0 ;
const static int IN_DELETE = 0 ;

extern "C"
{

static inline int inotify_init()
{
	return -1 ;
}

struct inotify_event
{
	int wd ;
	size_t mask ;
	const char * name ;
	size_t len ;
} ;

static inline int inotify_add_watch( int a,const char * b,size_t c )
{
	Q_UNUSED( a )
	Q_UNUSED( b )
	Q_UNUSED( c )

	return -1 ;
}

} //extern C

#endif

#ifdef Q_OS_WIN

extern "C"
{

struct pollfd {
	int fd ;
	short events ;
	short revents ;
} ;

const static short POLLPRI = 0 ;

static inline int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a )
	Q_UNUSED( b )
	Q_UNUSED( c )

	return 0 ;
}

typedef struct
{
	int foo ;
}fd_set ;

static inline int select( int a,fd_set * b,fd_set * c,fd_set * d,struct timeval * e )
{
	Q_UNUSED( a )
	Q_UNUSED( b )
	Q_UNUSED( c )
	Q_UNUSED( d )
	Q_UNUSED( e )

	return -1 ;
}

static inline void FD_ZERO( fd_set * e )
{
	Q_UNUSED( e )
}

static inline void FD_SET( int fd,fd_set * e )
{
	Q_UNUSED( fd )

	Q_UNUSED( e )
}

static inline int getuid()
{
	return -1 ;
}

static inline int chown( const char * a,int b,int c )
{
	Q_UNUSED( a )

	Q_UNUSED( b )

	Q_UNUSED( c )

	return -1 ;
}

struct termios
{
	unsigned int c_lflag ;
};

static inline int tcgetattr( int a,struct termios * b )
{
	Q_UNUSED( a )

	Q_UNUSED( b )

	return -1 ;
}

static inline int tcsetattr( int a,int b,struct termios * c )
{
	Q_UNUSED( a )

	Q_UNUSED( b )

	Q_UNUSED( c )

	return -1 ;
}

#define ECHO 0
#define TCSAFLUSH 0

} //extern C

#else

#include <poll.h>
#include <sys/select.h>
#include <termios.h>

#endif

#endif
