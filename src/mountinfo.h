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

class folderMonitor{
public:
	using function = std::function< void( const QString& ) > ;
	folderMonitor( bool wait,const QString& path = QString() ) ;
	const QString& path() const ;
	void contentCountIncreased( folderMonitor::function& function ) ;
	void contentCountDecreased( folderMonitor::function& function ) ;
private:
	QStringList folderList() const ;
	utility::result< QStringList > folderListSynced() const ;
	QString m_path ;
	QStringList m_folderList ;
	bool m_waitForSynced ;
} ;

#ifdef Q_OS_LINUX

#include <QtDBus>

class siriDBus{
public:
	siriDBus( QObject * obj ) : m_qObject( obj )
	{
	}
	void monitor()
	{
		m_dbus.reset( new QDBusConnection( QDBusConnection::sessionBus() ) ) ;

		auto a = "org.gtk.vfs.Daemon" ;
		auto b = "/org/gtk/vfs/mounttracker" ;
		auto c = "org.gtk.vfs.MountTracker" ;

		m_dbus->connect( a,b,c,"Mounted",m_qObject,SLOT( volumeAdded() ) ) ;

		m_dbus->connect( a,b,c,"Unmounted",m_qObject,SLOT( volumeRemoved() ) ) ;
	}
private:
	std::unique_ptr< QDBusConnection > m_dbus ;
	QObject * m_qObject ;
};

#else

class siriDBus{
public:
	siriDBus( QObject * obj )
	{
		Q_UNUSED( obj )
	}
	void monitor()
	{
	}
};

#endif

class dbusMonitor : public QObject
{
	Q_OBJECT
public:
	dbusMonitor( folderMonitor::function function ) ;
private slots:
	void volumeAdded() ;
	void volumeRemoved() ;
private:
	siriDBus m_dbus ;
	folderMonitor m_folderMonitor ;
	folderMonitor::function m_function ;
} ;

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

	dbusMonitor m_dbusMonitor ;

	class folderMountEvents{

	public:
		folderMountEvents( std::function< void( const QString& ) > ) ;
		void start() ;
		void stop() ;
		bool monitor() ;
	private:
		class entry{
		public:
			entry( int fd,const QString& path ) :
				m_folderMonitor( false,path ),m_fd( fd )
			{
			}
			const QString& path() const
			{
				return m_folderMonitor.path() ;
			}
			int fd() const
			{
				return m_fd ;
			}
			void contentCountChanged( folderMonitor::function& function )
			{
				m_folderMonitor.contentCountIncreased( function ) ;
			}
		private:
			folderMonitor m_folderMonitor ;
			int m_fd ;
		} ;
		std::vector< mountinfo::folderMountEvents::entry > m_fds ;
		int m_inotify_fd ;
		std::function< void( const QString& ) > m_update ;

	} m_folderMountEvents ;
};

#endif // MONITOR_MOUNTINFO_H
