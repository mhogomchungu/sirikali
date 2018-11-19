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

cryfs::cryfs() : engines::engine( "cryfs" )
{
}

const QStringList& cryfs::names() const
{
	return m_names ;
}

const QStringList& cryfs::fuseNames() const
{
	return m_fuseNames ;
}

engines::engine::status cryfs::notFoundCode() const
{
	return engines::engine::status::cryfsNotFound ;
}

QString cryfs::command( const engines::engine::cmdArgsList& args ) const
{
	auto separator = [](){

		/*
			 * declaring this variable as static to force this function to be called only
			 * once.
			 */
		static auto m = utility::backendIsLessThan( "cryfs","0.10" ).get() ;

		if( m && m.value() ){

			return "--" ;
		}else{
			return "" ;
		}
	}() ;

	auto e = QString( "%1 %2 %3 %4 %5 %6" ) ;

	commandOptions m( args,m_names.first(),m_names.first() ) ;

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
	/*
		 * Error codes are here: https://github.com/cryfs/cryfs/blob/develop/src/cryfs/ErrorCodes.h
		 *
		 * Valid for cryfs > 0.9.8
		 */

	if( s == 11 ){

		return engines::engine::status::cryfsBadPassword ;

	}else if( s == 14 ){

		return engines::engine::status::cryfsMigrateFileSystem ;
	}else{
		/*
			 * Falling back to parsing strings
			 */

		if( e.contains( "password" ) ){

			return engines::engine::status::cryfsBadPassword ;

		}else if( e.contains( "this filesystem is for cryfs" ) &&
			  e.contains( "it has to be migrated" ) ){

			return engines::engine::status::cryfsMigrateFileSystem ;
		}else{
			return engines::engine::status::backendFail ;
		}
	}
}

bool cryfs::setsCipherPath() const
{
	return true ;
}

QString cryfs::configFileArgument() const
{
	return "--config" ;
}

QStringList cryfs::configFileNames() const
{
	return { "cryfs.config",".cryfs.config" } ;
}

bool cryfs::autoMountsOnCreate() const
{
	return true ;
}

QString cryfs::setPassword( const QString& e ) const
{
	return e ;
}

bool cryfs::hasGUICreateOptions() const
{
	return true ;
}

QString cryfs::defaultCreateOptions() const
{
	return QString() ;
}

void cryfs::GUICreateOptionsinstance( QWidget * parent,
				      std::function< void( const engines::engine::Options& ) > function ) const
{
	cryfscreateoptions::instance( parent,std::move( function ) ) ;
}
