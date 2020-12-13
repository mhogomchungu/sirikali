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
	s.releaseURL            = "https://api.github.com/repos/mhogomchungu/cli/releases" ;
	s.passwordFormat        = "" ;
	s.incorrectPasswordText = "InvalidPassphraseException" ;
	s.configFileArgument    = "" ;
	s.keyFileArgument       = "" ;
	s.executableNames          = QStringList{ "cryptomator-cli","cryptomator-cli.jar" } ;
	s.volumePropertiesCommands = QStringList{} ;
	s.windowsUnMountCommand    = QStringList{} ;
	s.configFileNames          = QStringList{ "masterkey.cryptomator" } ;
	s.fuseNames                = QStringList{ "fuse.cryptomator" } ;
	s.names                    = QStringList{ "cryptomator" } ;
	s.failedToMountList        = QStringList{ " ERROR ","Exception","fuse: unknown option" } ;
	s.successfulMountedList    = QStringList{ "Mounted to" } ;
	s.notFoundCode             = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo              = { { "--version",true,0,0 } } ;
	s.versionMinimum           = "0.4.5" ;

	s.mountControlStructure    = "--foreground --vault %{cipherFolder} --fusemount %{mountPoint} --mountFlags %{fuseOpts}" ;
	s.createControlStructure   = "" ;

	return s ;
}

cryptomator::cryptomator() :
	engines::engine( _setOptions() ),
	m_version_greater_or_equal_0_4_5( false,*this,this->minimumVersion() )
{
}

engines::engine::ownsCipherFolder cryptomator::ownsCipherPath( const QString& cipherPath,
							       const QString& configPath ) const
{
	bool s = utility::pathExists( cipherPath + "/masterkey.cryptomator" ) ;

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
	if( m_version_greater_or_equal_0_4_5 ){

		return engines::engine::passAllRequirenments( opt ) ;
	}else{
		return engine::engine::status::backEndFailedToMeetMinimumRequirenment ;
	}
}
