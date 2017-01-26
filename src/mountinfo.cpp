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

#include <QString>
#include <QStringList>

#include "utility.h"
#include "task.h"
#include "siritask.h"

#include <QCoreApplication>

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& f ) :
	QThread( parent ),m_stop( std::move( f ) ),m_announceEvents( e ),
	m_linux( utility::platformIsLinux() ),m_oldMountList( this->mountedVolumes() )
{
	m_babu = parent ;
	m_baba = this ;
	m_main = this ;
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

		auto z = utility::Task::run( "mount" ).await().output() ;

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

		return [ this ](){ m_mtoto->terminate() ; } ;
	}else{
		return [ this ](){ m_hang = false ; } ;
	}
}

void mountinfo::threadStopped()
{
	m_stop() ;
}

void mountinfo::updateVolume()
{
	auto _volumeWasMounted = [ & ](){

		return m_oldMountList.size() < m_newMountList.size() ;
	} ;

	auto _mountedVolume = [ & ]( const QString& e ){

		return !m_oldMountList.contains( e ) ;
	} ;

	m_newMountList = this->mountedVolumes() ;

	if( _volumeWasMounted() ){

		for( const auto& it : m_newMountList ){

			if( _mountedVolume( it ) ){

				const auto e = utility::split( it,' ' ) ;

				if( e.size() > 3 ){

					gotEvent( e.at( 4 ) ) ;
				}
			}
		}
	}

	m_oldMountList = m_newMountList ;

	if( m_announceEvents ){

		emit gotEvent() ;
	}
}

void mountinfo::runLinux()
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

void mountinfo::runOSX()
{
	m_hang = true ;

	/*
	 * TODO: Find a better way to hang this thread until shutdown time
	 */

	while( true ){

		if( m_hang ){

			this->sleep( 1 ) ;
		}else{
			break ;
		}
	}
}

void mountinfo::announceEvents( bool s )
{
	m_announceEvents = s ;
}

void mountinfo::eventHappened()
{
	if( !m_linux ){

		this->updateVolume() ;
	}
}

void mountinfo::run()
{
	m_mtoto = this ;

	connect( m_mtoto,SIGNAL( finished() ),m_main,SLOT( threadStopped() ) ) ;
	connect( m_mtoto,SIGNAL( finished() ),m_mtoto,SLOT( deleteLater() ) ) ;

	if( m_linux ){

		this->runLinux() ;
	}else{
		this->runOSX() ;
	}
}
