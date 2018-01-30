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

namespace SiriKali{
namespace Winfsp{

struct winFsp{

	QString className ;
	QString instanceName ;
	QString command ;
} ;

bool FspLaunchStop( const QString& className,const QString& instanceName ) ;

bool FspLaunchStart( const QString& className,const QString& instanceName,
		     const QStringList opts,bool hasSecret ) ;

class ActiveInstances
{
public:
	ActiveInstances() ;
	~ActiveInstances() ;
	bool valid() const ;
	const std::vector< SiriKali::Winfsp::winFsp >& values() const ;
	QStringList commands() const ;
private:
	class impl ;
	std::unique_ptr< impl > m_handle ;
} ;

QString readRegister( const char * path,const char * key ) ;

}
}

#if QT_VERSION < QT_VERSION_CHECK( 5,4,0 )

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
