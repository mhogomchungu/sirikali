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
#include "json.h"
#include "install_prefix.h"

#include "customcreateoptions.h"

#include <QDir>
#include <QStandardPaths>

static utility::result< engines::engine::BaseOptions > _getOptions( const QByteArray& e,const QString& s )
{
	try{
		engines::engine::BaseOptions s ;

		auto json = nlohmann::json::parse( e.constData() ) ;

		auto _getStringList = [ & ]( const char * m ){

			QStringList s ;

			const auto e = json[ m ].get< std::vector< std::string > >() ;

			for( const auto& it : e ){

				if( !it.empty() ){

					s.append( it.c_str() ) ;
				}
			}

			return s ;
		} ;

		auto _getString = [ & ]( const char * s ){

			return json[ s ].get< std::string >().c_str() ;
		} ;

		auto _getBool = [ & ]( const char * s ){

			return json[ s ].get< bool >() ;
		} ;

		s.hasGUICreateOptions         = true ;
		s.customBackend               = true ;
		s.requiresAPassword           = _getBool( "requiresAPassword" ) ; ;
		s.autoMountsOnCreate          = _getBool( "autoMountsOnVolumeCreation" ) ;
		s.setsCipherPath              = _getBool( "setsCipherPath" ) ;
		s.supportsMountPathsOnWindows = _getBool( "supportsMountPointPaths" ) ;
		s.executableName              = _getString( "executableName" ) ;
		s.incorrectPasswordText       = _getString( "wrongPasswordText" ) ;
		s.incorrectPassWordCode       = _getString( "wrongPasswordErrorCode" ) ;
		s.configFileArgument          = _getString( "configFileArgument" ) ;
		s.failedToMountList           = _getStringList( "failedToMountTextList" ) ;
		s.successfulMountedList       = _getStringList( "successfullyMountedList" ) ;
		s.configFileNames             = _getStringList( "configFileNames" ) ;
		s.names                       = _getStringList( "names" ) ;
		s.fuseNames                   = _getStringList( "fuseNames" ) ;
		s.hasConfigFile               = s.configFileNames.size() > 0 ;
		s.notFoundCode                = engines::engine::status::customCommandNotFound ;

		return s ;

	}catch( ... ){

		utility::debug::cout() << "Failed to parse config file: " + s ;

		return {} ;
	}
}

static void _add_engines( QStringList& list,
			  const QString& path,
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

				if( m.names.size() > 0 ){

					list.append( m.names.first() ) ;
					engines.emplace_back( std::make_unique< custom >( m ) ) ;
				}else{
					utility::debug::cout() << "Name field not set in config file : " + path ;
				}
			}

			file.close() ;
		}
	}
}

void custom::addEngines( QStringList& list,std::vector< std::unique_ptr< engines::engine >>& engines )
{
	auto m = QStandardPaths::standardLocations( QStandardPaths::ConfigLocation ) ;

	if( !m.isEmpty() ){

		_add_engines( list,m.first() + "/SiriKali/backends/",engines ) ;
	}

	_add_engines( list,INSTALL_PREFIX"/share/SiriKali/backends/",engines ) ;
}

custom::custom( engines::engine::BaseOptions s ) : engines::engine( std::move( s ) )
{
}

engines::engine::args custom::command( const engines::engine::cmdArgsList& args ) const
{
	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	if( args.create ){

		QString e = "%1 %2 %3 %4" ;

		auto cmd = e.arg( args.exe,
				  args.opt.createOptions,
				  args.cipherFolder,
				  args.mountPoint ) ;

		return { args,m,cmd } ;
	}else{
		QString exe = "%1 %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		auto cmd = exe.arg( args.exe,
				    exeOptions.get(),
				    args.cipherFolder,
				    args.mountPoint,
				    m.fuseOpts().get() ) ;

		return { args,m,cmd } ;
	}
}

engines::engine::status custom::errorCode( const QString& e,int s ) const
{
	auto m = this->incorrectPasswordCode() ;

	if( !m.isEmpty() ){

		bool ok ;

		auto n = m.toInt( &ok ) ;

		if( ok && n == s ){

			return engines::engine::status::customCommandBadPassword ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}else{
		if( e.contains( this->incorrectPasswordText() ) ){

			return engines::engine::status::customCommandBadPassword ;

		}else if( e.contains( "cannot load WinFsp" ) ){

			return engines::engine::status::failedToLoadWinfsp ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}
}

QString custom::setPassword( const QString& e ) const
{
	return e ;
}

QString custom::installedVersionString() const
{
	return QString() ;
}

void custom::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	customcreateoptions::instance( parent,std::move( function ) ) ;
}
