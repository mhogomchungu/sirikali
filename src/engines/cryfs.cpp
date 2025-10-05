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

#include "cryfs.h"
#include "cryfscreateoptions.h"
#include "../win.h"
#include "options.h"
#include "custom.h"

#include "../settings.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
	s.usesFuseArgumentSwitch      = true ;
	s.likeSsh               = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = true ;
	s.passwordFormat        = "%{password}" ;
	s.idleString            = "--unmount-idle %{timeout}" ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.releaseURL            = "https://api.github.com/repos/cryfs/cryfs/releases" ;
	s.incorrectPasswordText = QStringList{ "Did you enter the correct password?" } ;
	s.successfulMountedList = QStringList{ "Mounting filesystem." } ;
	s.configFileNames       = QStringList{ "cryfs.config",".cryfs.config" } ;
	s.fuseNames             = QStringList{ "fuse.cryfs" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "cryfs" } ;
	s.executableNames       = QStringList{ "cryfs" } ;

	if( utility::platformIsWindows() ){

		auto a = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}" ;
		auto b = "InstallLocation" ;

		auto c = SiriKali::Windows::engineInstalledDir( a,b ) + "\\bin\\cryfs-unmount.exe" ;

		c = utility::cleanPath( c ) ;

		s.windowsInstallPathRegistryKey   = a ;
		s.windowsInstallPathRegistryValue = b ;
		s.windowsUnMountCommand           = QStringList{ c,"%{mountPoint}" } ;

	}else if( utility::platformIsFlatPak() ){

		auto exe = "flatpak-spawn" ;

		auto m = engines::executableNotEngineFullPath( "cryfs-unmount" ) ;

		if( !m.isEmpty() ){

			s.unMountCommand = QStringList{ exe,"--host",m,"--immediate","%{mountPoint}" } ;
		}

	}else if( utility::platformIsOSX() ){

		s.unMountCommand = QStringList{ "umount","%{mountPoint}" } ;
	}else{
		auto m = engines::executableNotEngineFullPath( "cryfs-unmount" ) ;

		if( !m.isEmpty() ){

			s.unMountCommand = QStringList{ m,"--immediate","%{mountPoint}" } ;
		}
	}

	s.notFoundCode = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo = { { "--version",true,2,0 } } ;

	s.createControlStructure = "%{createOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	s.mountControlStructure  = "%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;

	return s ;
}

cryfs::cryfs() : engines::engine( _setOptions() ),m_env( this->setEnv() )
{
}

bool cryfs::updatable( bool ) const
{
	return false ;
}

void cryfs::setUpBinary( bool add,QStringList& apps,const QString& basePath ) const
{
	engines::engine::setUpBinary( add,apps,basePath,"cryfs" ) ;
	engines::engine::setUpBinary( false,apps,basePath,"cryfs-unmount" ) ;
}

bool cryfs::onlineArchiveFileName( const QString& ) const
{
	return false ;
}

engines::engine::args cryfs::command( const QByteArray& password,
				      const cmdArgsList& args,
				      bool create ) const
{
	auto m = engines::engine::command( password,args,create ) ;

	if( utility::platformIsFlatPak() ){		

		m.cmd_args.prepend( "-f" ) ;
		m.cmd_args.prepend( m.cmd ) ;
		m.cmd_args.prepend( "--host" ) ;

		m.cmd_args.prepend( "--env=CRYFS_FRONTEND=noninteractive" ) ;

		m.cmd = "flatpak-spawn" ;

	}else if( utility::platformIsWindows() && !this->installedVersionGreaterOrEqual( "1.0.0" ) ){

		m.cmd_args.prepend( "-f" ) ;
	}

	return m ;
}

QProcessEnvironment cryfs::setEnv() const
{
	auto s = engines::engine::getProcessEnvironment() ;

	s.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
	s.insert( "CRYFS_FRONTEND","noninteractive" ) ;

	return s ;
}

bool cryfs::takesTooLongToUnlock() const
{
	return this->installedVersionGreaterOrEqual( "0.10.0" ) ;
}

