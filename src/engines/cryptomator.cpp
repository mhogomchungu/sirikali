/*
 *
 *  Copyright (c) 2020
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

#include "cryptomator.h"

#include "options.h"

#include "custom.h"

#include "../crypto.h"

#include "../utils/threads.hpp"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = false ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = false ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
	s.usesFuseArgumentSwitch      = false ;
	s.likeSsh               = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = false ;
	s.releaseURL            = "https://api.github.com/repos/cryptomator/cli/releases" ;
	s.passwordFormat        = "" ;
	s.configFileArgument    = "" ;
	s.keyFileArgument       = "" ;
	s.incorrectPasswordText    = QStringList{ "InvalidPassphraseException" } ;
	s.volumePropertiesCommands = QStringList{} ;
	s.configFileNames          = QStringList{ "vault.cryptomator" } ;
	s.fuseNames                = QStringList{ "fuse.cryptomator" } ;
	s.names                    = QStringList{ "cryptomator" } ;
	s.failedToMountList        = QStringList{ " ERROR ","Exception","fuse: unknown option" } ;
	s.successfulMountedList    = QStringList{ "Unlocked and mounted vault successfully" } ;
	s.notFoundCode             = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo              = { { "--version",true,0,0 } } ;
	s.versionMinimum           = "0.6.0" ;
	s.createControlStructure   = "" ;
	s.executableNames          = QStringList{ "cryptomator-cli" } ;

	QString m = "unlock --password:stdin --mounter=%1 --mountPoint %{mountPoint} %{cipherFolder}" ;

	if( utility::platformIsWindows() ){

		s.windowsUnMountCommand = QStringList{ "sirikali.exe", "-T","%{PID}" } ;

		auto a = "org.cryptomator.frontend.fuse.mount.WinFspMountProvider" ;

		s.mountControlStructure = m.arg( a ) ;
	}else{
		s.unMountCommand = QStringList{ "kill","-s","SIGTERM","%{PID}" } ;

		auto a = "org.cryptomator.frontend.fuse.mount.LinuxFuseMountProvider" ;

		s.mountControlStructure = m.arg( a ) ;
	}

	return s ;
}

cryptomator::cryptomator() :
	engines::engine( _setOptions() ),
	m_version_greater_or_equal_0_6_0( true,*this,this->minimumVersion() )
{
}

bool cryptomator::updatable( bool s ) const
{
	if( s ){

		if( utility::platformIsFlatPak() ){

			return utility::archInUseIsAnyOf( utility::arch::x64,utility::arch::aarch64 ) ;

		}else if( utility::platformIsWindows() ){

			return utility::archInUse( utility::arch::x64 ) ;
		}else{
			return false ;
		}
	}else{
		return false ;
	}
}

void cryptomator::deleteBinPath( const QString& e ) const
{
	auto m = e + "/cryptomator-cli" ;

	if( QFile::exists( e ) ){

		auto p = m + ".old" ;

		QDir().rename( m,p ) ;

		utils::qthread::run( [ p ](){

			QDir( p ).removeRecursively() ;
		} ) ;
	}
}

bool cryptomator::onlineArchiveFileName( const QString& e ) const
{
	if( utility::platformIsLinux() ){

		if( utility::archInUse( utility::arch::x64 ) ){

			return e.endsWith( "linux-x64.zip" ) ;

		}else if( utility::archInUse( utility::arch::aarch64 ) ){

			return e.endsWith( "linux-aarch64.zip" ) ;
		}else{
			return false ;
		}

	}else if( utility::platformIsWindows() ){

		return e.endsWith( "win-x64.zip" ) ;
	}else{
		return false ;
	}
}

bool cryptomator::enginesMatch( const QString& e ) const
{
	return this->name() + "-cli" == e ;
}

void cryptomator::setUpBinary( bool add,QStringList& apps,const QString& basePath ) const
{
	auto m = basePath + "/cryptomator-cli/" ;

	engines::engine::setUpBinary( add,apps,m,"cryptomator-cli" ) ;
}

engines::engine::terminate_result
cryptomator::terminateProcess( int attempts,const engines::engine::terminate_process& e ) const
{
	if( utility::platformIsFlatPak() ){

		utility::logger logger ;

		auto exe = "flatpak-spawn" ;

		QStringList args{ "--host",this->fuserMountPath(),"-u",e.mountPath() } ;

		const auto& env = this->getProcessEnvironment() ;

		logger.showText( exe,args ) ;

		auto m = utility::unwrap( Task::process::run( exe,args,-1,"",env ) ) ;

		logger.showText( m ) ;

		if( m.success() ){

			return engines::engine::terminateProcess( attempts,e ) ;
		}else{
			for( int i = 1 ; i < attempts ; i++ ){

				logger.showText( exe,args ) ;

				utility::Task::suspendForOneSecond() ;

				m = utility::unwrap( Task::process::run( exe,args,-1,"",env ) ) ;

				logger.showText( m ) ;

				if( m.success() ){

					return engines::engine::terminateProcess( attempts,e ) ;
				}
			}
		}

		return { std::move( m ),std::move( exe ),std::move( args ) } ;
	}else{
		return engines::engine::terminateProcess( attempts,e ) ;
	}
}

QString cryptomator::setExecutablePermissions( const QString& e ) const
{
	if( utility::platformIsNOTWindows() ){

		if( QFile::exists( e + "/target" ) ){

			QDir().rename( e + "/target/cryptomator-cli",e + "/cryptomator-cli" ) ;

			QDir( e + "/target" ).removeRecursively() ;
		}

		auto m = e + "/cryptomator-cli/bin/cryptomator-cli" ;

		QFile f( m ) ;

		f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

		return m ;
	}else{
		return e + "/cryptomator-cli/cryptomator-cli.exe" ;
	}
}

void cryptomator::changeEnvironment( const QString&,const QStringList& args ) const
{
	if( args.size() > 1 ){

		auto m = QFileInfo( args[ 1 ] ).dir().absolutePath() ;

		QDir::setCurrent( m ) ;
	}
}

engines::engine::args cryptomator::command( const QByteArray& password,const cmdArgsList& args,bool create ) const
{
	if( utility::platformIsFlatPak() ){

		auto m = engines::engine::command( password,args,create ) ;

		m.cmd_args.prepend( m.cmd ) ;
		m.cmd_args.prepend( "--host" ) ;

		m.cmd = "flatpak-spawn" ;

		return m ;
	}else{
		return engines::engine::command( password,args,create ) ;
	}
}

void cryptomator::updateOptions( QStringList& opts,
				 const engines::engine::cmdArgsList& args,
				 bool create ) const
{
	for( int i = 0 ; i < opts.size() ; i++ ){

		if( opts[ i ] == "--mountPoint" && i + 1 < opts.size() ){

			opts[ i ] += "=" + opts[ i + 1 ] ;

			opts.removeAt( i + 1 ) ;

			break ;
		}
	}

	if( !create && args.boolOptions.unlockInReadOnly ){

		opts.insert( opts.size() - 2,"--mountOption=-oro" ) ;
	}
}

engines::engine::ownsCipherFolder cryptomator::ownsCipherPath( const QString& cipherPath,
							       const QString& configPath ) const
{
	bool s = utility::pathExists( cipherPath + "/vault.cryptomator" ) ;

	return { s,cipherPath,configPath } ;
}

void cryptomator::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableIdleTime = false ;
	ee.enableCheckBox = false ;
	ee.enableKeyFile  = false ;
	ee.enableConfigFile = false ;

	e.ShowUI() ;
}

engines::engine::status cryptomator::passAllRequirenments( const engines::engine::cmdArgsList& opt ) const
{
	if( m_version_greater_or_equal_0_6_0 ){

		return engines::engine::passAllRequirenments( opt ) ;
	}else{
		return engine::engine::status::backEndFailedToMeetMinimumRequirenment ;
	}
}
