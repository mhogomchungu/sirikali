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
#include "win.h"
#include "settings.h"
#include "crypto.h"
#include "processManager.h"

#include <QMetaObject>
#include <QtGlobal>
#include <QFileInfo>
#include <QFile>

#include <vector>
#include <utility>

static QString _getFileSystem( const QString& dev,const QString& fs )
{
	/*
	 * We expect "dev" to have a value that looks like
	 *
	 * securefs@/path/to/cipher/folder
	 *
	 * We get the backend end name now and cipher path will be taken
	 * later on in this source file.
	 */
	auto ss = dev.indexOf( "@" ) ;

	if( ss != -1 ){
		/*
		 * "dev" is in the expected format and all is well
		 */
		return "fuse." + dev.mid( 0,ss ) ;
	}else{
		/*
		 * we will get here if "dev" has only something like "securefs" and
		 * this means we just lost the path to cipher path and this is probably
		 * a backend bug because it didn't set "fsname" fuse option we gave it.
		 */
		for( const auto& it : engines::instance().supportedEngines() ){

			if( it->name() == dev ){
				/*
				 * "dev" has a name of a backend we support, return it.
				 */
				return "fuse." + dev ;
			}
		}
	}
	/*
	 * We have no idea what is in "dev", return the value reported by Qt.
	 */
	return fs ;
}

static volumeInfo::List _qt_volumes()
{
	volumeInfo::List s ;

	for( const auto& it : QStorageInfo::mountedVolumes() ){

		auto dev = engines::engine::encodeMountPath( it.device() ) ;

		auto mp = engines::engine::encodeMountPath( it.rootPath() ) ;

		auto fs = _getFileSystem( dev,it.fileSystemType() ) ;

		auto mode = it.isReadOnly() ? "ro" : "rw" ;

		s.emplace_back( std::move( dev ),std::move( mp ),std::move( fs ),mode ) ;
	}

	return s ;
}

static mountinfo::List _processManager_volumes()
{
	mountinfo::List s ;

	auto& m = processManager::get() ;

	m.removeInActive() ;

	for( const auto& e : m.mountOptions() ){

		auto fs = "fuse." + e.subtype ;

		auto mm = e.subtype + "@" + e.cipherFolder ;

		volumeInfo aa{ std::move( mm ),e.mountPointPath,std::move( fs ),e.mode } ;

		s.emplace_back( mountinfo::volumeEntry{ e.engine,std::move( aa ) } ) ;
	}

	return s ;
}

static volumeInfo::List _linux_volumes()
{
	volumeInfo::List s ;

	for( const auto& e : utility::split( utility::fileContents( "/proc/self/mountinfo" ) ) ){

		const auto k = utility::split( e,' ' ) ;

		const auto m = k.size() ;

		if( m > 5 ){

			const auto& q = k.at( 5 ) ;

			s.emplace_back( k.at( m - 2 ),k.at( 4 ),k.at( m - 3 ),q.mid( 0,2 ),q ) ;
		}
	}

	return s ;
}

static volumeInfo::List _unlocked_volumes()
{
	auto a = [](){

		if( utility::platformIsLinux() ){

			return _linux_volumes() ;

		}else if( utility::platformIsWindows() ){

			return mountinfo::toVolumeInfoList( _processManager_volumes() ) ;
		}else{
			return _qt_volumes() ;
		}
	}() ;

	for( auto&& it : engines::instance().mountInfo( a ) ){

		a.emplace_back( std::move( it ) ) ;
	}

	return a ;
}

mountinfo::mountinfo( QObject * parent,
		      bool e,
		      std::function< void() > quit,
		      std::function< void( const QString& ) > debug ) :
	m_parent( parent ),
	m_quit( std::move( quit ) ),
	m_debug( std::move( debug ) ),
	m_announceEvents( e ),
	m_linuxMonitorExit( false ),
	m_folderMonitorExit( false ),
	m_oldMountList( _unlocked_volumes() ),
	m_dbusMonitor( [ this ]( const QString& e ){ this->autoMount( e ) ; },m_debug )
{
	processManager::get().updateVolumeList( [ this ](){

		this->updateVolume() ;
	} ) ;

	if( utility::platformIsLinux() ){

		this->linuxMonitor() ;

	}else if( utility::platformIsWindows() ){

		this->windowsMonitor() ;
	}else{
		this->qtMonitor() ;
	}
}

