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

static void _parse_v1( custom::opts& s,const SirikaliJson& json )
{
	s.baseOpts.requiresPolkit              = false ;
	s.baseOpts.hasGUICreateOptions         = true ;
	s.baseOpts.customBackend               = true ;
	s.baseOpts.passwordFormat              = json.getByteArray( "passwordFormat" ) ;
	s.baseOpts.requiresAPassword           = json.getBool( "requiresAPassword" ) ;
	s.baseOpts.autoMountsOnCreate          = json.getBool( "autoMountsOnVolumeCreation" ) ;
	s.baseOpts.supportsMountPathsOnWindows = json.getBool( "supportsMountPointPaths" ) ;
	s.mountControlStructure                = json.getString( "mountControlStructure" ) ;
	s.createControlStructure               = json.getString( "createControlStructure" ) ;
	s.baseOpts.reverseString               = json.getString( "reverseString" ) ;
	s.baseOpts.idleString                  = json.getString( "idleString" ) ;
	s.baseOpts.executableName              = json.getString( "executableName" ) ;
	s.baseOpts.incorrectPasswordText       = json.getString( "wrongPasswordText" ) ;
	s.baseOpts.incorrectPassWordCode       = json.getString( "wrongPasswordErrorCode" ) ;
	s.baseOpts.unMountCommand              = json.getString( "unMountCommand" ) ;
	s.baseOpts.configFileArgument          = json.getString( "configFileArgument" ) ;
	s.baseOpts.windowsUnMountCommand       = json.getString( "windowsUnMountCommand" ) ;
	s.baseOpts.failedToMountList           = json.getStringList( "failedToMountTextList" ) ;
	s.baseOpts.successfulMountedList       = json.getStringList( "successfullyMountedList" ) ;
	s.baseOpts.configFileNames             = json.getStringList( "configFileNames" ) ;
	s.baseOpts.names                       = json.getStringList( "names" ) ;
	s.baseOpts.fuseNames                   = json.getStringList( "fuseNames" ) ;
	s.baseOpts.fileExtensions              = json.getStringList( "fileExtensions" ) ;
	s.baseOpts.volumePropertiesCommands    = json.getStringList( "volumePropertiesCommands" ) ;
	s.baseOpts.windowsInstallPathRegistryKey   = json.getString( "windowsInstallPathRegistryKey" ) ;
	s.baseOpts.windowsInstallPathRegistryValue = json.getString( "windowsInstallPathRegistryValue" ) ;
	s.baseOpts.hasConfigFile                   = s.baseOpts.configFileNames.size() > 0 ;
	s.baseOpts.notFoundCode                    = engines::engine::status::customCommandNotFound ;
}

static void _parse_v11( custom::opts& s,const SirikaliJson& json )
{
	s.baseOpts.backendRequireMountPath   = json.getBool( "backendRequireMountPath" ) ;
	s.baseOpts.autorefreshOnMountUnMount = json.getBool( "autorefreshOnMountUnMount" ) ;
	s.baseOpts.backendTimeout            = json.getInterger( "backendTimeout" ) ;
	s.baseOpts.takesTooLongToUnlock      = json.getBool( "takesTooLongToUnlock" ) ;
}

static QString _getVersion( const SirikaliJson& json )
{
	try{
		return json.getString( "versionNumber" ) ;

	}catch( ... ){

		return "1.0" ;
	}
}

static utility::result< custom::opts > _getOptions( const QByteArray& e,const QString& s )
{
	auto _log_error = []( const QString& msg,const QString& path ){

		auto a = "\nFailed to parse file for reading: " + path ;

		utility::debug::showDebugWindow( msg + a ) ;
	} ;

	try{
		custom::opts s ;

		SirikaliJson json( e,SirikaliJson::type::CONTENTS ) ;

		auto a = _getVersion( json ) ;

		if( a == "1.1" ){

			_parse_v1( s,json ) ;
			_parse_v11( s,json ) ;
		}else{
			_parse_v1( s,json ) ;
		}

		return s ;

	}catch( const SirikaliJson::exception& e ){

		_log_error( e.what(),s ) ;

	}catch( const std::exception& e ){

		_log_error( e.what(),s ) ;

	}catch( ... ){

		_log_error( "Unknown error has occured",s ) ;
	}

	return {} ;
}

