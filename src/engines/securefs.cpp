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

#include "securefs.h"

#include "securefscreateoptions.h"

#include "options.h"

#include "custom.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = false ;
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
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = true ;
	s.releaseURL            = "https://api.github.com/repos/netheril96/securefs/releases" ;
	s.passwordFormat        = "%{password}\n%{password}" ;
	s.incorrectPasswordText = "Invalid password" ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.keyFileArgument       = "--keyfile %{keyfile}" ;
	s.volumePropertiesCommands = QStringList{ "securefs info %{cipherFolder}" } ;
	s.windowsUnMountCommand = QStringList{ "sirikali.exe", "-T","%{PID}" } ;
	s.configFileNames       = QStringList{ ".securefs.json","securefs.json" } ;
	s.fuseNames             = QStringList{ "fuse.securefs" } ;
	s.names                 = QStringList{ "securefs" } ;
	s.failedToMountList     = QStringList{ "Error","init" } ;
	s.successfulMountedList = QStringList{ "has been started","init" } ;
	s.executableNames       = QStringList{ "securefs" } ;

	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "version",true,1,0 } } ;

	if( utility::platformIsWindows() ){

		s.mountControlStructure = "mount %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}else{
		s.mountControlStructure = "mount -b %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}

	s.createControlStructure = "create %{createOptions} %{cipherFolder}" ;

	return s ;
}

securefs::securefs() :
	engines::engine( _setOptions() ),
	m_version_greater_or_equal_0_11_1( true,*this,0,11,1 )
{
}

static void _updateKeys( QStringList& opts,engines::engine::cmdArgsList& e )
{
	if( !e.keyFile.isEmpty() ){

		if( !e.key.isEmpty() ){

			opts.append( "--askpass" ) ;
		}
	}
}

void securefs::updateOptions( engines::engine::cmdArgsList& e,bool creating ) const
{
	if( m_version_greater_or_equal_0_11_1 ){

		if( creating ){

			_updateKeys( e.createOptions,e ) ;
		}else{
			_updateKeys( e.mountOptions,e ) ;
		}
	}
}

void securefs::updateOptions( engines::engine::commandOptions& opts,
			      const engines::engine::cmdArgsList& args,
			      bool creating ) const
{
	Q_UNUSED( args )

	if( !creating && m_version_greater_or_equal_0_11_1 ){

		auto fuseOpts = opts.fuseOpts() ;
		auto exeOpts  = opts.exeOptions() ;

		auto fsname = fuseOpts.extractStartsWith( "fsname" ) ;

		auto fssubtype = fuseOpts.extractStartsWith( "subtype" ) ;

		if( fsname ){

			exeOpts.add( "--fsname",fsname.RValue().mid( 7 ) ) ;
		}

		if( fssubtype ){

			exeOpts.add( "--fssubtype",fssubtype.RValue().mid( 8 ) ) ;
		}
	}
}

bool securefs::requiresAPassword( const engines::engine::cmdArgsList& opt ) const
{
	if( opt.mountOptions.contains( "--keyfile" ) || !opt.keyFile.isEmpty() ){

		return false ;
	}else{
		return engines::engine::requiresAPassword( opt ) ;
	}
}

void securefs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	securefscreateoptions::instance( *this,s,m_version_greater_or_equal_0_11_1 ) ;
}

void securefs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableIdleTime = false ;
	ee.enableCheckBox = false ;
	ee.enableKeyFile  = m_version_greater_or_equal_0_11_1 ;

	e.ShowUI() ;
}
