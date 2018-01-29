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
#include "task.hpp"

#include <QMetaObject>
#include <QtGlobal>

#include <QFile>

#include <vector>
#include <utility>

enum class background_thread{ True,False } ;

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)

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

#ifdef WIN32

#include <winfsp/launch.h>

struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

const static short POLLPRI = 0 ;

static int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a ) ;
	Q_UNUSED( b ) ;
	Q_UNUSED( c ) ;

	return 0 ;
}

#if 0

static QStringList _getwinfspInstances( background_thread thread )
{
	auto exe = "\"" + utility::winFSPpath() + "\\bin\\launchctl-x86.exe\"" ;

	auto cmd = [ & ]( const QString& e ){

		auto s = [ & ](){

			if( thread == background_thread::True ){

				return utility::Task::run( exe + " " + e ).get() ;
			}else{
				return utility::Task::run( exe + " " + e ).await() ;
			}
		}() ;

		return s.splitOutput( '\n',utility::Task::channel::stdOut ) ;
	} ;

	auto s = cmd( "list" ) ;

	if( s.size() > 0 && s.first().startsWith( "OK" ) ){

		s.removeFirst() ;
	}

	QStringList m ;

	for( const auto& it : s ){

		auto e = utility::split( it,' ' ) ;

		if( e.size() > 1 ){

			auto s = cmd( "info " + e.at( 0 ) + " " + e.at( 1 ) ) ;

			if( s.size() > 2 ){

				m.append( s.at( 2 ) ) ;
			}
		}
	}

	return m ;
}

#endif

namespace SiriKali {

struct winFsp{

	QString className ;
	QString instanceName ;
	QString command ;
} ;

}

class infoList
{
public:
	infoList()
	{
		for( const auto& it : this->nameList() ){

			this->list( it ) ;
		}
	}
	bool valid()
	{
		return m_error == 0 ;
	}
	const std::vector< SiriKali::winFsp >& values()
	{
		return m_entries ;
	}
	QStringList commands()
	{
		QStringList s ;

		for( const auto& it : m_entries ){

			s.append( it.command ) ;
		}

		return s ;
	}
private:
	void list( const SiriKali::winFsp& e )
	{
		std::array< WCHAR,BUFFER_SIZE > buffer ;

		ULONG size = static_cast< ULONG >( buffer.size() ) ;

		std::array< WCHAR,BUFFER_SIZE > className ;
		std::array< WCHAR,BUFFER_SIZE > instanceName ;

		e.className.toWCharArray( className.data() ) ;
		e.instanceName.toWCharArray( instanceName.data() ) ;

		FspLaunchGetInfo( className.data(),
				  instanceName.data(),
				  buffer.data(),
				  &size,
				  &m_error) ;

		size = size / sizeof( WCHAR ) ;

		if( m_error == 0 ){

			this->build( m_entries,buffer.data(),size,3 ) ;
		}
	}
	std::vector< SiriKali::winFsp > nameList()
	{
		std::array< WCHAR,BUFFER_SIZE > buffer ;

		ULONG size = static_cast< ULONG >( buffer.size() ) ;

		std::vector< SiriKali::winFsp > entries ;

		FspLaunchGetNameList( buffer.data(),&size,&m_error) ;

		size = size / sizeof( WCHAR ) ;

		if( m_error == 0 ){

			this->build( entries,buffer.data(),size,2 ) ;
		}

		return entries ;
	}
	std::pair< WCHAR *,int > component( WCHAR * buffer,ULONG size,ULONG e )
	{
		ULONG i = e ;

		while( true ){

			if( e >= size ){

				return { nullptr,0 } ;

			}else if( buffer[ e ] == 0 ){

				return { &buffer[ i ],e + 1 } ;
			}else{
				e++ ;
			}
		}
	}
	void build( std::vector< SiriKali::winFsp >& entries,
		     WCHAR * buffer,ULONG size,int count )
	{
		ULONG i = 0 ;

		while( true ){

			auto s = this->component( buffer,size,i ) ;

			if( s.first == nullptr ){

				return ;
			}

			auto e = this->component( buffer,size,s.second ) ;

			if( e.first == nullptr ){

				return ;
			}

			if( count == 2 ){

				auto a = QString::fromWCharArray( s.first ) ;
				auto b = QString::fromWCharArray( e.first ) ;

				entries.emplace_back( SiriKali::winFsp{ a,b } ) ;

				i = e.second ;
			}else{
				auto m = this->component( buffer,size,e.second ) ;

				if( m.first == nullptr ){

					return ;
				}

				auto a = QString::fromWCharArray( s.first ) ;
				auto b = QString::fromWCharArray( e.first ) ;
				auto c = QString::fromWCharArray( m.first ) ;

				entries.emplace_back( SiriKali::winFsp{ a,b,c } ) ;

				i = m.second ;
			}
		}
	}
	static const size_t BUFFER_SIZE = 4096 ;
	ULONG m_error ;
	std::vector< SiriKali::winFsp > m_entries ;
} ;

