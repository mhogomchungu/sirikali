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

	s.backendTimeout                  = json.getInterger( "backendTimeout",0 ) ;

	s.passwordFormat                  = json.getByteArray( "passwordFormat","%{password}" ) ;

	s.sshOptions                      = json.getString( "sshOptions" ) ;
	s.mountControlStructure           = json.getString( "mountControlStructure","%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ) ;
	s.createControlStructure          = json.getString( "createControlStructure" ) ;
	s.reverseString                   = json.getString( "reverseString" ) ;
	s.idleString                      = json.getString( "idleString" ) ;
	s.executableName                  = json.getString( "executableName" ) ;
	s.incorrectPasswordText           = json.getString( "wrongPasswordText" ) ;
	s.incorrectPassWordCode           = json.getString( "wrongPasswordErrorCode" ) ;
	s.configFileArgument              = json.getString( "configFileArgument" ) ;
	s.keyFileArgument                 = json.getString( "keyFileArgument" ) ;
	s.windowsInstallPathRegistryKey   = json.getString( "windowsInstallPathRegistryKey" ) ;
	s.windowsInstallPathRegistryValue = json.getString( "windowsInstallPathRegistryValue" ) ;
	s.windowsExecutableFolderPath     = json.getString( "windowsExecutableFolderPath" ) ;

	s.windowsUnMountCommand           = json.getStringList( "windowsUnMountCommand" ) ;
	s.unMountCommand                  = json.getStringList( "unMountCommand" ) ;
	s.failedToMountList               = json.getStringList( "failedToMountTextList" ) ;
	s.successfulMountedList           = json.getStringList( "windowsSuccessfullyMountedList" ) ;
	s.configFileNames                 = json.getStringList( "configFileNames" ) ;
	s.names                           = json.getStringList( "names" ) ;
	s.fuseNames                       = json.getStringList( "fuseNames" ) ;
	s.fileExtensions                  = json.getStringList( "fileExtensions" ) ;
	s.volumePropertiesCommands        = json.getStringList( "volumePropertiesCommands" ) ;

	s.hasConfigFile                   = s.configFileNames.size() > 0 ;

	s.notFoundCode                    = engines::engine::status::customCommandNotFound ;
}

static utility2::result< engines::engine::BaseOptions > _getOptions( QFile& f )
{
	engines::engine::BaseOptions s ;

	SirikaliJson json( f,utility::jsonLogger() ) ;

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

struct resolveStruct{
	const engines::engine& engine ;
	const QString& controlStructure ;
	const engines::engine::cmdArgsList& args ;
	const QByteArray& password ;
	const QStringList& opts ;
	const QStringList& fuseOpts ;
} ;

class resolve{
public:
	struct args{
		const char * first ;
		const QString& second ;
	} ;
	template< typename ... T >
	resolve( const T& ... e )
	{
		this->set( e ... ) ;
	}
	QString option( QString a ) const
	{
		for( const auto& it : m_opts ){

			if( !it.second.isEmpty() ){

				a.replace( it.first,it.second ) ;
			}
		}

		for( const auto& it : m_opts ){

			if( a.contains( it.first ) ) {

				return {} ;
			}
		}

		return a ;
	}
private:
	template< typename T >
	void set( const T& t )
	{
		m_opts.emplace_back( t ) ;
	}
	template< typename E,typename ... T >
	void set( const E& e,const T& ... t )
	{
		this->set( e ) ;
		this->set( t ... ) ;
	}
	std::vector< resolve::args > m_opts ;
};

class replace{
public:
	replace( QStringList& s,int position ) :
		m_stringList( s ),m_position( position )
	{
	}
	void set( const QString& e )
	{
		m_stringList.insert( m_position,e ) ;
	}
	void set( const QStringList& e )
	{
		for( int i = e.size() - 1 ; i >= 0 ; i-- ){

			m_stringList.insert( m_position,e.at( i ) ) ;
		}
	}
private:
	QStringList& m_stringList ;
	int m_position ;
};

template< typename ... T >
static void _resolve( QStringList& orgs,
		      const QString& name,
		      const QString& controlStructure,
		      const T& ... rrr )
{	
	if( controlStructure.isEmpty() ){

		return ;
	}

	resolve rr( rrr ... ) ;

	auto m = utility::split( controlStructure,' ' ) ;

	if( m.size() == 1 ){

		auto a = rr.option( m.at( 0 ) ) ;

		if( !a.isEmpty() ){

			orgs.append( a ) ;
		}

	}else if( m.size() == 2 ){		

		auto a = rr.option( m.at( 1 ) ) ;

		if( !a.isEmpty() ){

			orgs.append( m.at( 0 ) ) ;
			orgs.append( a ) ;
		}
	}else{
		auto s = QString( "Wrong control structure detected in custom backend named \"%1\"." ) ;
		utility::debug::logErrorWhileStarting( s.arg( name ) ) ;
	}
}

static QStringList _replace_opts( const resolveStruct& r )
{
	auto opts = r.opts ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.configFileArgument(),
		  resolve::args{ "%{cipherFolder}",r.args.cipherFolder },
		  resolve::args{ "%{configFileName}",r.engine.configFileName() },
		  resolve::args{ "%{configFilePath}",r.args.configFilePath } ) ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.keyFileArgument(),
		  resolve::args{ "%{keyfile}",r.args.keyFile } ) ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.idleString(),
		  resolve::args{ "%{timeout}",r.args.idleTimeout } ) ;

	return opts ;
}

