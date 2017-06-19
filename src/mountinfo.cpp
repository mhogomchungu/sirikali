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

#include <QProcess>
#include <QMetaObject>

#include <memory>

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& stop ) :
	m_parent( parent ),
	m_announceEvents( e ),
	m_linux( utility::platformIsLinux() )
{
	m_oldMountList = this->mountedVolumes() ;

	if( m_linux ){

		this->linuxMonitor().then( std::move( stop ) ) ;
	}else{
		this->osxMonitor().then( std::move( stop ) ) ;
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
	return std::move( m_stop ) ;
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

	m_oldMountList = m_newMountList ;
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

Task::future< void >& mountinfo::linuxMonitor()
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

	auto s = std::addressof( e ) ;

	m_stop = [ s ](){ s->first_thread()->terminate() ; } ;

	return e ;
}

Task::future< void >& mountinfo::osxMonitor()
{
	return Task::run( [ this ]{

		QProcess e ;

		m_stop = [ & ](){ e.terminate() ; } ;

#if QT_VERSION > QT_VERSION_CHECK( 5,0,0 )

		QObject::connect( &e,&QProcess::readyReadStandardOutput,[ & ](){

			/*
			 * Clear the buffer,not sure if its necessary.
			 *
			 * In the future,we will examine the output and call this->updateVolume()
			 * only when volumes are mounted/unmounted to get the same behavior
			 * linux code path has.
			 */
			e.readAllStandardOutput() ;

			this->updateVolume() ;
		} ) ;
#endif
		e.start( "diskutil activity" ) ;

		e.waitForFinished( -1 ) ;
	} ) ;
}