static QStringList _getwinfspInstances( background_thread thread )
{
	if( thread == background_thread::True ){

		return Task::await( [](){ return infoList().commands() ; } ) ;
	}else{
		return infoList().commands() ;
	}
}

#else
#include <poll.h>

static QStringList _getwinfspInstances( background_thread thread )
{
	Q_UNUSED( thread ) ;
	return QStringList() ;
}

#endif

static QStringList _unlocked_volumes( background_thread thread )
{
	if( utility::platformIsLinux() ){

		return utility::split( utility::fileContents( "/proc/self/mountinfo" ) ) ;

	}else if( utility::platformIsOSX() ){

		QStringList s ;
		QString mode ;
		QString fs ;
		const QString w = "x x x:x x %1 %2,x - %3 %4 x" ;

		auto e = [ & ](){

			if( thread == background_thread::True ){

				return utility::Task::run( "mount" ).get().splitOutput( '\n' ) ;
			}else{
				return utility::Task::run( "mount" ).await().splitOutput( '\n' ) ;
			}
		}() ;

		for( const auto& it : e ){

			auto e = utility::split( it,' ' ) ;

			if( e.size() > 2 ){

				if( e.contains( ", read-only," ) ){

					mode = "ro" ;
				}else{
					mode = "rw" ;
				}

				fs = "fuse." + it.mid( 0,it.indexOf( '@' ) ) ;

				s.append( w.arg( e.at( 2 ),mode,fs,e.at( 0 ) ) ) ;
			}
		}

		return s ;
	}else{
		QStringList s ;
		QString mode ;
		QString fs ;
		const QString w = "x x x:x x %1 %2,x - %3 %4 x" ;

		auto path = []( QString e ){

			if( e.startsWith( '\"' ) ){

				e.remove( 0,1 ) ;
			}

			if( e.endsWith( '\"' ) ){

				e.truncate( e.size() - 1 ) ;
			}

			return e ;
		} ;

		for( const auto& it : _getwinfspInstances( thread ) ){

			auto e = utility::split( it,' ' ) ;

			if( e.size() > 6 ){

				if( e.contains( " -o ro " ) ){

					mode = "ro" ;
				}else{
					mode = "rw" ;
				}

				auto m = e.at( 5 ).mid( 11 ) ;

				fs = "fuse." + m ;

				s.append( w.arg( path( e.last() ),mode,fs,m + "@" + path( e.at( 6 ) ) ) ) ;
			}
		}

		return s ;
	}
}

mountinfo::mountinfo( QObject * parent,bool e,std::function< void() >&& quit ) :
	m_parent( parent ),
	m_quit( std::move( quit ) ),
	m_announceEvents( e ),
	m_oldMountList( _unlocked_volumes( background_thread::False ) )
{
	if( utility::platformIsLinux() ){

		this->linuxMonitor() ;

	}else if( utility::platformIsOSX() ){

		this->osxMonitor() ;
	}else{
		this->windowsMonitor();
	}
}

mountinfo::~mountinfo()
{
}

Task::future< std::vector< volumeInfo > >& mountinfo::unlockedVolumes()
{
	return Task::run( [](){

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

		std::vector< volumeInfo > e ;

		volumeInfo::mountinfo info ;

		for( const auto& it : _unlocked_volumes( background_thread::True ) ){

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

				e.emplace_back( info ) ;
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
	m_newMountList = _unlocked_volumes( background_thread::False ) ;

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

	m_process.setProcessChannelMode( QProcess::MergedChannels ) ;

	m_process.start( "diskutil activity" ) ;
}

void mountinfo::windowsMonitor()
{
	m_exit = false ;

	m_stop = [ this ](){ m_exit = true ; } ;

	auto interval = utility::winFSPpollingInterval() ;

	Task::run( [ &,interval ](){

		auto previous = QStorageInfo::mountedVolumes() ;
		auto now = previous ;

		while( true ){

			if( m_exit ){

				break ;
			}else{
				utility::Task::wait( interval ) ;

				now = QStorageInfo::mountedVolumes() ;

				if( now != previous ){

					this->updateVolume() ;
				}

				previous = std::move( now ) ;
			}
		}

	} ).then( std::move( m_quit ) ) ;
}
