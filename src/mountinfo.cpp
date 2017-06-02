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

#include "mountinfo.h"
#include "utility.h"
#include "siritask.h"

#include <QMetaObject>

#include <memory>

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& f ) :
	m_parent( parent ),
	m_stop( std::move( f ) ),
	m_announceEvents( e ),
	m_linux( utility::platformIsLinux() )
{
	if( m_linux ){

		m_oldMountList = this->mountedVolumes() ;

		auto& e = Task::run( [ this ](){ this->run() ; } ) ;

		e.then( [ this ](){ m_stop() ; } ) ;

		m_task = std::addressof( e ) ;
	}
}

mountinfo::mountinfo() : m_linux( utility::platformIsLinux() )
{
}

mountinfo::~mountinfo()
{
}

QStringList mountinfo::mountedVolumes()
{
	if( m_linux ){

		QFile f( "/proc/self/mountinfo" ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			return utility::split( f.readAll() ) ;
		}else{
			return QStringList() ;
		}
	}else{
		QStringList s ;
		QString mode ;
		QString fs ;
		const QString w = "x x x:x x %1 %2,x - %3 %4 x" ;

		auto z = utility::Task::run( "mount" ).await().stdOut() ;

		for( const auto& it : utility::split( z ) ){

			auto e = utility::split( it,' ' ) ;

			if( e.contains( ", read-only," ) ){

				mode = "ro" ;
			}else{
				mode = "rw" ;
			}

			fs = "fuse." + it.mid( 0,it.indexOf( '@' ) ) ;

			s.append( w.arg( e.at( 2 ),mode,fs,e.at( 0 ) ) ) ;
		}

		return s ;
	}
}

std::function< void() > mountinfo::stop()
{
	if( m_linux ){

		return [ this ](){

			auto e = m_task->first_thread() ;

			if( e->isRunning() ){

				e->terminate() ;
			}else{
				m_stop() ;
			}
		} ;
	}else{
		return [ this ](){ m_stop() ; } ;
	}
}

void mountinfo::updateVolume()
{
	m_newMountList = this->mountedVolumes() ;

	auto _volumeWasMounted = [ & ](){

		return m_oldMountList.size() < m_newMountList.size() ;
	} ;

	auto _mountedVolume = [ & ]( const QString& e ){

		return !m_oldMountList.contains( e ) ;
	} ;

	if( m_announceEvents ){

		QMetaObject::invokeMethod( m_parent,"pbUpdate",Qt::QueuedConnection ) ;

		if( _volumeWasMounted() ){

			for( const auto& it : m_newMountList ){

				if( !_mountedVolume( it ) ){

					continue ;
				}

				const auto e = utility::split( it,' ' ) ;

				if( e.size() < 4 ){

					continue ;
				}

				QMetaObject::invokeMethod( m_parent,
							   "autoMountFavoritesOnAvailable",
							   Qt::QueuedConnection,
							   Q_ARG( QString,e.at( 4 ) ) ) ;
			}
		}
	}

	m_oldMountList = m_newMountList ;
}

void mountinfo::announceEvents( bool s )
{
	m_announceEvents = s ;
}

void mountinfo::eventHappened()
{
	if( !m_linux && m_announceEvents ){

		/*
		 * Suspend for a bit to give mount command time to
		 * properly populate its mounted list before calling it.
		 *
		 * Sometimes,mount events do not get registered and i suspect its
		 * because we call mount too soon.
		 */

		utility::Task::suspendForOneSecond() ;

		QMetaObject::invokeMethod( m_parent,
					   "pbUpdate",
					   Qt::QueuedConnection ) ;
	}
}

void mountinfo::anza()
{
	if( m_linux ){

		m_task->start() ;
	}
}

void mountinfo::run()
{
	class mountEvent
	{
	public:
		mountEvent() : m_handle( "/proc/self/mountinfo" )
		{
			m_handle.open( QIODevice::ReadOnly ) ;
			m_monitor.fd     = m_handle.handle() ;
			m_monitor.events = POLLPRI ;
		}
		operator bool()
		{
			poll( &m_monitor,1,-1 ) ;
			return true ;
		}
	private:
		QFile m_handle ;
		struct pollfd m_monitor ;
	} event ;

	while( event ){

		this->updateVolume() ;
	}
}
