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

enum class background_thread{ True,False } ;
static QStringList _mounted_volumes( background_thread thread )
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

		auto list = [ & ](){

			if( thread == background_thread::True ){

				return utility::Task::run( "mount" ).get().splitOutput( '\n' ) ;
			}else{
				return utility::Task::run( "mount" ).await().splitOutput( '\n' ) ;
			}
		}() ;

		for( const auto& it : list ){

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

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& quit ) :
	m_parent( parent ),
	m_quit( std::move( quit ) ),
	m_announceEvents( e ),
	m_linux( utility::platformIsLinux() ),
	m_oldMountList( _mounted_volumes( background_thread::False ) )
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

Task::future< QVector< volumeInfo > >& mountinfo::unlockedVolumes()
{
	return Task::run< QVector< volumeInfo > >( [](){

		auto _hash = []( const QString& e ){

			/*
			 * jenkins one at a time hash function.
			 *
			 * https://en.wikipedia.org/wiki/Jenkins_hash_function
			 */

			uint32_t hash = 0 ;

			auto p = e.toLatin1() ;

			auto key = p.constData() ;

			auto l = p.size() ;

			for( decltype( l ) i = 0 ; i < l ; i++ ){

				hash += *( key + i ) ;

				hash += ( hash << 10 ) ;

				hash ^= ( hash >> 6 ) ;
			}

			hash += ( hash << 3 ) ;

			hash ^= ( hash >> 11 ) ;

			hash += ( hash << 15 ) ;

			return QString::number( hash ) ;
		} ;

		auto _decode = []( QString path,bool set_offset ){

			path.replace( "\\012","\n" ) ;
			path.replace( "\\040"," " ) ;
			path.replace( "\\134","\\" ) ;
			path.replace( "\\011","\\t" ) ;

			if( set_offset ){

				return path.mid( path.indexOf( '@' ) + 1 ) ;
			}else{
				return path ;
			}
		} ;

		auto _fs = []( QString e ){

			e.replace( "fuse.","" ) ;

			return e ;
		} ;

		auto _ro = []( const QStringList& l ){

			return l.at( 5 ).mid( 0,2 ) ;
		} ;

		QVector< volumeInfo > e ;

		volumeInfo::mountinfo info ;

		for( const auto& it : _mounted_volumes( background_thread::True ) ){

			if( volumeInfo::supported( it ) ){

				const auto& k = utility::split( it,' ' ) ;

				const auto s = k.size() ;

				if( s < 6 ){

					continue ;
				}

				const auto& cf = k.at( s - 2 ) ;

				const auto& m = k.at( 4 ) ;

				const auto& fs = k.at( s - 3 ) ;

				if( utility::startsWithAtLeastOne( cf,"encfs@",
								   "cryfs@",
								   "securefs@",
								   "gocryptfs@" ) ){

					info.volumePath = _decode( cf,true ) ;

				}else if( utility::equalsAtleastOne( fs,"fuse.gocryptfs",
								     "ecryptfs" ) ){

					info.volumePath = _decode( cf,false ) ;
				}else{
					info.volumePath = _hash( m ) ;
				}

				info.mountPoint   = _decode( m,false ) ;
				info.fileSystem   = _fs( fs ) ;
				info.mode         = _ro( k ) ;
				info.mountOptions = k.last() ;

				e.append( info ) ;
			}
		}

		return e ;
	} ) ;
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
	m_newMountList = _mounted_volumes( background_thread::False ) ;

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