mountinfo::~mountinfo()
{
}

static mountinfo::volumeEntry _decode( const engines::engine& engine,volumeInfo&& e )
{
	auto _decode = []( QString& path,bool set_offset ){

		engines::engine::decodeSpecialCharacters( path ) ;

		if( set_offset ){

			path = path.mid( path.indexOf( '@' ) + 1 ) ;
		}
	} ;

	auto _starts_with = []( const engines::engine& e,const QString& s ){

		for( const auto& it : e.names() ){

			if( s.startsWith( it + "@",Qt::CaseInsensitive ) ){

				return true ;
			}
		}

		return false ;
	} ;

	auto& cipherPath = e.cipherPath() ;
	auto& mountPoint = e.mountPoint() ;
	auto& fileSystem = e.fileSystem() ;

	if( engine.setsCipherPath() ){

		if( _starts_with( engine,cipherPath ) ){

			_decode( cipherPath,true ) ;
		}else{
			if( cipherPath.compare( engine.name(),Qt::CaseInsensitive ) ){

				_decode( cipherPath,false ) ;
			}else{
				cipherPath = crypto::sha256( e.mountPoint() ).mid( 0,20 ) ;
			}
		}
	}else{
		cipherPath = crypto::sha256( mountPoint ).mid( 0,20 ) ;
	}

	_decode( mountPoint,false ) ;

	const auto& n = engine.displayName() ;

	if( n.isEmpty() ){

		fileSystem.replace( "fuse.","" ) ;

		if( !fileSystem.isEmpty() ){

			fileSystem.replace( 0,1,fileSystem.at( 0 ).toUpper() ) ;
		}
	}else{
		fileSystem = n ;

		fileSystem.replace( 0,1,fileSystem.at( 0 ).toUpper() ) ;
	}

	return { engine,std::move( e ) } ;
}

static bool _has_access_to_mount_point( const volumeInfo& e,const engines::engine& engine )
{
	/*
	 * We do not know who unlocked the volume so we assume this user did it by checking
	 * if they have access to the mount point path
	 */
	if( QFileInfo( e.mountPoint() ).isReadable() ){

		auto m = "Adding a known engine \"%1\" at \"%2\"\nbecause its mount point is accessible" ;
		utility::debug() << QString( m ).arg( engine.name(),e.cipherPath() ) ;
		return true ;
	}else{
		auto m = "Skipping a known engine \"%1\" at \"%2\"\nbecause its mount point is inaccessible" ;
		utility::debug() << QString( m ).arg( engine.name(),e.cipherPath() ) ;
		return false ;
	}
}

static bool _owned_by_user( const engines::engine& engine,
			    const QString& userNameSection,
			    volumeInfo& vInfo )
{
	auto& m = vInfo.cipherPath() ;

	/*
	 * A properly formatted cipherPath will be in the format of
	 * @USER@ENGINE@CIPHER_PATH
	 */

	auto usrNameSection = engines::engine::encodeMountPath( userNameSection ) ;

	auto engineSection = engine.name() + "@" ;

	if( m.startsWith( usrNameSection + engineSection,Qt::CaseInsensitive ) ){

		/*
		 * CIPHER_PATH is properly formatted and the volume was unlocked by this user
		 */

		/*
		 * remove the username section part
		 */
		m = m.mid( usrNameSection.size() ) ;

		return true ;
	}else{
		auto a = m.indexOf( '@' ) ;

		if( a == -1 ){
			/*
			 * Malformed entry, could not find a single entry for '@'.
			 *
			 * We will get here is "m" contains only cipherPath component.
			 */
			return _has_access_to_mount_point( vInfo,engine ) ;
		}else{
			auto usrName = m.mid( 0,a ) ;

			auto engSection = m.mid( a + 1 ) ;

			if( engSection.startsWith( engineSection,Qt::CaseInsensitive ) ){

				/*
				 * CIPHER_PATH is properly formatted and the volume was unlocked by another user
				 */
				/*
				 * We will get here is "m" contains something like "$OTHERUSER@gocryptfs@/woof/cipherPath"
				 * and we are sure this entry was created by another user.
				 */

				auto msg = "Skipping a known engine at \"%1\"\nbecause it was opened by user \"%2\"" ;

				utility::debug() << QString( msg ).arg( m,usrName ) ;

				return false ;
			}else{
				/*
				 * We will get here is "m" contains something like "gocryptfs@/woof/cipherPath"
				 */
				return _has_access_to_mount_point( vInfo,engine ) ;
			}
		}
	}
}

