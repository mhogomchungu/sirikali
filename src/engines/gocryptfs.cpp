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

#include "gocryptfs.h"
#include "custom.h"

#include "gocryptfscreateoptions.h"
#include "options.h"

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
	s.acceptsSubType        = false ;
	s.acceptsVolName        = false ;
	s.releaseURL             = "https://api.github.com/repos/rfjakob/gocryptfs/releases" ;
	s.passwordFormat         = "%{password}" ;
	s.volumePropertiesCommands = QStringList{ "gocryptfs -info %{cipherFolder}",
						  "gocryptfs -info %{plainFolder}" } ;
	s.reverseString         = "-reverse" ;
	s.idleString            = "-idle %{timeout}" ;
	s.incorrectPasswordText = "Password incorrect." ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.executableNames       = QStringList{ "gocryptfs" } ;
	s.configFileNames       = QStringList{ "gocryptfs.conf",
					       ".gocryptfs.conf",
					       ".gocryptfs.reverse.conf",
					       "gocryptfs.reverse.conf" } ;
	s.fuseNames             = QStringList{ "fuse.gocryptfs","fuse.gocryptfs-reverse" } ;
	s.names                 = QStringList{ "gocryptfs","gocryptfs.reverse","gocryptfs-reverse" } ;
	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "--version",true,1,0 } } ;

	s.createControlStructure = "-q --init %{createOptions} %{cipherFolder}" ;
	s.mountControlStructure  = "-q %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;

	return s ;
}

gocryptfs::gocryptfs() : engines::engine( _setOptions() ),
	m_version_has_error_codes( true,*this,1,2,1 )
{
}

template< typename Function >
static bool _set_if_found( const Function& function )
{
	std::array< QString,3 > m = { "gocryptfs.reverse.conf",
				      ".gocryptfs.reverse.conf",
				      "gocryptfs.reverse" } ;
	for( const auto& it : m ){

		if( function( it ) ){

			return true ;
		}
	}

	return false ;
}

void gocryptfs::updateOptions( engines::engine::cmdArgsList& opt,bool creating ) const
{
	if( creating ){

		if( opt.boolOptions.unlockInReverseMode ){

			opt.createOptions.append( this->reverseString() ) ;
		}
	}else{
		opt.boolOptions.unlockInReverseMode = [ & ](){

			if( opt.configFilePath.isEmpty() ){

				return _set_if_found( [ & ]( const QString& e ){

					return utility::pathExists( opt.cipherFolder + "/" + e ) ;
				} ) ;
			}else{
				return _set_if_found( [ & ]( const QString& e ){

					return opt.configFilePath.endsWith( e ) ;
				} ) ;
			}
		}() ;
	}
}

engines::engine::status gocryptfs::errorCode( const QString& e,int s ) const
{
	if( m_version_has_error_codes ){

		/*
		 * This error code was added in gocryptfs 1.2.1
		 */

		if( s == 12 ){

			return engines::engine::status::badPassword ;
		}
	}

	return engines::engine::errorCode( e,s ) ;
}

void gocryptfs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	gocryptfscreateoptions::instance( *this,s ) ;
}

void gocryptfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
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
