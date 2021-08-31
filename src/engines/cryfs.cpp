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

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	auto a = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}" ;
	auto b = "InstallLocation" ;

	auto c = SiriKali::Windows::engineInstalledDir( a,b ) + "\\bin\\cryfs-unmount.exe" ;
	s.windowsInstallPathRegistryKey   = a ;
	s.windowsInstallPathRegistryValue = b ;
	s.windowsUnMountCommand           = QStringList{ c,"%{mountPoint}" } ;

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
	s.incorrectPasswordText = "Could not load config file. Did you enter the correct password?" ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.releaseURL            = "https://api.github.com/repos/cryfs/cryfs/releases" ;
	s.successfulMountedList = QStringList{ "Mounting filesystem." } ;
	s.configFileNames       = QStringList{ "cryfs.config",".cryfs.config" } ;
	s.fuseNames             = QStringList{ "fuse.cryfs" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "cryfs" } ;
	s.executableNames       = QStringList{ "cryfs" } ;

	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "--version",true,2,0 } } ;

	if( utility::platformIsWindows() ){

		s.createControlStructure = "-f %{createOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		s.mountControlStructure  = "-f %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}else{
		s.createControlStructure = "%{createOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		s.mountControlStructure  = "%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}

	return s ;
}

cryfs::cryfs() :
	engines::engine( _setOptions() ),
	m_env( this->setEnv() ),
	m_version_greater_or_equal_0_10_0( true,*this,0,10,0 ),
	m_version_greater_or_equal_0_9_9( true,*this,0,9,9 )
{
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
	return m_version_greater_or_equal_0_10_0 ;
}

const QProcessEnvironment& cryfs::getProcessEnvironment() const
{
	return m_env ;
}

engines::engine::status cryfs::errorCode( const QString& e,int s ) const
{
	if( m_version_greater_or_equal_0_9_9 ){

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
	}else{
		/*
		 * Falling back to parsing strings
		 */
		if( utility::containsAtleastOne( e,"Error 11:",this->incorrectPasswordText() ) ){

			return engines::engine::status::badPassword ;

		}else if( e.contains( "This filesystem is for CryFS" ) &&
			  e.contains( "It has to be migrated" ) ){

			return engines::engine::status::cryfsMigrateFileSystem ;

		}else if( e.contains( "The filesystem id in the config file is different to the last time we loaded a filesystem from this basedir" ) ){

			return engines::engine::status::cryfsReplaceFileSystem ;
		}
	}

	return engines::engine::status::backendFail ;
}

void cryfs::updateOptions( engines::engine::cmdArgsList& e,bool creating ) const
{
	Q_UNUSED( creating )

	if( m_version_greater_or_equal_0_10_0 && e.boolOptions.allowReplacedFileSystem ){

		e.mountOptions.append( "--allow-replaced-filesystem" ) ;
	}

	if( e.boolOptions.allowUpgradeFileSystem ){

		e.mountOptions.append( "--allow-filesystem-upgrade" ) ;
	}

	if( !m_version_greater_or_equal_0_10_0 ){

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

		if( !m_version_greater_or_equal_0_9_9 ){

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
	cryfscreateoptions::instance( *this,s,m_version_greater_or_equal_0_10_0 ) ;
}

void cryfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	if( m_version_greater_or_equal_0_10_0 ){

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