engines::engine::cmdStatus cryfs::commandStatus( const engines::engine::commandStatusOpts& e ) const
{
	if( utility::platformIsWindows() || utility::platformIsFlatPak() ){

		return engines::engine::commandStatus( e ) ;
	}else{
		const auto& engine = *this ;

		auto n = engine.errorCode( e.stdOut(),e.stdError(),e.exitCode() ) ;

		if( e.success() ){

			if( n == engines::engine::status::backendCrashed ){

				return { n,engine,e.stdError() } ;
			}else{
				return { engines::engine::status::success,engine } ;
			}
		}else{
			return { n,engine,e.stdOut() } ;
		}
	}
}

const QProcessEnvironment& cryfs::getProcessEnvironment() const
{
	return m_env ;
}

static bool _wrong_password( const QString& e,const char * s,const QStringList& m )
{
	for( const auto& a : m ){

		if( utility::containsAtleastOne( e,s,a ) ){

			return true ;
		}
	}

	return false ;
}

engines::engine::status cryfs::errorCode( const QString& e,const QString& err,int s ) const
{
	if( s == 0 ){

		if( err.contains( "[error]" ) ){

			return engines::engine::status::backendCrashed ;
		}
	}

	if( this->installedVersionGreaterOrEqual( "0.9.9" ) ){

		/*
		 * Error codes are here: https://github.com/cryfs/cryfs/blob/develop/src/cryfs/ErrorCodes.h
		 *
		 * Valid for cryfs > 0.9.8
		 */

		if( s == 11 ){

			return engines::engine::status::badPassword ;

		}else if( s == 14 ){

			return engines::engine::status::cryfsMigrateFileSystem ;

		}else if( s == 20 ){

			return engines::engine::status::cryfsReplaceFileSystem ;
		}
	}

	/*
	 * Falling back to parsing strings
	 */
	if( _wrong_password( e,"Error 11:",this->incorrectPasswordText() ) ){

		return engines::engine::status::badPassword ;

	}else if( e.contains( "This filesystem is for CryFS" ) &&
		  e.contains( "It has to be migrated" ) ){

		return engines::engine::status::cryfsMigrateFileSystem ;

	}else if( e.contains( "The filesystem id in the config file is different to the last time we loaded a filesystem from this basedir" ) ){

		return engines::engine::status::cryfsReplaceFileSystem ;
	}

	return engines::engine::status::backendFail ;
}

void cryfs::updateOptions( engines::engine::cmdArgsList& e,bool creating ) const
{
	Q_UNUSED( creating )

	if( this->installedVersionGreaterOrEqual( "0.10.0" ) && e.boolOptions.allowReplacedFileSystem ){

		e.mountOptions.append( "--allow-replaced-filesystem" ) ;
	}

	if( e.boolOptions.allowUpgradeFileSystem ){

		e.mountOptions.append( "--allow-filesystem-upgrade" ) ;
	}

	if( !this->installedVersionGreaterOrEqual( "0.10.0" ) ){

		e.fuseOptionsSeparator = "--" ;
	}
}

engines::engine::status cryfs::passAllRequirenments( const engines::engine::cmdArgsList& opt ) const
{
	auto s = engines::engine::passAllRequirenments( opt ) ;

	if( s != engines::engine::status::success ){

		return s ;
	}

	if( opt.boolOptions.allowUpgradeFileSystem ){

		if( !this->installedVersionGreaterOrEqual( "0.9.9" ) ){

			return engines::engine::status::cryfsVersionTooOldToMigrateVolume ;
		}
	}

	if( utility::platformIsWindows() ){

		/*
		 * Not sure how to work around this[1] bug report but it
		 * should be handled here.
		 *
		 * [1] https://github.com/cryfs/cryfs/issues/319
		 */
	}

	return s ;
}

void cryfs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	cryfscreateoptions::instance( *this,s,this->installedVersionGreaterOrEqual( "0.10.0" ) ) ;
}

void cryfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	if( this->installedVersionGreaterOrEqual( "0.10.0" ) ){

		ee.checkBoxChecked = s.mOpts.opts.allowReplacedFileSystem ;
	}else{
		ee.checkBoxChecked = false ;
	}

	ee.checkBoxText    = QObject::tr( "Allow Replaced File System" ) ;
	ee.enableCheckBox  = true ;
	ee.enableKeyFile   = false ;

	ee.updateOptions = []( const ::options::Options& s ){

		engines::engine::booleanOptions e ;

		e.allowReplacedFileSystem = s.checkBoxChecked ;

		return e ;
	} ;

	e.ShowUI() ;
}
