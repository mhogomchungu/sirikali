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

struct mountInfo{
	const QStringList& mountInfo ;
	const QStringList& mountedVolumes ;
	const QStringList& fuseNames ;
	const QString& exe ;
} ;

static QStringList _encrypted_volumes( const QString& list )
{
	QStringList l ;

	auto s = utility::split( list,'\n' ) ;

	for( int i = s.size() - 1 ; i >= 0 ; i-- ){

		const auto& x =s.at( i ) ;

		if( x.startsWith( "MOUNTPOINT" ) ){

			break ;
		}else{
			auto a = utility::split( x," " ) ;

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

static utility::Task _run( const QString& cmd )
{
	return utility::unwrap( utility::Task::run( cmd ) ) ;
}

static QStringList _fscrypt_mount_points( const QString& exe )
{
	auto s = _run( exe + " status" ).stdOut() ;

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

static QString _property( const QString& exe,const QString& m,const QString& opt )
{
	auto s = _run( exe + " status " + m ) ;

	if( s.success() ){

		for( const auto& xt : utility::split( s.stdOut(),'\n' ) ){

			if( xt.startsWith( opt ) ){

				auto m = utility::split( xt,':' ) ;

				return _sanitize( m ) ;
			}
		}
	}

	return QString() ;
}

template< typename Function >
static QStringList _mountInfo( const mountInfo& e,Function removeEntry )
{
	QStringList l ;

	const auto& a = e.fuseNames.at( 0 ) ;
	const auto& b = e.fuseNames.at( 1 ) ;

	for( const auto& it : e.mountedVolumes ){

		auto s = _property( e.exe,it,"Unlocked:" ) ;

		if( !s.isEmpty() ){

			auto md = _get_fs_mode( e.mountInfo,it ) ;

			if( s == "Yes" ){

				l.append( mountinfo::mountProperties( it,md,a,it ) ) ;

			}else if( s.startsWith( "Partially" ) ){

				l.append( mountinfo::mountProperties( it,md,b,it ) ) ;
			}else{
				removeEntry( it ) ;
			}
		}else{
			removeEntry( it ) ;
		}
	}

	return l ;
}

static QString _volume_properties( const QString& cipherFolder,
				   const QString& mountPoint,
				   const QString& e )
{
	Q_UNUSED( cipherFolder )

	if( e.isEmpty() ){

		return QString() ;
	}

	auto exe = utility::Task::makePath( e ) ;

	auto a = _property( exe,mountPoint,"Policy:" ) ;

	if( a.isEmpty() ){

		return QString() ;
	}

	auto s = utility::Task::makePath( mountPoint ) ;

	auto m = _mount_point( mountPoint,exe ) ;

	if( m.isEmpty() ){

		return QString() ;
	}

	auto r = _run( exe + " metadata dump --policy=" + m + ":" + a ) ;

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
	s.releaseURL            = "https://api.github.com/repos/google/fscrypt/releases" ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "fscrypt" ;
	s.incorrectPasswordText = "incorrect key provided" ;
	s.fuseNames             = QStringList{ "fscrypt","fscrypt*" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "fscrypt","fscrypt*" } ;
	s.volumePropertiesCommands = QStringList{ "fscrypt status %{plainFolder}" } ;
	s.notFoundCode             = engines::engine::status::fscryptNotFound ;
	s.versionInfo              = { { "--version",true,0,2 },    // for fscrypt < 0.2.7
				       { "--version",true,2,0 } } ; // for fscrypt >= 0.2.7

	return s ;
}

fscrypt::fscrypt() :
	engines::engine( _setOptions() ),
	m_versionGreatorOrEqual_0_2_6( true,*this,0,2,6 )
{
}

engines::engine::status fscrypt::unmount( const QString& cipherFolder,
					  const QString& mountPoint,
					  int maxCount) const
{
	Q_UNUSED( cipherFolder )

	const auto& e = this->executableFullPath() ;

	if( e.isEmpty() ){

		return engines::engine::status::failedToUnMount ;
	}

	auto exe = utility::Task::makePath( e ) ;

	if( m_versionGreatorOrEqual_0_2_6 ){

		exe += " lock " + mountPoint + " " + this->userOption() ;
	}else{
		auto mp = utility::removeFirstAndLast( mountPoint,1,1 ) ;

		auto m = utility::Task::makePath( _mount_point( mp,exe ) ) ;

		exe += " purge " + m + " --force --drop-caches=false " + this->userOption() ;
	}

	for( int i = 0 ; i < maxCount ; i++ ){

		auto s = _run( exe ) ;

		if( s.success() ){

			m_unlockedVolumeManager.removeEntry( mountPoint ) ;

			return engines::engine::status::success ;

		}else if( s.stdError().contains( "Directory was incompletely locked because some files are still open" ) ){

			return engines::engine::status::fscryptPartialVolumeClose ;
		}
	}

	return engines::engine::status::failedToUnMount ;
}

QStringList fscrypt::mountInfo( const QStringList& a ) const
{
	const auto& exe = this->executableFullPath() ;

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

bool fscrypt::ownsCipherPath( const QString& cipherPath ) const
{
	const auto& exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return false ;
	}else{
		auto m = utility::Task::makePath( cipherPath ) ;

		auto e = utility::Task::makePath( exe ) ;

		auto s = _run( e + " status " + m ) ;

		if( s.success() ){

			return s.stdOut().contains( "is encrypted with fscrypt" ) ;
		}else{
			return false ;
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

		return _volume_properties( cipherFolder,mountPoint,this->executableFullPath() ) ;
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

void fscrypt::GUICreateOptionsinstance( QWidget *,engines::engine::function ) const
{
	//fscryptcreateoptions::instance( parent,std::move( function ) ) ;
}

#ifdef Q_OS_LINUX

#include <pwd.h>

QString fscrypt::userOption() const
{
	return QString( "--user=\"%1\"" ).arg( getpwuid( getuid() )->pw_name ) ;
}

#else

QString fscrypt::userOption() const
{
	return QString() ;
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

		_log_error( "Unknown error has occured",m_configFilePath ) ;
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

		_log_error( "Unknown error has occured",m_configFilePath ) ;
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

	if( a.removeAll( utility::removeFirstAndLast( e,1,1 ) ) ){

		this->updateList( a ) ;
	}
}
