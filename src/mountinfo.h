/*
 *
 *  Copyright (c) 2012-2015
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


#ifndef MONITOR_MOUNTINFO_H
#define MONITOR_MOUNTINFO_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QProcess>
#include <QVector>

#include <functional>
#include <memory>
#include <vector>

#include "volumeinfo.h"

class mountinfo : private QObject
{
	Q_OBJECT
public:
	static QString mountProperties( const QString& mountPoint,
					const QString& mode,
					const QString& fileSystem,
					const QString& cipherPath ) ;

	static QString encodeMountPath( const QString& ) ;

	static Task::future< std::vector< volumeInfo > >& unlockedVolumes() ;

	mountinfo( QObject * parent,bool,std::function< void() >&& ) ;

	void stop() ;

	void announceEvents( bool ) ;

	~mountinfo() ;
private slots:
	void volumeUpdate( void ) ;
private:
	void windowsMonitor( void ) ;
	void linuxMonitor( void ) ;
	void osxMonitor( void ) ;
	void updateVolume( void ) ;
	void pbUpdate( void ) ;
	void autoMount( const QString& ) ;
	void pollForUpdates( void ) ;

	QObject * m_parent ;

	std::function< void() > m_stop = nullptr ;
	std::function< void() > m_quit ;

	bool m_announceEvents ;
	std::atomic_bool m_exit ;

	QStringList m_oldMountList ;
	QStringList m_newMountList ;
};

#endif // MONITOR_MOUNTINFO_H
