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
#include "fscryptcreateoptions.h"
#include "options.h"

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
	utility::debug() << cmd ;

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
	auto s = _run( exe + " status " + utility::Task::makePath( m ) ) ;

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

		auto tt = engines::engine::decodeSpecialCharactersConst( it ) ;

		auto s = _property( e.exe,tt,"Unlocked:" ) ;

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

static QStringList _get_protector_names( const QString& exe,const QString& mountPoint )
{
	auto r = utility::split( _run( exe + " status " + mountPoint ).stdOut() ) ;

	QStringList l ;

	for( int i = 0 ; i < r.size() ; i++ ){

		if( r.at( i ).startsWith( "PROTECTOR" ) ){

			for( int j = i + 1 ; j < r.size() ; j++ ){

				const auto& m = r.at( j ) ;

				if( m.startsWith( "POLICY" ) ){

					break ;
				}else{
					auto a = m.indexOf( '"' ) ;
					auto b = m.lastIndexOf( '"' ) ;

					if( a != -1 && b != -1 && a != b ){

						l.append( m.mid( a + 1,b - a - 1 ) ) ;
					}
				}
			}

			break ;
		}
	}

	return l ;
}

static QString _name( const QString& cipherPath,const QString& e,const QString& exe )
{
	auto ee = utility::Task::makePath( exe ) ;

	auto mp = utility::Task::makePath( _mount_point( cipherPath,ee ) ) ;

	auto mm = _get_protector_names( ee,mp ) ;

	auto _contains = [ & ]( const QString& e ){

		return mm.contains( e,Qt::CaseInsensitive ) ;
	} ;

	if( _contains( e ) ){

		for( int i = 0 ; i < 1000 ; i++ ){

			auto s = e + "_" + QString::number( i ) ;

			if( !_contains( s ) ){

				return s ;
			}
		}
	}

	return e ;
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
	s.hasGUICreateOptions   = true ;
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
	s.versionInfo              = { { "--version",true,2,0 },    // for fscrypt >= 0.2.7
				       { "--version",true,0,2 } } ; // for fscrypt < 0.2.7

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

		exe += " lock " + mountPoint ;
	}else{
		auto mp = utility::removeFirstAndLast( mountPoint,1,1 ) ;

		auto m = utility::Task::makePath( _mount_point( mp,exe ) ) ;

		exe += " purge " + m + " --force --drop-caches=false" ;
	}

	for( int i = 0 ; i < maxCount ; i++ ){

		auto s = _run( exe ) ;

		if( s.success() ){

			m_unlockedVolumeManager.removeEntry( mountinfo::encodeMountPath( mountPoint ) ) ;

			return engines::engine::status::success ;

		}else if( s.stdError().contains( "Directory was incompletely locked because some files are still open" ) ){

			return engines::engine::status::fscryptPartialVolumeClose ;
		}
	}

	return engines::engine::status::failedToUnMount ;
}

QStringList fscrypt::mountInfo( const QStringList& a ) const
{
	auto exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return {} ;
	}

	exe = utility::Task::makePath( exe ) ;

	auto list = m_unlockedVolumeManager.getList() ;
	const auto& names = this->fuseNames() ;

	return _mountInfo( { a,list,names,exe },[ this ]( const QString& e ){

		m_unlockedVolumeManager.removeEntry( e ) ;
	} ) ;
}

engines::engine::ownsCipherFolder fscrypt::ownsCipherPath( const QString& cipherPath,
							   const QString& configPath ) const
{
	Q_UNUSED( configPath )

	if( QFileInfo( cipherPath ).isFile() ){

		return { false,cipherPath,configPath } ;
	}

	const auto& exe = this->executableFullPath() ;

	if( !exe.isEmpty() ){

		auto m = utility::Task::makePath( cipherPath ) ;

		auto e = utility::Task::makePath( exe ) ;

		auto s = _run( e + " status " + m ) ;

		if( s.success() && s.stdOut().contains( "is encrypted with fscrypt" ) ){

			return { true,cipherPath,configPath } ;
		}
	}

	return { false,cipherPath,configPath } ;
}

bool fscrypt::requiresAPassword( const engines::engine::cmdArgsList::options& opt ) const
{
	if( opt.keyFile.isEmpty() ){

		return engines::engine::requiresAPassword( opt ) ;
	}else{
		return false ;
	}
}

void fscrypt::updateVolumeList( const engines::engine::cmdArgsList::options& e ) const
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

	exeOptions.add( "--quiet" ) ;

	if( !args.opt.keyFile.isEmpty() ){

		exeOptions.add( "--key=" + utility::Task::makePath( args.opt.keyFile ) ) ;
	}

	if( args.create ){

		QString n ;

		QString ss = "--source=custom_passphrase" ;

		for( const auto& it : utility::split( args.opt.createOptions,"\\040" ) ){

			if( it.startsWith( "--name=" ) ){

				n = it.mid( 7 ) ;

			}else if( it.startsWith( "--source=" ) ){

				ss = it ;
			}else{
				exeOptions.add( it ) ;
			}
		}

		exeOptions.add( ss ) ;

		if( ss != "--source=pam_passphrase" ){

			if( n.isEmpty() ){

				n = utility::split( args.opt.cipherFolder,'/' ).last() ;

				if( n.isEmpty() ){

					n = "root" ;
				}
			}

			auto nn = _name( args.opt.cipherFolder,n,this->executableFullPath() ) ;

			exeOptions.add( "--name=\"" + nn + "\"" ) ;
		}

		auto cmd = e.arg( args.exe + " encrypt",exeOptions.get(),args.cipherFolder ) ;

		return { args,m,cmd } ;
	}else{
		auto cmd = e.arg( args.exe + " unlock",exeOptions.get(),args.cipherFolder ) ;

		return { args,m,cmd } ;
	}
}

engines::engine::status fscrypt::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::fscryptBadPassword ;

	}else if( e.contains( "fscrypt unlock: no key file specified" ) ){

		return engines::engine::status::fscryptKeyFileRequired ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

void fscrypt::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	fscryptcreateoptions::instance( s,{} ) ;
}

void fscrypt::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableCheckBox     = false ;
	ee.enableIdleTime     = false ;
	ee.enableMountOptions = false ;
	ee.enableConfigFile   = false ;

	ee.keyFileTitle = QObject::tr( "Unlock Fscrypt Volume With A Specified 32 Byte(256-Bit) KeyFile." ) ;

	e.ShowUI() ;
}

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

	a.append( mountinfo::encodeMountPath( e ) ) ;

	this->updateList( a ) ;
}

void fscrypt::unlockedVolumeList::removeEntry( const QString& e )
{
	auto a = this->getList() ;

	if( a.removeAll( e ) ){

		this->updateList( a ) ;
	}
}
