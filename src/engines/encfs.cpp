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

#include "encfs.h"

#include "encfscreateoptions.h"
#include "options.h"

#include "custom.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
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
	s.releaseURL            = "https://api.github.com/repos/vgough/encfs/releases" ;
	s.passwordFormat        = "%{password}\n%{password}" ;
	s.reverseString         = "--reverse" ;
	s.idleString            = "--idle=%{timeout}" ;
	s.incorrectPasswordText = "Error decoding volume key, password incorrect" ;
	s.configFileArgument    = "--config=%{configFilePath}" ;
	s.windowsInstallPathRegistryKey   = "SOFTWARE\\ENCFS" ;
	s.windowsInstallPathRegistryValue = "InstallDir" ;
	s.windowsUnMountCommand           = QStringList{ "taskkill","/F","/PID","%{PID}" } ;
	s.volumePropertiesCommands        = QStringList{ "encfsctl %{cipherFolder}" } ;
	s.configFileNames       = QStringList{ ".encfs6.xml","encfs6.xml",".encfs5",".encfs4" } ;
	s.fuseNames             = QStringList{ "fuse.encfs" } ;
	s.executableNames       = QStringList{ "encfs" } ;
	s.names                 = QStringList{ "encfs","encfsctl" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.successfulMountedList = QStringList{ "has been started" } ;
	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "--version",false,2,0 } } ;

	if( utility::platformIsWindows() ){

		s.autoCreatesMountPoint = true ;
		s.autoDeletesMountPoint = true ;

		s.mountControlStructure  = "-f --stdinpass %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		s.createControlStructure = "-f --stdinpass --standard %{createOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}else{
		s.autoCreatesMountPoint  = false ;
		s.autoDeletesMountPoint  = false ;

		s.mountControlStructure  = "--stdinpass %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		s.createControlStructure = "--stdinpass --standard %{createOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}

	return s ;
}

encfs::encfs() :
	engines::engine( _setOptions() ),
	m_environment( engines::engine::getProcessEnvironment() )
{
}

engines::engine::args encfs::command( const QByteArray& password,
				      const engines::engine::cmdArgsList& args,
				      bool create ) const
{
	m_environment.remove( "ENCFS6_CONFIG" ) ;

	if( !m_configPathThroughEnv.isEmpty() ){

		auto a = "Encfs: Setting Env Variable Of: ENCFS6_CONFIG=" ;
		utility::debug() << a + m_configPathThroughEnv ;
		m_environment.insert( "ENCFS6_CONFIG",m_configPathThroughEnv ) ;
	}

	return engines::engine::command( password,args,create ) ;
}

const QProcessEnvironment& encfs::getProcessEnvironment() const
{
	return m_environment ;
}

void encfs::updateOptions( engines::engine::cmdArgsList& args,bool creating ) const
{
	if( creating && args.boolOptions.unlockInReverseMode ){

		args.createOptions.append( this->reverseString() ) ;
	}

	m_configPathThroughEnv = args.configFilePath ;

	args.configFilePath.clear() ;
}

void encfs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	encfscreateoptions::instance( s ) ;
}

void encfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableKeyFile = false ;

	ee.checkBoxChecked = s.mOpts.opts.unlockInReverseMode ;

	ee.updateOptions = []( const ::options::Options& s ){

		engines::engine::booleanOptions e ;

		e.unlockInReverseMode = s.checkBoxChecked ;

		return e ;
	} ;

	e.ShowUI() ;
}
