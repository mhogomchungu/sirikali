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

#include "cryfs.h"
#include "commandOptions.h"
#include "cryfscreateoptions.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.autoMountsOnCreate  = true ;
	s.hasGUICreateOptions = true ;
	s.setsCipherPath      = true ;

	s.configFileArgument  = "--config" ;

	s.configFileNames = QStringList{ "cryfs.config",".cryfs.config" } ;

	s.fuseNames = QStringList{ "fuse.cryfs" } ;
	s.names     = QStringList{ "cryfs" } ;

	s.notFoundCode = engines::engine::status::cryfsNotFound ;

	return s ;
}

cryfs::cryfs() : engines::engine( _setOptions() )
{
}

QString cryfs::command( const engines::engine::cmdArgsList& args ) const
{
	auto separator = [](){

		auto m = utility::backendIsLessThan( "cryfs","0.10" ).get() ;

		if( m && m.value() ){

			return "--" ;
		}else{
			return "" ;
		}
	}() ;

	auto e = QString( "%1 %2 %3 %4 %5 %6" ) ;

	commandOptions m( args,this->name(),this->name() ) ;

	auto exeOptions = m.exeOptions() ;

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( "--unmount-idle",args.opt.idleTimeout ) ;
	}

	if( args.create ){

		exeOptions.add( args.opt.createOptions ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	return e.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      separator,
		      m.fuseOpts().get() ) ;
}

engines::engine::status cryfs::errorCode( const QString& e,int s ) const
{
	auto m = utility::backendIsGreaterOrEqualTo( "cryfs","0.9.9" ).get() ;

	if( m && m.value() ){

		/*
		 * Error codes are here: https://github.com/cryfs/cryfs/blob/develop/src/cryfs/ErrorCodes.h
		 *
		 * Valid for cryfs > 0.9.8
		 */

		if( s == 11 ){

			return engines::engine::status::cryfsBadPassword ;

		}else if( s == 14 ){

			return engines::engine::status::cryfsMigrateFileSystem ;
		}
	}else{
		/*
		 * Falling back to parsing strings
		 */

		if( e.contains( "password" ) ){

			return engines::engine::status::cryfsBadPassword ;

		}else if( e.contains( "this filesystem is for cryfs" ) &&
			  e.contains( "it has to be migrated" ) ){

			return engines::engine::status::cryfsMigrateFileSystem ;
		}
	}

	return engines::engine::status::backendFail ;
}

QString cryfs::setPassword( const QString& e ) const
{
	return e ;
}

QString cryfs::installedVersionString() const
{
	if( m_version.isEmpty() ){

		m_version = this->baseInstalledVersionString( "--version",true,2,0 ) ;
	}

	return m_version ;
}

void cryfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	cryfscreateoptions::instance( parent,std::move( function ) ) ;
}

