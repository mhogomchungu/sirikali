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
#include "engines.h"

#ifdef Q_OS_LINUX

#include <QtDBus>

class siriDBus{
public:
	siriDBus( QObject * obj ) : m_qObject( obj )
	{
	}
	void monitor()
	{
		m_dbus = std::make_unique< QDBusConnection >( QDBusConnection::sessionBus() ) ;

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
		pollMonitor( int fd,int timeOut = 1000,short eventType = POLLPRI )
		{
			m_timeOut       = timeOut ;
			m_pollfd.fd     = fd ;
			m_pollfd.events = eventType ;
		}
		template< typename Function >
		void poll( Function&& function )
		{
			while( true ){

				auto a = ::poll( &m_pollfd,1,m_timeOut ) ;

				if( function( a ) ){

					break ;
				}
			}
		}
	private:
		int m_timeOut ;
		struct pollfd m_pollfd ;
	} ;

	class selectMonitor{
	public:
		selectMonitor( int fd,int sec = 1,int usec = 0 ) : m_fd( fd )
		{
			m_tv.tv_sec  = sec ;
			m_tv.tv_usec = usec ;

			FD_ZERO( &m_rfds ) ;
		}
		template< typename Function >
		void poll( Function&& function ) const
		{
			struct timeval tv ;

			while( true ){

				FD_SET( m_fd,&m_rfds ) ;

				tv.tv_sec  = m_tv.tv_sec ;
				tv.tv_usec = m_tv.tv_usec ;

				auto r = select( m_fd + 1,&m_rfds,nullptr,nullptr,&tv ) ;

				if( function( r ) ){

					break ;
				}
			}
		}
	private:
		int m_fd ;
		mutable fd_set m_rfds ;
		struct timeval m_tv ;
	} ;

	class folderMonitor{
	public:
		folderMonitor( int sec = 1,int usec = 0 ) :
			m_inotify_fd( inotify_init() ),
			m_selectMonitor( m_inotify_fd,sec,usec )
		{
		}
		~folderMonitor()
		{
			close( m_inotify_fd ) ;
		}
		bool functional() const
		{
			return m_inotify_fd != -1 ;
		}
		bool hasFoldersToMonitor() const
		{
			return m_fds.size() > 0 ;
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
		class entries{
		public:
			class _entry{
			public:
				_entry( int fd,const QString& path ) :
					 m_path( path + "/" ),
					 m_fd( fd )
				{
				}
				~_entry()
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
			entries( const struct inotify_event * event,
				 const std::vector< entries::_entry >& fds ) :
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
			void pevent( Function& function,unsigned int s ) const
			{
				if( m_event && m_event->mask & s ){

					for( auto& it : m_fds ){

						if( it.fd() == m_event->wd ){

							function( it.path() + m_event->name ) ;

							break ;
						}
					}
				}
			}
			const struct inotify_event * m_event ;
			const std::vector< entries::_entry >& m_fds ;
		} ;

		template< typename Function >
		void poll( Function&& function ) const
		{
			m_selectMonitor.poll( [ this,&function ]( int r ){

				if( r > 0 ){

					std::vector< char > buffer( 10485760 ) ;

					auto s = read( m_inotify_fd,buffer.data(),buffer.size() ) ;

					auto a = buffer.data() ;

					auto b = buffer.data() + s ;

					return this->poll( a,b,r,function ) ;
				}else{
					return function( r,entries( nullptr,m_fds ) ) ;
				}
			} ) ;
		}
	private:
		template< typename Function >
		bool poll( const char * it,const char * end,int r,Function& function ) const
		{
			bool m = false ;

			while( it < end ){

				auto a = reinterpret_cast< const struct inotify_event * >( it ) ;

				m = function( r,entries( a,m_fds ) ) ;

				if( m ){

					break ;
				}else{
					it += sizeof( struct inotify_event ) + a->len ;
				}
			}

			return m ;
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
		std::vector< entries::_entry > m_fds ;
		int m_inotify_fd ;
		selectMonitor m_selectMonitor ;
	} ;
} ;

class folderMonitor{
public:
	using function = std::function< void( const QString& ) > ;
	folderMonitor( bool wait,
		       std::function< void( const QString& ) >&,
		       const QString& path = QString() ) ;
	const QString& path() const ;
	void contentCountIncreased( folderMonitor::function& function ) ;
	void contentCountDecreased( folderMonitor::function& function ) ;
private:
	QStringList folderList() const ;
	void folderListSynced( std::function< void( QStringList ) > ) const ;
	QString m_path ;
	QStringList m_folderList ;
	bool m_waitForSynced ;
	std::function< void( const QString& ) >& m_debug ;
} ;

class dbusMonitor : public QObject
{
	Q_OBJECT
public:
	dbusMonitor( folderMonitor::function function,
		     std::function< void( const QString& ) >& ) ;
private slots:
	void volumeAdded() ;
	void volumeRemoved() ;
private:
	siriDBus m_dbus ;
	folderMonitor m_folderMonitor ;
	folderMonitor::function m_function ;
	std::function< void( const QString& ) >& m_debug ;
} ;

class mountinfo : private QObject
{
	Q_OBJECT
public:
	struct volumeEntry{

		const engines::engine& engine ;
		volumeInfo vInfo ;
	} ;

	using List = std::vector< volumeEntry > ;

	static volumeInfo::List toVolumeInfoList( mountinfo::List m )
	{
		volumeInfo::List s ;

		for( auto&& it : m ){

			s.emplace_back( std::move( it.vInfo ) ) ;
		}

		return s ;
	}

	static Task::future< mountinfo::List >& unlockedVolumes() ;

	mountinfo( QObject * parent,
		   bool,
		   std::function< void() >,
		   std::function< void( const QString& ) > ) ;

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
	std::function< void( const QString& ) > m_debug ;

	bool m_announceEvents ;

	std::atomic_bool m_exit ;
	std::atomic_bool m_linuxMonitorExit ;
	std::atomic_bool m_folderMonitorExit ;

	volumeInfo::List m_oldMountList ;
	volumeInfo::List m_newMountList ;

	dbusMonitor m_dbusMonitor ;
};

#endif // MONITOR_MOUNTINFO_H
