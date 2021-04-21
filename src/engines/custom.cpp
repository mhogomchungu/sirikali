/*
 *
 *  Copyright (c) 2019
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

#include "custom.h"
#include "../json_parser.hpp"
#include "install_prefix.h"
#include "../settings.h"

#include "customcreateoptions.h"

#include <QDir>

static void _parse( engines::engine::BaseOptions& s,const SirikaliJson& json )
{
	s.requiresPolkit                  = false ;
	s.hasGUICreateOptions             = true ;
	s.customBackend                   = true ;

	s.setsCipherPath                  = json.getBool( "setsCipherPath",true ) ;
	s.backendRunsInBackGround         = json.getBool( "runsInBackGround",true ) ;
	s.backendRequireMountPath         = json.getBool( "backendRequireMountPath",true ) ;
	s.autorefreshOnMountUnMount       = json.getBool( "autorefreshOnMountUnMount",true ) ;
	s.takesTooLongToUnlock            = json.getBool( "takesTooLongToUnlock",false ) ;
	s.requiresAPassword               = json.getBool( "requiresAPassword",true ) ;
	s.autoMountsOnCreate              = json.getBool( "autoMountsOnVolumeCreation",true ) ;
	s.supportsMountPathsOnWindows     = json.getBool( "windowsSupportsMountPointPaths",false ) ;
	s.acceptsSubType                  = json.getBool( "acceptsSubType",true ) ;
	s.acceptsVolName                  = json.getBool( "acceptsVolName",true ) ;
	s.likeSsh                         = json.getBool( "likeSsh",false ) ;
	s.autoCreatesMountPoint           = json.getBool( "autoCreatesMountPoint",false ) ;
	s.autoDeletesMountPoint           = json.getBool( "autoDeletesMountPoint",false ) ;
	s.usesOnlyMountPoint              = json.getBool( "usesOnlyMountPoint",false ) ;
	s.usesFuseArgumentSwitch          = json.getBool( "usesFuseArgumentSwitch",true ) ;

	s.backendTimeout                  = json.getInterger( "backendTimeout",0 ) ;

	s.passwordFormat                  = json.getByteArray( "passwordFormat","%{password}" ) ;

	s.defaultFavoritesMountOptions    = json.getString( "defaultFavoritesMountOptions" ) ;
	s.mountControlStructure           = json.getString( "mountControlStructure","%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ) ;
	s.createControlStructure          = json.getString( "createControlStructure" ) ;
	s.reverseString                   = json.getString( "reverseString" ) ;
	s.idleString                      = json.getString( "idleString" ) ;
	s.incorrectPasswordText           = json.getString( "wrongPasswordText" ) ;
	s.incorrectPassWordCode           = json.getString( "wrongPasswordErrorCode" ) ;
	s.configFileArgument              = json.getString( "configFileArgument" ) ;
	s.keyFileArgument                 = json.getString( "keyFileArgument" ) ;
	s.windowsInstallPathRegistryKey   = json.getString( "windowsInstallPathRegistryKey" ) ;
	s.windowsInstallPathRegistryValue = json.getString( "windowsInstallPathRegistryValue" ) ;
	s.windowsExecutableFolderPath     = json.getString( "windowsExecutableFolderPath" ) ;
	s.displayName                     = json.getString( "displayName" ) ;
	s.versionMinimum                  = json.getString( "versionMinimum" ) ;
	s.sirikaliMinimumVersion          = json.getString( "sirikaliMinimumVersion" ) ;

	s.executableNames                 = QStringList{ json.getString( "executableName" ) } ;

	s.windowsUnMountCommand           = json.getStringList( "windowsUnMountCommand" ) ;
	s.unMountCommand                  = json.getStringList( "unMountCommand" ) ;
	s.failedToMountList               = json.getStringList( "stringsToCheckForFailedMount" ) ;
	s.successfulMountedList           = json.getStringList( "stringsToCheckForSuccessfulMount" ) ;
	s.configFileNames                 = json.getStringList( "configFileNames" ) ;
	s.names                           = json.getStringList( "names" ) ;
	s.fuseNames                       = json.getStringList( "fuseNames" ) ;
	s.fileExtensions                  = json.getStringList( "fileExtensions" ) ;
	s.volumePropertiesCommands        = json.getStringList( "volumePropertiesCommands" ) ;
	s.hasConfigFile                   = s.configFileNames.size() > 0 ;

	s.notFoundCode                    = engines::engine::status::engineExecutableNotFound ;

	auto versionArgumentString        = json.getString( "versionArgumentString" ) ;
	auto versionOutputStdOut          = json.getBool( "versionOutputStdOut",true ) ;
	auto versionStringTextPosition    = json.getIntVector( "versionStringTextPosition" ) ;

	if( !versionArgumentString.isEmpty() && versionStringTextPosition.size() > 1 ){

		engines::engine::BaseOptions::vInfo ss ;

		ss.versionArgument = std::move( versionArgumentString ) ;
		ss.readFromStdOut  = versionOutputStdOut ;
		ss.argumentLine    = versionStringTextPosition[ 0 ] ;
		ss.argumentNumber  = versionStringTextPosition[ 1 ] ;

		s.versionInfo.emplace_back( std::move( ss ) ) ;
	}
}

static utility2::result< engines::engine::BaseOptions > _getOptions( QFile& f )
{
	engines::engine::BaseOptions s ;

	utility::logger logger ;

	SirikaliJson json( f,logger.function() ) ;

	if( json.passed() ){

		_parse( s,json ) ;

		return s ;
	}else{
		return {} ;
	}
}

static void _add_engines( const QString& path,
			  std::vector< std::unique_ptr< engines::engine > >& engines )
{
	QFile file ;

	for( const auto& it : QDir( path ).entryList( QDir::Filter::Files ) ){

		auto c = path + it ;
		file.setFileName( c ) ;

		if( file.open( QIODevice::ReadOnly ) ){

			auto s = _getOptions( file ) ;

			if( s.has_value() ){

				const auto& m = s.value() ;

				if( m.names.size() > 0 && m.fuseNames.size() > 0 ){

					auto n = QString( it ).replace( ".json","" ) ;

					if( n.size() > 0 ){

						engines.emplace_back( std::make_unique< custom >( m ) ) ;
					}
				}else{
					auto a = "Name field/Fuse names not set in config file : " + path ;

					utility::debug::logErrorWhileStarting( a ) ;
				}
			}

			file.close() ;
		}else{
			utility::debug::logErrorWhileStarting( QString( "Warning, Failed To Open File For Reading: %1" ).arg( path ) ) ;
		}
	}
}

void custom::addEngines( std::vector< std::unique_ptr< engines::engine >>& engines )
{
	auto m = settings::instance().ConfigLocation() ;

	if( !m.isEmpty() ){

		_add_engines( m + "/backends/",engines ) ;
	}

	if( utility::platformIsWindows() ){

		_add_engines( QDir().currentPath() + "/backends/",engines ) ;
	}else{
		_add_engines( INSTALL_PREFIX"/share/SiriKali/backends/",engines ) ;
	}
}

custom::custom( engines::engine::BaseOptions baseOpts ) :
	engines::engine( std::move( baseOpts ) )
{
}

engines::engine::status custom::errorCode( const QString& e,int s ) const
{
	const auto& m = this->incorrectPasswordCode() ;

	if( !m.isEmpty() ){

		bool ok ;

		auto n = m.toInt( &ok ) ;

		if( ok && n == s ){

			return engines::engine::status::badPassword ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}else{
		const auto& s = this->incorrectPasswordText() ;

		if( !s.isEmpty() && e.contains( s ) ){

			return engines::engine::status::badPassword ;

		}else if( e.contains( "cannot load WinFsp" ) ){

			return engines::engine::status::failedToLoadWinfsp ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}
}

void custom::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	customcreateoptions::instance( s ) ;
}