Task::future< mountinfo::List >& mountinfo::unlockedVolumes()
{
	return Task::run( [](){

		mountinfo::List e ;

		const auto& engines = engines::instance() ;

		auto userNameSection = utility::userName() + "@" ;

		for( auto&& it : _unlocked_volumes() ){

			const auto& engine = engines.getByFsName( it.fileSystem() ) ;

			if( engine.known() ){

				if( utility::platformIsWindows() ){

					e.emplace_back( _decode( engine,std::move( it ) ) ) ;

				}else if( engine.runsInBackGround() ){

					if( _owned_by_user( engine,userNameSection,it ) ){

						e.emplace_back( _decode( engine,std::move( it ) ) ) ;
					}
				}
			}
		}

		if( utility::platformIsNOTWindows() ){

			for( auto&& it : _processManager_volumes() ){

				e.emplace_back( _decode( it.engine,std::move( it.vInfo ) ) ) ;
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
	m_newMountList = _unlocked_volumes() ;

	auto _volumeWasMounted = [ & ](){

		return m_oldMountList.size() < m_newMountList.size() ;
	} ;

	auto _mountedVolume = [ & ]( const QString& e ){

		for( const auto& it : m_oldMountList ){

			if( it.cipherPath() == e ){

				return false ;
			}
		}

		return true ;
	} ;

	if( m_announceEvents ){

		this->pbUpdate() ;

		if( _volumeWasMounted() ){

			for( const auto& it : m_newMountList ){

				if( _mountedVolume( it.cipherPath() ) ){

					this->autoMount( it.mountPoint() ) ;
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
	auto& s = Task::run( [ this ](){

		QFile s( "/proc/self/mountinfo" ) ;

		s.open( QIODevice::ReadOnly ) ;

		eventsMonitor::pollMonitor( s.handle() ).poll( [ this ]( int a ){

			if( m_linuxMonitorExit ){

				return true ;

			}else if( a > 0 ){

				this->updateVolume() ;

			}else if( a == 0 ){

				/*
				 * Timout has occurred
				 */
			}else{
				m_debug( "Warning: pollMonitor.poll failed" ) ;
			}

			return false ;
		} ) ;
	} ) ;

	m_stop = [ this ](){ m_linuxMonitorExit = true ; } ;

	auto m = settings::instance().mountMonitorFolderPaths() ;

	if( !m.isEmpty() ){

		Task::run( [ this,m = std::move( m ) ](){

			using fm = eventsMonitor::folderMonitor ;

			fm mm ;

			if( mm.functional() ){

				mm.createList( m ) ;
			}else{
				m_debug( "Warning: failed to init folder monitor" ) ;
			}

			mm.poll( [ this ]( int r,const fm::entries& entries ){

				if( m_folderMonitorExit ){

					return true ;

				}else if( r == 0 ){

					/*
					 * Timeout
					 */

				}else if( r < 0 ){

					m_debug( "Warning: folderMonitor.poll failed" ) ;
				}else{
					entries.created( [ this ]( const QString& e ){

						this->autoMount( e ) ;
					} ) ;
				}

				return false ;
			} ) ;

		} ).then( std::move( m_quit ) ) ;

		s.then( [ this ](){ m_folderMonitorExit = true ; } ) ;
	}else{
		s.then( std::move( m_quit ) ) ;
	}
}

void mountinfo::pollForUpdates()
{
	m_exit = false ;

	m_stop = [ this ](){ m_exit = true ; } ;

	auto interval = settings::instance().pollForUpdatesInterval() ;

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

void mountinfo::qtMonitor()
{
	this->pollForUpdates() ;
}

void mountinfo::windowsMonitor()
{
}

static QString _gvfs_fuse_path()
{
	auto s = settings::instance().gvfsFuseMonitorPath() ;

	if( s.isEmpty() ){

		for( const auto& it : _unlocked_volumes() ){

			if( it.fileSystem() == "fuse.gvfsd-fuse" ){

				return it.mountPoint() ;
			}
		}
	}

	return s ;
}

dbusMonitor::dbusMonitor( folderMonitor::function function,
			  std::function< void( const QString& ) >& debug ) :
	m_dbus( this ),
	m_folderMonitor( true,debug,_gvfs_fuse_path() ),
	m_function( std::move( function ) ),
	m_debug( debug )
{
	if( !m_folderMonitor.path().isEmpty() ){

		m_debug( "gvfs fuse path: " + m_folderMonitor.path() ) ;

		m_dbus.monitor() ;
	}
}

void dbusMonitor::volumeRemoved()
{
	static folderMonitor::function ss = [ this ]( const QString& e ){

		m_debug( "gvfs fuse unmount: " + e ) ;
	} ;

	m_folderMonitor.contentCountDecreased( ss ) ;
}

void dbusMonitor::volumeAdded()
{
	static folderMonitor::function ss = [ this ]( const QString& e ){

		m_debug( "gvfs fuse mount: " + e ) ;

		m_function( e ) ;
	} ;

	m_folderMonitor.contentCountIncreased( ss ) ;
}

folderMonitor::folderMonitor( bool e,
			      std::function< void( const QString& ) >& debug,
			      const QString& path ) :
	m_path( path ),
	m_folderList( this->folderList() ),m_waitForSynced( e ),
	m_debug( debug )
{
	while( m_path.endsWith( "/" ) ){

		m_path = utility::removeLast( m_path,1 ) ;
	}
}

const QString& folderMonitor::path() const
{
	return m_path ;
}

void folderMonitor::contentCountIncreased( folderMonitor::function& function )
{
	this->folderListSynced( [ & ]( QStringList s ){

		auto e = s ;

		for( const auto& it : utility::asConst( m_folderList ) ){

			s.removeOne( it ) ;
		}

		for( const auto& it : utility::asConst( s ) ){

			function( m_path + "/" + it ) ;
		}

		m_folderList = std::move( e ) ;
	} ) ;
}

void folderMonitor::contentCountDecreased( folderMonitor::function& function )
{
	this->folderListSynced( [ & ]( QStringList s ){

		for( const auto& it : utility::asConst( s ) ){

			m_folderList.removeOne( it ) ;
		}

		for( const auto& it : utility::asConst( m_folderList ) ){

			function( m_path + "/" + it ) ;
		}

		m_folderList = std::move( s ) ;
	} ) ;
}

QStringList folderMonitor::folderList() const
{
	return QDir( m_path ).entryList( QDir::NoDotAndDotDot | QDir::Dirs ) ;
}

void folderMonitor::folderListSynced( std::function< void( QStringList ) > function ) const
{
	if( m_waitForSynced ){

		utility::Timer( 1000,[ &,function = std::move( function ) ]( int counter ){

			auto a = this->folderList() ;

			if( a != m_folderList ){

				m_debug( "gvfs folder is up to date" ) ;

				function( std::move( a ) ) ;

				return true ;

			}else if( counter == 5 ){

				m_debug( "Timed out waiting for gvfs folder to update" ) ;

				return true ;
			}else{
				m_debug( "Waiting for gvfs folder to update" ) ;

				return false ;
			}
		} ) ;
	}else{
		function( this->folderList() ) ;
	}
}
