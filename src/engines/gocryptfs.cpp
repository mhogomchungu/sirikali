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
#include "commandOptions.h"

#include "gocryptfscreateoptions.h"

gocryptfs::gocryptfs() : backEnd::engine( "gocryptfs" )
{
}

const QStringList& gocryptfs::names() const
{
	return m_names ;
}

const QStringList& gocryptfs::fuseNames() const
{
	return m_fuseNames ;
}

siritask::status gocryptfs::notFoundCode() const
{
	return siritask::status::gocryptfsNotFound ;
}

QString gocryptfs::command( const backEnd::cmdArgsList& args ) const
{
	if( args.create ){

		QString e = "%1 %2 %3" ;

		commandOptions m( args,m_names.first() ) ;

		auto exeOptions = m.exeOptions() ;

		exeOptions.add( "--init","-q",args.opt.createOptions ) ;

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		if( args.opt.reverseMode ){

			exeOptions.add( "-reverse" ) ;
		}

		return e.arg( args.exe,exeOptions.get(),args.cipherFolder ) ;
	}else{
		commandOptions m( args,"gocryptfs" ) ;

		QString e = "%1 %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		exeOptions.add( "-q" ) ;

		if( args.opt.reverseMode ){

			exeOptions.add( "-reverse" ) ;
		}

		if( !args.opt.idleTimeout.isEmpty() ){

			exeOptions.addPair( "-idle",args.opt.idleTimeout ) ;
		}

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		return e.arg( args.exe,
			      exeOptions.get(),
			      args.cipherFolder,
			      args.mountPoint,
			      m.fuseOpts().get() ) ;
	}
}

siritask::status gocryptfs::errorCode( const QString& e,int s ) const
{
	/*
	 * This error code was added in gocryptfs 1.2.1
	 */
	if( s == 12 ){

		return siritask::status::gocryptfsBadPassword ;

	}else if( e.contains( "password" ) ){

		return siritask::status::gocryptfsBadPassword ;
	}else{
		return siritask::status::backendFail ;
	}
}

bool gocryptfs::setsCipherPath() const
{
	return true ;
}

QString gocryptfs::configFileArgument() const
{
	return "--config" ;
}

QStringList gocryptfs::configFileNames() const
{
	return { "gocryptfs.conf",
		".gocryptfs.conf",
		 ".gocryptfs.reverse.conf",
		 "gocryptfs.reverse.conf"} ;
}

bool gocryptfs::autoMountsOnCreate() const
{
	return false ;
}

QString gocryptfs::setPassword( const QString& e ) const
{
	return e ;
}

bool gocryptfs::hasGUICreateOptions() const
{
	return true ;
}

QString gocryptfs::defaultCreateOptions() const
{
	return QString() ;
}

void gocryptfs::GUICreateOptionsinstance( QWidget * parent,
					  std::function< void( const backEnd::engine::Options& ) > function ) const
{
	gocryptfscreateoptions::instance( parent,std::move( function ) ) ;
}
