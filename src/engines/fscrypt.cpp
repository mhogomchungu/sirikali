/*
 *
 *  Copyright (c) 2018
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

#include "fscrypt.h"
#include "../win.h"
#include "../settings.h"
#include "../mountinfo.h"
#include "../json_parser.hpp"

#include <vector>

static const char * _spaceToken = " " ;

static QStringList _encrypted_volumes( const QString& list )
{
	QStringList l ;

	auto s = utility::split( list,'\n' ) ;

	for( int i = s.size() - 1 ; i >= 0 ; i-- ){

		const auto& x =s.at( i ) ;

		if( x.startsWith( "MOUNTPOINT" ) ){

			break ;
		}else{
			auto a = utility::split( x,_spaceToken ) ;

			if( a.size() > 4 ){

				if( a.at( 3 ) == "supported" && a.at( 4 ) == "Yes" ){

					l.append( a.at( 0 ) ) ;
				}
			}
		}
	}

	return l ;
}

static QString _get_fs_mode( const QStringList& s,const QString& m )
{
	for( const auto& it : s ){

		auto a = utility::split( it,' ' ) ;

		if( a.size() > 6 ){

			if( m.startsWith( a.at( 4 ) ) ){

				return a.at( 5 ).mid( 0,2 ) ;
			}
		}
	}

	return "-" ;
}

static QStringList _fscrypt_mount_points( const QString& exe )
{
	auto s = utility::unwrap( utility::Task::run( exe + " status" ) ).stdOut() ;

	if( s.isEmpty() ){

		return {} ;
	}

	return _encrypted_volumes( s ) ;
}

static QString _mount_point( const QString& e,const QString& exe )
{
	for( const auto& it : _fscrypt_mount_points( exe ) ){

		if( e.startsWith( it ) ){

			return it ;
		}
	}

	return QString() ;
}

static QString _volume_properties( const QString& cipherFolder,
				   const QString& mountPoint,
				   const QString& e )
{
	if( e.isEmpty() ){

		return QString() ;
	}

	auto a = utility::split( cipherFolder,':' ) ;

	if( a.size() < 2 ){

		return QString() ;
	}

	auto s = utility::Task::makePath( mountPoint ) ;

	auto exe = utility::Task::makePath( e ) ;

	auto m = _mount_point( mountPoint,exe ) ;

	if( m.isEmpty() ){

		return QString() ;
	}

	exe += " metadata dump --policy=" + m + ":" + a.at( 1 ) ;

	auto r = utility::unwrap( utility::Task::run( exe ) ) ;

	if( r.success() ){

		return r.stdOut() ;
	}else{
		return QString() ;
	}
}

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = false ;
	s.backendRequireMountPath     = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = true ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "fscrypt" ;
	s.incorrectPasswordText = "incorrect key provided" ;
	s.configFileNames       = QStringList{ ".fscrypt" } ;
	s.fuseNames             = QStringList{ "fscrypt","fscrypt*" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "fscrypt","fscrypt*" } ;
	s.volumePropertiesCommands = QStringList{ "fscrypt status %{plainFolder}" } ;
	s.notFoundCode             = engines::engine::status::fscryptNotFound ;

	return s ;
}

fscrypt::fscrypt() : engines::engine( _setOptions() )
{	
}

engines::engine::status fscrypt::unmount( const QString& cipherFolder,
					  const QString& mountPoint,
					  int maxCount) const
{
	Q_UNUSED( cipherFolder )

	auto exe = this->executablePath() ;

	if( exe.isEmpty() ){

		return engines::engine::status::failedToUnMount ;
	}

	exe = utility::Task::makePath( exe ) ;

	exe += " lock " + mountPoint + " " + this->userOption() ;

	for( int i = 0 ; i < maxCount ; i++ ){

		auto s = utility::unwrap( utility::Task::run( exe,false ) ) ;

		if( s.success() ){

			m_unlockedVolumeManager.removeEntry( mountPoint ) ;

			return engines::engine::status::success ;

		}else if( s.stdError().contains( "Directory was incompletely locked because some files are still open" ) ){

			return engines::engine::status::fscryptPartialVolumeClose ;
		}
	}

	return engines::engine::status::failedToUnMount ;
}

struct mountInfo{
	const QStringList& mountInfo ;
	const QStringList& mountedVolumes ;
	const QStringList& fuseNames ;
	const QString& exe ;
} ;

static QString _sanitize( const QStringList& m )
{
	if( m.size() > 1 ){

		auto s = m.at( 1 ) ;

		while( s.contains( ' ' ) ){

			s.replace( " ","" ) ;
		}

		return s ;
	}else{
		return QString() ;
	}
}

template< typename Function >
static QStringList _mountInfo( const mountInfo& e,Function removeEntry )
{
	QStringList l ;

	QString unlockedName      = e.fuseNames.at( 0 ) ;
	QString unlockedPartially = e.fuseNames.at( 1 ) ;

	for( const auto& it : e.mountedVolumes ){

		auto s = utility::Task::run( e.exe + " status " + it ).get() ;

		if( s.success() ){

			QString policy ;
			QString unlockStatus ;

			auto md = _get_fs_mode( e.mountInfo,it ) ;

			for( const auto& xt : utility::split( s.stdOut(),'\n' ) ){

				if( xt.startsWith( "Policy:" ) ){

					auto m = utility::split( xt,':' ) ;

					policy = utility::policyString() + _sanitize( m ) ;

				}else if( xt.startsWith( "Unlocked:" ) ){

					auto m = utility::split( xt,':' ) ;

					unlockStatus = _sanitize( m ) ;
				}
			}

			if( unlockStatus == "Yes" ){

				l.append( mountinfo::mountProperties( it,md,unlockedName,policy ) ) ;

			}else if( unlockStatus.startsWith( "Partially" ) ){

				l.append( mountinfo::mountProperties( it,md,unlockedPartially,policy ) ) ;
			}else{
				removeEntry( it ) ;
			}
		}else{
			removeEntry( it ) ;
		}
	}

	return l ;
}

QStringList fscrypt::mountInfo( const QStringList& a ) const
{
	auto exe = this->executablePath() ;

	if( exe.isEmpty() ){

		return {} ;
	}

	return Task::await( [ & ](){

		auto list = m_unlockedVolumeManager.getList() ;
		const auto& names = this->fuseNames() ;

		return _mountInfo( { a,list,names,exe },[ this ]( const QString& e ){

			m_unlockedVolumeManager.removeEntry( e ) ;
		} ) ;
	} ) ;
}

const engines::engine& fscrypt::proveEngine( const QString& cipherPath ) const
{
	if( !utility::platformIsLinux() ){

		return engines::instance().getUnKnown() ;
	}

	auto exe = this->executablePath() ;

	if( exe.isEmpty() ){

		return engines::instance().getUnKnown() ;
	}else{
		auto m = utility::Task::makePath( cipherPath ) ;

		exe = utility::Task::makePath( exe ) ;

		auto s = utility::unwrap( utility::Task::run( exe + " status " + m ) ).success() ;

		if( s ){

			return *this ;
		}else{
			return engines::instance().getUnKnown() ;
		}
	}
}

void fscrypt::updateVolumeList( const engines::engine::options& e ) const
{
	m_unlockedVolumeManager.addEntry( e.cipherFolder ) ;
}

Task::future< QString >& fscrypt::volumeProperties( const QString& cipherFolder,
						    const QString& mountPoint ) const
{
	return Task::run( [ = ](){

		return _volume_properties( cipherFolder,mountPoint,this->executablePath() ) ;
	} ) ;
}

engines::engine::args fscrypt::command( const QByteArray& password,
					const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	auto e = QString( "%1 %2 %3" ) ;

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.create ){

		auto m = "encrypt --source=custom_passphrase --name=%1 --quiet %2" ;

		auto n = utility::split( args.opt.cipherFolder,'/' ).last() ;

		if( n.isEmpty() ){

			n = "root" ;
		}

		auto s = QString( m ).arg( n,this->userOption() ) ;

		exeOptions.add( s ) ;
	}else{
		exeOptions.add( QString( "unlock --quiet %1" ).arg( this->userOption() ) ) ;
	}

	auto cmd = e.arg( args.exe,
			  exeOptions.get(),
			  args.cipherFolder ) ;

	return { args,m,cmd } ;
}

engines::engine::status fscrypt::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::fscryptBadPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString fscrypt::installedVersionString() const
{
	return QString() ;
}

void fscrypt::GUICreateOptionsinstance( QWidget *,engines::engine::function ) const
{
	//fscryptcreateoptions::instance( parent,std::move( function ) ) ;
}

#ifdef Q_OS_WIN

QString fscrypt::userOption() const
{
	return QString() ;
}

#else

#include <pwd.h>

QString fscrypt::userOption() const
{
	return QString( "--user=\"%1\"" ).arg( getpwuid( getuid() )->pw_name ) ;
}

const QString& fscrypt::executablePath() const
{
	if( m_exe.isEmpty() ){

		m_exe = this->executableFullPath() ;
	}

	return m_exe ;
}

#endif

static QString _setOption()
{
	auto a = settings::instance().ConfigLocation() + "/fscrypt" ;
	QDir().mkpath( a ) ;
	return a + "/unlockedList.json" ;
}

fscrypt::unlockedVolumeList::unlockedVolumeList() :
	m_configFilePath( _setOption() )
{
}

static void _log_error( const QString& msg,const QString& path )
{
	auto a = "\nFailed to parse fscrypt config file: " + path ;
	utility::debug::cout() << msg + a ;
}

QStringList fscrypt::unlockedVolumeList::getList() const
{
	try {
		SirikaliJson json( m_configFilePath,SirikaliJson::type::PATH ) ;

		return json.getStringList( m_keyName ) ;

	}catch( const SirikaliJson::exception& e ){

		_log_error( e.what(),m_configFilePath ) ;

	}catch( const std::exception& e ){

		_log_error( e.what(),m_configFilePath ) ;

	}catch( ... ){

		_log_error( "Unknown error has occured","ff" ) ;
	}

	return {} ;
}

void fscrypt::unlockedVolumeList::updateList( const QStringList& e )
{
	try {
		SirikaliJson json ;

		json[ m_keyName ] = e ;

		QFile::remove( m_configFilePath ) ;

		json.toFile( m_configFilePath ) ;

	}catch( const SirikaliJson::exception& e ){

		_log_error( e.what(),m_configFilePath ) ;

	}catch( const std::exception& e ){

		_log_error( e.what(),m_configFilePath ) ;

	}catch( ... ){

		_log_error( "Unknown error has occured","ff" ) ;
	}
}

void fscrypt::unlockedVolumeList::addEntry( const QString& e )
{
	auto a = this->getList() ;

	a.append( e ) ;

	this->updateList( a ) ;
}

void fscrypt::unlockedVolumeList::removeEntry( const QString& e )
{
	auto a = this->getList() ;

	a.removeAll( e.mid( 1,e.length() - 2 ) ) ;

	this->updateList( a ) ;
}
