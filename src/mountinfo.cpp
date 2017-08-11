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
#include "task.h"

#include <QMetaObject>

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& quit ) :
	m_parent( parent ),
	m_quit( std::move( quit ) ),
	m_announceEvents( e ),
	m_linux( utility::platformIsLinux() ),
	m_oldMountList( this->mountedVolumes() )
{
	if( m_linux ){

		this->linuxMonitor() ;
	}else{
		this->osxMonitor() ;
	}
}

mountinfo::~mountinfo()
{
}

QStringList mountinfo::mountedVolumes()
{
	if( utility::platformIsLinux() ){

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

		for( const auto& it : utility::Task::run( "mount" ).get().splitOutput( '\n' ) ){

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

void mountinfo::stop()
{
	if( m_stop ){

		m_stop() ;
	}else{
		m_quit() ;
	}
}

void mountinfo::volumeUpdate()
{
	m_newMountList = this->mountedVolumes() ;

	auto _volumeWasMounted = [ & ](){

		return m_oldMountList.size() < m_newMountList.size() ;
	} ;

	auto _mountedVolume = [ & ]( const QString& e ){

		return !m_oldMountList.contains( e ) ;
	} ;

	if( m_announceEvents ){

		this->pbUpdate() ;

		if( _volumeWasMounted() ){

			for( const auto& it : m_newMountList ){

				const auto e = utility::split( it,' ' ) ;

				if( _mountedVolume( it ) && e.size() > 3 ){

					this->autoMount( e.at( 4 ) ) ;
				}
			}
		}
	}

	m_oldMountList = std::move( m_newMountList ) ;
}

void mountinfo::updateVolume()
{
	QMetaObject::invokeMethod( this,"volumeUpdate",Qt::QueuedConnection ) ;
}

void mountinfo::pbUpdate()
{
	QMetaObject::invokeMethod( m_parent,"pbUpdate",Qt::QueuedConnection ) ;
}

void mountinfo::autoMount( const QString& e )
{
	QMetaObject::invokeMethod( m_parent,
				   "autoMountFavoritesOnAvailable",
				   Qt::QueuedConnection,
				   Q_ARG( QString,e ) ) ;
}

void mountinfo::announceEvents( bool s )
{
	m_announceEvents = s ;
}

void mountinfo::linuxMonitor()
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
	} ;

	auto& e = Task::run( [ & ](){

		mountEvent event ;

		while( event ){

			this->updateVolume() ;
		}
	} ) ;

	m_stop = [ s = std::addressof( e ) ](){ s->first_thread()->terminate() ; } ;

	e.then( std::move( m_quit ) ) ;
}

#if QT_VERSION > QT_VERSION_CHECK( 5,0,0 )

void mountinfo::osxMonitor()
{
	m_stop = [ this ](){ m_process.terminate() ; } ;

	auto s = static_cast< void( QProcess::* )( int ) >( &QProcess::finished ) ;

	connect( &m_process,s,[ this ]( int e ){ Q_UNUSED( e ) ; m_quit() ; } ) ;

	connect( &m_process,&QProcess::readyReadStandardOutput,[ & ](){

		/*
		 * Clear the buffer,not sure if its necessary.
		 *
		 * In the future,we will examine the output and call this->updateVolume()
		 * only when we notice a volumes was mounted/unmounted to reduce noise if
		 * "diskutil activity" is too chatty.
		 */
		m_process.readAllStandardOutput() ;

		this->updateVolume() ;
	} ) ;

	m_process.start( "diskutil activity" ) ;
}

#else

void mountinfo::osxMonitor()
{
}

#endif