template< typename Function >
static void _replace_opts( QStringList& mm,
			    const char * controlStructure,
			    Function function )
{
	for( int i = 0 ; i < mm.size() ; i++ ){

		auto& it = mm[ i ] ;

		if( it == controlStructure ){

			mm.removeAt( i ) ;

			function( { mm,i } ) ;

			break ;
		}
	}
}

static QStringList _resolve( const resolveStruct& r )
{
	auto mm = utility::split( r.controlStructure,' ' ) ;

	_replace_opts( mm,"%{cipherFolder}",[ & ]( replace s ){

		s.set( r.args.cipherFolder )  ;
	} ) ;

	_replace_opts( mm,"%{mountPoint}",[ & ]( replace s ){

		s.set( r.args.mountPoint ) ;
	} ) ;

	_replace_opts( mm,"%{password}",[ & ]( replace s ){

		s.set( r.password ) ;
	} ) ;

	_replace_opts( mm,"%{fuseOpts}",[ & ]( replace s ){

		if( !r.fuseOpts.isEmpty() ){

			if( r.args.fuseOptionsSeparator.isEmpty() ){

				s.set( { "-o",r.fuseOpts.join( ',' ) } ) ;
			}else{
				s.set( { r.args.fuseOptionsSeparator,"-o",r.fuseOpts.join( ',' ) } ) ;
			}
		}
	} ) ;

	_replace_opts( mm,"%{createOptions}",[ & ]( replace s ){

		s.set( _replace_opts( r ) ) ;
	} ) ;

	_replace_opts( mm,"%{mountOptions}",[ & ]( replace s ){

		auto opts = _replace_opts( r ) ;

		if( r.args.boolOptions.unlockInReverseMode ){

			opts.append( r.engine.reverseString() ) ;
		}

		s.set( opts ) ;
	} ) ;

	return mm ;
}

engines::engine::args custom::set_command( const engines::engine& engine,
					   const QByteArray& password,
					   const engines::engine::cmdArgsList& args,
					   bool create )
{
	engines::engine::commandOptions m( create,engine,args ) ;

	if( create ){

		auto opts = args.createOptions + m.exeOptions().get() ;

		auto s = _resolve( { engine,
				     engine.createControlStructure(),
				     args,
				     password,
				     opts,
				     m.fuseOpts().get() } ) ;

		return { args,m,engine.executableFullPath(),s } ;
	}else{
		auto s = _resolve( { engine,
				     engine.mountControlStructure(),
				     args,
				     password,
				     m.exeOptions().get(),
				     m.fuseOpts().get() } ) ;

		return { args,m,engine.executableFullPath(),s } ;
	}
}

engines::engine::args custom::command( const QByteArray& password,
				       const engines::engine::cmdArgsList& args,
				       bool create ) const
{
	return custom::set_command( *this,password,args,create ) ;
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
