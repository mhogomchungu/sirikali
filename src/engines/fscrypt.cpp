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

#include "custom.h"

struct mountInfo{
	const volumeInfo::List& mountInfo ;
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

static QString _get_fs_mode( const volumeInfo::List& s,const QString& m )
{
	for( const auto& it : s ){

		if( m.startsWith( it.mountPoint() ) ){

			return it.mode() ;
		}
	}

	return "rw" ;
}

static utility::Task _run( const QString& cmd,
			   const QStringList& list,
			   const engines::engine& engine,
			   bool debug = false )
{
	if( debug ){

		auto exe = cmd ;

		for( const auto& it : list ){

			exe += " \"" + it + "\"" ;
		}

		utility::debug() << exe ;
	}

	auto& e = utility::Task::run( cmd,list,{ false,false,engine.allowLogging( list ) } ) ;

	return utility::unwrap( e ) ;
}

static QStringList _fscrypt_mount_points( const QString& exe,const engines::engine& engine )
{
	auto s = _run( exe,{ "status" },engine ).stdOut() ;

	if( s.isEmpty() ){

		return {} ;
	}

	return _encrypted_volumes( s ) ;
}

static QString _mount_point( const QString& e,const QString& exe,const engines::engine& engine )
{
	for( const auto& it : _fscrypt_mount_points( exe,engine ) ){

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

static QString _property( const QString& exe,
			  const QString& m,
			  const QString& opt,
			  const engines::engine& engine )
{
	auto s = _run( exe,{ "status",m },engine ) ;

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
static volumeInfo::List _mountInfo( const mountInfo& e,
				    const engines::engine& engine,
				    Function removeEntry )
{
	volumeInfo::List l ;

	const auto& a = e.fuseNames.at( 0 ) ;
	const auto& b = e.fuseNames.at( 1 ) ;

	auto userID = "user_id=" + utility::userIDAsString() ;

	for( const auto& it : e.mountedVolumes ){

		auto tt = engines::engine::decodeSpecialCharactersConst( it ) ;

		auto s = _property( e.exe,tt,"Unlocked:",engine ) ;

		if( !s.isEmpty() ){

			auto md = _get_fs_mode( e.mountInfo,it ) ;

			if( s == "Yes" ){

				l.emplace_back( it,it,a,md,userID ) ;

			}else if( s.startsWith( "Partially" ) ){

				l.emplace_back( it,it,b,md,userID ) ;
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
				   const QString& exe,
				   const engines::engine& engine )
{
	Q_UNUSED( cipherFolder )

	if( exe.isEmpty() ){

		return QString() ;
	}

	auto a = _property( exe,mountPoint,"Policy:",engine ) ;

	if( a.isEmpty() ){

		return QString() ;
	}

	auto m = _mount_point( mountPoint,exe,engine ) ;

	if( m.isEmpty() ){

		return QString() ;
	}

	auto r = _run( exe,{ "metadata","dump","--policy=" + m + ":" + a },engine ) ;

	if( r.success() ){

		return r.stdOut() ;
	}else{
		return QString() ;
	}
}

static QStringList _get_protector_names( const QString& exe,
					 const QString& mountPoint,
					 const engines::engine& engine )
{
	auto r = utility::split( _run( exe,{ "status",mountPoint },engine ).stdOut() ) ;

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

static QString _name( const QString& cipherPath,
		      const QString& e,
		      const QString& exe,
		      const engines::engine& engine )
{
	auto mp = _mount_point( cipherPath,exe,engine ) ;

	auto mm = _get_protector_names( exe,mp,engine ) ;

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
	s.backendRunsInBackGround     = true ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
	s.likeSsh               = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.releaseURL            = "https://api.github.com/repos/google/fscrypt/releases" ;
	s.passwordFormat        = "%{password}" ;
	s.incorrectPasswordText = "incorrect key provided" ;
	s.executableNames       = QStringList{ "fscrypt" } ;
	s.fuseNames             = QStringList{ "fscrypt","fscrypt*" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "fscrypt","fscrypt*" } ;
	s.volumePropertiesCommands = QStringList{ "fscrypt status %{plainFolder}" } ;
	s.notFoundCode             = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo              = { { "--version",true,2,0 },    // for fscrypt >= 0.2.7
				       { "--version",true,0,2 } } ; // for fscrypt < 0.2.7

	s.createControlStructure = "encrypt --quiet %{createOptions} %{cipherFolder}" ;
	s.mountControlStructure  = "unlock --quiet %{mountOptions} %{cipherFolder}" ;

	s.keyFileArgument        = "--key=%{keyfile}" ;

	return s ;
}

fscrypt::fscrypt() :
	engines::engine( _setOptions() ),
	m_versionGreatorOrEqual_0_2_6( true,*this,0,2,6 )
{
}

engines::engine::status fscrypt::unmount( const engines::engine::unMount& e ) const
{
	const auto& exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return engines::engine::status::failedToUnMount ;
	}

	engines::engine::commandOptions m ;

	auto exeOptions = m.exeOptions() ;

	if( m_versionGreatorOrEqual_0_2_6 ){

		exeOptions.add( "lock",e.mountPoint ) ;
	}else{
		auto m = _mount_point( e.mountPoint,exe,*this ) ;

		exeOptions.add( "purge",m,"--force","--drop-caches=false" ) ;
	}

	for( int i = 0 ; i < e.numberOfAttempts ; i++ ){

		auto s = _run( exe,exeOptions.get(),*this ) ;

		if( s.success() ){

			m_unlockedVolumeManager.removeEntry( engines::engine::encodeMountPath( e.mountPoint ) ) ;

			return engines::engine::status::success ;

		}else if( s.stdError().contains( "Directory was incompletely locked because some files are still open" ) ){

			return engines::engine::status::fscryptPartialVolumeClose ;
		}
	}

	return engines::engine::status::failedToUnMount ;
}

volumeInfo::List fscrypt::mountInfo( const volumeInfo::List& a ) const
{
	auto exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return {} ;
	}

	auto list = m_unlockedVolumeManager.getList() ;
	const auto& names = this->fuseNames() ;

	return _mountInfo( { a,list,names,exe },*this,[ this ]( const QString& e ){

		m_unlockedVolumeManager.removeEntry( e ) ;
	}) ;
}

engines::engine::ownsCipherFolder fscrypt::ownsCipherPath( const QString& cipherPath,
							   const QString& configPath ) const
{
	if( cipherPath.startsWith( "fscrypt ",Qt::CaseInsensitive ) ){

		return { true,cipherPath.mid( 8 ),configPath } ;
	}

	Q_UNUSED( configPath )

	if( QFileInfo( cipherPath ).isFile() ){

		return { false,cipherPath,configPath } ;
	}

	const auto& exe = this->executableFullPath() ;

	if( !exe.isEmpty() ){

		auto s = _run( exe,{ "status",cipherPath },*this ) ;

		if( s.success() && s.stdOut().contains( "is encrypted with fscrypt" ) ){

			return { true,cipherPath,configPath } ;
		}
	}

	return { false,cipherPath,configPath } ;
}

utility2::LOGLEVEL fscrypt::allowLogging( const QStringList& args ) const
{
	if( args.size() > 0 ){

		const auto& e = args.at( 0 ) ;

		if( e == "unlock" ){

			return utility2::LOGLEVEL::COMMAND_AND_UNLOCK_DURATION ;

		}else if( e == "status" ){

			return utility2::LOGLEVEL::NOLOG ;
		}else{
			return utility2::LOGLEVEL::COMMAND_ONLY ;
		}
	}else{
		return utility2::LOGLEVEL::COMMAND_ONLY ;
	}
}

bool fscrypt::requiresAPassword( const engines::engine::cmdArgsList& opt ) const
{
	if( opt.createOptions.contains( "--source=raw_key" ) ){

		return false ;

	}else if( opt.keyFile.isEmpty() ){

		return engines::engine::requiresAPassword( opt ) ;
	}else{
		return false ;
	}
}

void fscrypt::updateVolumeList( const engines::engine::cmdArgsList& e ) const
{
	m_unlockedVolumeManager.addEntry( e.cipherFolder ) ;
}

Task::future< QString >& fscrypt::volumeProperties( const QString& cipherFolder,
						    const QString& mountPoint ) const
{
	return Task::run( [ this,cipherFolder,mountPoint ](){

		return _volume_properties( cipherFolder,mountPoint,this->executableFullPath(),*this ) ;
	} ) ;
}

void fscrypt::updateOptions( engines::engine::cmdArgsList& args,bool creating ) const
{
	if( creating ){

		QStringList opts ;

		QString n ;

		QString ss = "--source=custom_passphrase" ;

		for( const auto& it : args.createOptions ){

			if( it.startsWith( "--name=" ) ){

				n = it.mid( 7 ) ;

			}else if( it.startsWith( "--source=" ) ){

				ss = it ;

			}else if( !it.isEmpty() ){

				opts.append( it ) ;
			}
		}

		opts.append( ss ) ;

		if( ss != "--source=pam_passphrase" ){

			if( n.isEmpty() ){

				n = utility::split( args.cipherFolder,'/' ).last() ;

				if( n.isEmpty() ){

					n = "root" ;
				}
			}

			auto nn = _name( args.cipherFolder,n,this->executableFullPath(),*this ) ;

			opts.append( "--name=" + nn ) ;
		}

		args.createOptions = opts ;
	}else{
		if( !args.keyFile.isEmpty() ){

			args.key = "\n" ;
		}
	}
}

engines::engine::status fscrypt::errorCode( const QString& e,int s ) const
{
	if( e.contains( "fscrypt unlock: no key file specified" ) ){

		return engines::engine::status::fscryptKeyFileRequired ;
	}else{
		return engines::engine::errorCode( e,s ) ;
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
	if( utility::pathNotExists( m_configFilePath ) ){

		this->updateList( QStringList() ) ;
	}
}

QStringList fscrypt::unlockedVolumeList::getList() const
{
	utility::logger logger ;

	SirikaliJson json( QFile( m_configFilePath ),logger.function() ) ;

	if( json.passed() ){

		return json.getStringList( m_keyName ) ;
	}else {
		return {} ;
	}
}

void fscrypt::unlockedVolumeList::updateList( const QStringList& e )
{
	utility::logger logger ;

	SirikaliJson json( logger.function() ) ;

	json[ m_keyName ] = e ;

	if( json.passed() ){

		QFile::remove( m_configFilePath ) ;

		json.toFile( m_configFilePath ) ;
	}
}

void fscrypt::unlockedVolumeList::addEntry( const QString& e )
{
	auto a = this->getList() ;

	a.append( engines::engine::encodeMountPath( e ) ) ;

	this->updateList( a ) ;
}

void fscrypt::unlockedVolumeList::removeEntry( const QString& e )
{
	auto a = this->getList() ;

	if( a.removeAll( e ) ){

		this->updateList( a ) ;
	}
}
