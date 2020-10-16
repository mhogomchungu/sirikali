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
	s.autorefreshOnMountUnMount   = false ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = false ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
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
	s.executableName        = "cryptomator-cli.jar" ;
	s.incorrectPasswordText = "InvalidPassphraseException" ;
	s.configFileArgument    = "%{unused}" ;
	s.keyFileArgument       = "" ;
	s.volumePropertiesCommands = QStringList{} ;
	s.windowsUnMountCommand    = QStringList{} ;
	s.configFileNames          = QStringList{ "masterkey.cryptomator" } ;
	s.fuseNames                = QStringList{ "fuse.cryptomator" } ;
	s.names                    = QStringList{ "cryptomator" } ;
	s.failedToMountList        = QStringList{ " ERROR " } ;
	s.successfulMountedList    = QStringList{ "Mounted to" } ;
	s.unMountCommand           = QStringList{ "SIGTERM" } ;
	s.notFoundCode             = engines::engine::status::cryptomatorNotFound ;
	s.versionInfo              = { {} } ;

	s.mountControlStructure    = "--vault %{cipherFolder} -fusemount %{mountPoint}" ;
	s.createControlStructure   = "" ;

	return s ;
}

cryptomator::cryptomator() :
	engines::engine( _setOptions() )
{
}

void cryptomator::updateOptions( QStringList& opts,
				 const engines::engine::cmdArgsList& e,
				 bool creating ) const
{
	Q_UNUSED( creating )

	auto vaultName = crypto::sha256( e.mountPoint ).mid( 0,16 ) ;

	for( int i = 0 ; i < opts.size() ; i++ ){

		auto& it = opts[ i ] ;

		if( it == e.cipherFolder ){

			it = vaultName + "=" + e.cipherFolder ;

		}else if( it == e.mountPoint ){

			it  = vaultName + "=" + e.mountPoint ;
		}
	}
}

engines::engine::args cryptomator::command( const QByteArray& password,
					    const engines::engine::cmdArgsList& args,
					    bool create ) const
{
	return custom::set_command( *this,password,args,create ) ;
}

engines::engine::status cryptomator::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::cryptomatorBadPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

void cryptomator::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableIdleTime = false ;
	ee.enableCheckBox = false ;
	ee.enableKeyFile  = false ;

	e.ShowUI() ;
}