static void _add_engines( const QString& path,
			  std::vector< std::unique_ptr< engines::engine >>& engines )
{
	const auto s = QDir( path ).entryList( QDir::Filter::Files ) ;

	QFile file ;

	for( const auto& it : s ){

		auto c = path + it ;
		file.setFileName( c ) ;

		if( file.open( QIODevice::ReadOnly ) ){

			auto s = _getOptions( file.readAll(),c ) ;

			if( s.has_value() ){

				const auto& m = s.value() ;

				if( m.baseOpts.names.size() > 0 && m.baseOpts.fuseNames.size() > 0 ){

					auto n = QString( it ).replace( ".json","" ) ;

					if( n.size() > 0 ){

						engines.emplace_back( std::make_unique< custom >( m ) ) ;
					}
				}else{
					auto a = "Name field/Fuse names not set in config file : " + path ;

					utility::debug::showDebugWindow( a ) ;
				}
			}

			file.close() ;
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

custom::custom( custom::opts s ) :
	engines::engine( std::move( s.baseOpts ) ),
	m_mountControlStructure( s.mountControlStructure ),
	m_createControlStructure( s.createControlStructure )
{
}

engines::engine::args custom::command( const QByteArray& password,
				       const engines::engine::cmdArgsList& args ) const
{
	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	if( args.create ){

		auto exeOptions = m.exeOptions() ;

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		exeOptions.add( args.opt.createOptions ) ;

		QString cmd = m_createControlStructure ;

		cmd.replace( "%{mountOptions}",exeOptions.get(),Qt::CaseInsensitive ) ;
		cmd.replace( "%{cipherFolder}",args.cipherFolder,Qt::CaseInsensitive ) ;
		cmd.replace( "%{mountPoint}",args.mountPoint,Qt::CaseInsensitive ) ;
		cmd.replace( "%{password}",password ) ;

		return { args,m,args.exe + " " + cmd } ;
	}else{
		QString cmd = m_mountControlStructure ;

		auto exeOptions = m.exeOptions() ;

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		if( args.opt.reverseMode ){

			exeOptions.add( this->reverseString() ) ;
		}

		if( !args.opt.idleTimeout.isEmpty() && !this->idleString().isEmpty() ){

			exeOptions.addPair( this->idleString(),args.opt.idleTimeout ) ;
		}

		cmd.replace( "%{mountOptions}",exeOptions.get(),Qt::CaseInsensitive ) ;
		cmd.replace( "%{cipherFolder}",args.cipherFolder,Qt::CaseInsensitive ) ;
		cmd.replace( "%{mountPoint}",args.mountPoint,Qt::CaseInsensitive ) ;
		cmd.replace( "%{fuseOpts}",m.fuseOpts().get(),Qt::CaseInsensitive ) ;
		cmd.replace( "%{password}",password ) ;

		return { args,m,args.exe + " " + cmd } ;
	}
}

engines::engine::status custom::errorCode( const QString& e,int s ) const
{
	const auto& m = this->incorrectPasswordCode() ;

	if( !m.isEmpty() ){

		bool ok ;

		auto n = m.toInt( &ok ) ;

		if( ok && n == s ){

			return engines::engine::status::customCommandBadPassword ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}else{
		const auto& s = this->incorrectPasswordText() ;

		if( !s.isEmpty() && e.contains( s ) ){

			return engines::engine::status::customCommandBadPassword ;

		}else if( e.contains( "cannot load WinFsp" ) ){

			return engines::engine::status::failedToLoadWinfsp ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}
}

const QString& custom::installedVersionString() const
{
	return m_version.get() ;
}

void custom::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	customcreateoptions::instance( parent,std::move( function ) ) ;
}
