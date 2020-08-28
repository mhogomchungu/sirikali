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
#include "compatibility.hpp"

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

class eventsMonitor{
public:
	class pollMonitor{
	public:
		pollMonitor( std::atomic_bool& s,
			     int fd,
			     int timeOut = 1000,
			     short eventType = POLLPRI ) :
			m_exit( s ),
			m_timeOut( timeOut )
		{
			m_pollfd.fd     = fd ;
			m_pollfd.events = eventType ;
		}
		template< typename Function >
		void poll( Function function )
		{
			while( true ){

				auto a = ::poll( &m_pollfd,1,m_timeOut ) ;

				if( m_exit ){

					break ;
				}else{
					function( a ) ;
				}
			}
		}
	private:
		std::atomic_bool& m_exit ;
		int m_timeOut ;
		struct pollfd m_pollfd ;
	} ;

	class selectMonitor{

	public:
		selectMonitor( std::atomic_bool& s,int fd,int sec = 1,int usec = 0 ) :
			m_exit( s ),
			m_fd( fd )
		{
			m_tv.tv_sec  = sec ;
			m_tv.tv_usec = usec ;

			FD_ZERO( &m_rfds ) ;
		}

		template< typename Function >
		void event( Function&& function )
		{
			struct timeval tv ;

			while( true ){

				FD_SET( m_fd,&m_rfds ) ;

				tv.tv_sec  = m_tv.tv_sec ;
				tv.tv_usec = m_tv.tv_usec ;

				int r = ::select( m_fd + 1,&m_rfds,nullptr,nullptr,&tv ) ;

				if( m_exit ){

					break ;
				}else{
					function( r ) ;
				}
			}
		}
	private:
		std::atomic_bool& m_exit ;
		int m_fd ;
		fd_set m_rfds ;
		struct timeval m_tv ;
	} ;

	class folderMonitor{
	public:
		class entry{
		public:
			entry( int fd,const QString& path ) :
				m_path( path + "/" ),
				m_fd( fd )
			{
			}
			~entry()
			{
				close( m_fd ) ;
			}
			const QString& path() const
			{
				return m_path ;
			}
			int fd() const
			{
				return m_fd ;
			}
		private:
			QString m_path ;
			int m_fd ;
		} ;

		folderMonitor( std::atomic_bool& s ) :
			m_inotify_fd( inotify_init() ),
			m_selectMonitor( s,m_inotify_fd )
		{
		}
		~folderMonitor()
		{
			close( m_inotify_fd ) ;
		}
		bool functional()
		{
			return m_inotify_fd != -1 ;
		}
		bool working()
		{
			return m_inotify_fd != -1 && m_fds.size() > 0 ;
		}
		void createList( const QStringList& l )
		{
			this->pevent( l,IN_CREATE ) ;
		}
		void deleteList( const QStringList& l )
		{
			this->pevent( l,IN_DELETE ) ;
		}
		void monitorList( const QStringList& l,uint32_t ev )
		{
			this->pevent( l,ev ) ;
		}
		class Entry{
		public:
			Entry( const struct inotify_event * event,const std::vector< entry >& fds ) :
				m_event( event ),m_fds( fds )
			{
			}
			template< typename Function >
			void created( Function&& function ) const
			{
				this->pevent( function,IN_CREATE ) ;
			}
			template< typename Function >
			void deleted( Function&& function ) const
			{
				this->pevent( function,IN_DELETE ) ;
			}
		private:
			template< typename Function >
			void pevent( Function&& function,unsigned int s ) const
			{
				if( m_event->mask & s ){

					for( auto& it : m_fds ){

						if( it.fd() == m_event->wd ){

							function( it.path() + m_event->name ) ;

							break ;
						}
					}
				}
			}
			const struct inotify_event * m_event ;
			const std::vector< entry >& m_fds ;
		} ;

		template< typename Function >
		void event( Function&& function )
		{
			m_selectMonitor.event( [ this,&function ]( int r ){

				std::vector< char > buffer( 10485760 ) ;

				if( r > 0 ){

					auto s = read( m_inotify_fd,buffer.data(),buffer.size() ) ;

					if( s > 0 ){

						auto a = buffer.data() ;

						auto b = buffer.data() + s ;

						this->event( a,b,r,function ) ;
					}
				}else{
					function( r,eventsMonitor::folderMonitor::Entry( nullptr,m_fds ) ) ;
				}
			} ) ;
		}
	private:
		template< typename Function >
		void event( const char * it,const char * end,int r,Function& function )
		{
			while( it < end ){

				auto a = reinterpret_cast< const struct inotify_event * >( it ) ;

				function( r,eventsMonitor::folderMonitor::Entry( a,m_fds ) ) ;

				it += sizeof( struct inotify_event ) + a->len ;
			}
		}
		void pevent( const QStringList& l,uint32_t ev )
		{
			for( const auto& it : l ){

				int e = inotify_add_watch( m_inotify_fd,it.toUtf8(),ev ) ;

				if( e != -1 ){

					if( it.endsWith( "/" ) ){

						auto s = it ;

						while( s.endsWith( "/" ) ){

							s = utility::removeLast( s,1 ) ;
						}

						m_fds.emplace_back( e,s ) ;
					}else{
						m_fds.emplace_back( e,it ) ;
					}
				}
			}
		}
		std::vector< entry > m_fds ;
		int m_inotify_fd ;
		selectMonitor m_selectMonitor ;
	} ;
} ;

class folderMonitor{
public:
	using function = std::function< void( const QString& ) > ;
	folderMonitor( bool wait,const QString& path = QString() ) ;
	const QString& path() const ;
	void contentCountIncreased( folderMonitor::function& function ) ;
	void contentCountDecreased( folderMonitor::function& function ) ;
private:
	QStringList folderList() const ;
	void folderListSynced( std::function< void( QStringList ) > ) const ;
	QString m_path ;
	QStringList m_folderList ;
	bool m_waitForSynced ;
} ;

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
	void qtMonitor( void ) ;
	void updateVolume( void ) ;
	void pbUpdate( void ) ;
	void autoMount( const QString& ) ;
	void pollForUpdates( void ) ;

	QObject * m_parent ;

	std::function< void() > m_stop = nullptr ;
	std::function< void() > m_quit ;

	bool m_announceEvents ;

	std::atomic_bool m_exit ;

	std::atomic_bool m_linuxMonitorExit ;

	volumeInfo::List m_oldMountList ;
	volumeInfo::List m_newMountList ;

	dbusMonitor m_dbusMonitor ;

	class folderMountEvents{

	public:
		folderMountEvents( std::function< void( const QString& ) > ) ;
		~folderMountEvents() ;
		void start() ;
		void stop() ;
		bool monitor() ;
	private:
		std::function< void( const QString& ) > m_update ;
		std::atomic_bool m_folderMonitorExit ;
		eventsMonitor::folderMonitor m_folderMonitor ;

	} m_folderMountEvents ;
};

#endif // MONITOR_MOUNTINFO_H
