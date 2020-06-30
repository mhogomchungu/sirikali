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

static void _parse( custom::opts& s,const SirikaliJson& json )
{
	s.baseOpts.requiresPolkit                  = false ;
	s.baseOpts.hasGUICreateOptions             = true ;
	s.baseOpts.customBackend                   = true ;

	s.baseOpts.setsCipherPath                  = json.getBool( "setsCipherPath",true ) ;
	s.baseOpts.backendRunsInBackGround         = json.getBool( "runsInBackGround",true ) ;
	s.baseOpts.backendRequireMountPath         = json.getBool( "backendRequireMountPath",true ) ;
	s.baseOpts.autorefreshOnMountUnMount       = json.getBool( "autorefreshOnMountUnMount",true ) ;
	s.baseOpts.takesTooLongToUnlock            = json.getBool( "takesTooLongToUnlock",false ) ;
	s.baseOpts.requiresAPassword               = json.getBool( "requiresAPassword",true ) ;
	s.baseOpts.autoMountsOnCreate              = json.getBool( "autoMountsOnVolumeCreation",true ) ;
	s.baseOpts.supportsMountPathsOnWindows     = json.getBool( "supportsMountPointPaths",false ) ;
	s.baseOpts.acceptsSubType                  = json.getBool( "acceptsSubType",true ) ;
	s.baseOpts.acceptsVolName                  = json.getBool( "acceptsVolName",true ) ;

	s.baseOpts.backendTimeout                  = json.getInterger( "backendTimeout",0 ) ;

	s.baseOpts.passwordFormat                  = json.getByteArray( "passwordFormat","%{password}" ) ;

	s.mountControlStructure                    = json.getString( "mountControlStructure","%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ) ;
	s.createControlStructure                   = json.getString( "createControlStructure","%{createOptions} %{cipherFolder} %{mountPoint}" ) ;
	s.baseOpts.reverseString                   = json.getString( "reverseString" ) ;
	s.baseOpts.idleString                      = json.getString( "idleString" ) ;
	s.baseOpts.executableName                  = json.getString( "executableName" ) ;
	s.baseOpts.incorrectPasswordText           = json.getString( "wrongPasswordText" ) ;
	s.baseOpts.incorrectPassWordCode           = json.getString( "wrongPasswordErrorCode" ) ;
	s.baseOpts.configFileArgument              = json.getString( "configFileArgument" ) ;
	s.baseOpts.windowsInstallPathRegistryKey   = json.getString( "windowsInstallPathRegistryKey" ) ;
	s.baseOpts.windowsInstallPathRegistryValue = json.getString( "windowsInstallPathRegistryValue" ) ;

	s.baseOpts.windowsUnMountCommand           = json.getStringList( "windowsUnMountCommand" ) ;
	s.baseOpts.unMountCommand                  = json.getStringList( "unMountCommand" ) ;
	s.baseOpts.failedToMountList               = json.getStringList( "failedToMountTextList" ) ;
	s.baseOpts.successfulMountedList           = json.getStringList( "successfullyMountedList" ) ;
	s.baseOpts.configFileNames                 = json.getStringList( "configFileNames" ) ;
	s.baseOpts.names                           = json.getStringList( "names" ) ;
	s.baseOpts.fuseNames                       = json.getStringList( "fuseNames" ) ;
	s.baseOpts.fileExtensions                  = json.getStringList( "fileExtensions" ) ;
	s.baseOpts.volumePropertiesCommands        = json.getStringList( "volumePropertiesCommands" ) ;

	s.baseOpts.hasConfigFile                   = s.baseOpts.configFileNames.size() > 0 ;

	s.baseOpts.notFoundCode                    = engines::engine::status::customCommandNotFound ;
}

static utility2::result< custom::opts > _getOptions( const QByteArray& e,const QString& s )
{
	auto _log_error = []( const QString& msg,const QString& path ){

		auto a = "\nFailed to parse file for reading: " + path ;

		utility::debug::logErrorWhileStarting( msg + a ) ;
	} ;

	try{
		custom::opts s ;

		SirikaliJson json( e,
				   SirikaliJson::type::CONTENTS,
				   []( const QString& e ){ utility::debug() << e ; } ) ;

		_parse( s,json ) ;

		return s ;

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

					utility::debug::logErrorWhileStarting( a ) ;
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

template< typename T >
static QString _replace( QString a,const T& opts )
{
	for( const auto& it : opts ){

		a.replace( it.first,it.second ) ;
	}

	return a ;
}

static void _resolve( QStringList& orgs,
		      const QString& name,
		      const QString& controlStructure,
		      const std::vector< std::pair< QString,QString > >& opts )
{
	if( controlStructure.isEmpty() ){

		return ;
	}

	auto m = utility::split( controlStructure,' ' ) ;

	if( m.size() == 1 ){

		orgs.append( _replace( m.at( 0 ),opts ) ) ;

	}else if( m.size() == 2 ){

		orgs.append( m.at( 0 ) ) ;
		orgs.append( _replace( m.at( 1 ),opts ) ) ;
	}else{
		auto s = QString( "Wrong control structure detected in custom backend named \"%1\"." ) ;
		utility::debug::logErrorWhileStarting( s.arg( name ) ) ;
	}
}

QStringList custom::resolve( const resolveStruct& r ) const
{
	auto mm = utility::split( r.controlStructure,' ' ) ;

	for( auto& it : mm ){

		if( it == "%{cipherFolder}" ){

			it = r.args.cipherFolder ;

		}else if( it == "%{mountPoint}" ){

			it = r.args.mountPoint ;

		}else if( it == "%{password}" ){

			it = r.password ;
		}
	}

	for( int i = 0 ; i < mm.size() ; i++ ){

		auto& it = mm[ i ] ;

		if( it == "%{fuseOpts}" ){

			mm.removeAt( i ) ;

			if( !r.fuseOpts.isEmpty() ){

				mm.insert( i,r.fuseOpts.join( ',' ) ) ;
				mm.insert( i,"-o" ) ;
			}

			break ;
		}
	}

	for( int i = 0 ; i < mm.size() ; i++ ){

		const auto& it = mm[ i ] ;

		if( it == "%{mountOptions}" ){

			mm.removeAt( i ) ;

			QStringList opts ;

			if( r.args.boolOptions.unlockInReverseMode ){

				opts.append( this->reverseString() ) ;
			}

			std::vector< std::pair< QString,QString > > oo ;

			oo.emplace_back( std::make_pair( "%{cipherFolder}",r.args.cipherFolder ) ) ;
			oo.emplace_back( std::make_pair( "%{configFileName}",this->configFileName() ) ) ;
			oo.emplace_back( std::make_pair( "%{configFilePath}",r.args.configFilePath ) ) ;

			_resolve( opts,this->name(),this->configFileArgument(),oo ) ;

			if( !r.args.idleTimeout.isEmpty() ){

				_resolve( opts,
					  this->name(),
					  this->idleString(),
					  { std::make_pair( "%{timeout}",r.args.idleTimeout ) } ) ;
			}

			opts.append( r.createOpts ) ;

			if( !opts.isEmpty() ){

				for( int j = opts.size() - 1 ; j >= 0 ; j-- ){

					mm.insert( i,opts.at( j ) ) ;
				}
			}

			break ;
		}
	}

	return mm ;
}

engines::engine::args custom::command( const QByteArray& password,
				       const engines::engine::cmdArgsList& args,
				       bool create ) const
{
	engines::engine::commandOptions m( *this,args ) ;

	if( create ){

		QStringList opts ;

		if( !args.createOptions.isEmpty() ){

			opts = utility::split( args.createOptions,' ' ) ;
		}

		auto exeOptions = m.exeOptions() ;

		auto s = this->resolve( { m_createControlStructure,args,password,opts,m.fuseOpts().get() } ) ;

		exeOptions.add( s ) ;

		return { args,m,this->executableFullPath(),exeOptions.get() } ;

	}else{
		auto exeOptions = m.exeOptions() ;

		auto s = this->resolve( { m_mountControlStructure,args,password,{},m.fuseOpts().get() } ) ;

		exeOptions.add( s ) ;

		return { args,m,this->executableFullPath(),exeOptions.get() } ;
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

void custom::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	customcreateoptions::instance( s ) ;
}
