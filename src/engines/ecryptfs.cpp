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

#include "ecryptfs.h"
#include "commandOptions.h"
#include "ecryptfscreateoptions.h"

ecryptfs::ecryptfs() : backEnd::engine( "ecryptfs" )
{
}

const QStringList& ecryptfs::names() const
{
	return m_names ;
}

const QStringList& ecryptfs::fuseNames() const
{
	return m_fuseNames ;
}

siritask::status ecryptfs::notFoundCode() const
{
	return siritask::status::ecryptfs_simpleNotFound ;
}

QString ecryptfs::command( const backEnd::cmdArgsList& args ) const
{
	auto e = QString( "%1 %2 %3 -a %4 %5 %6" ) ;

	auto s = e.arg( args.exe,
			args.create ? args.opt.createOptions : "-o key=passphrase",
			args.opt.ro ? "--readonly" : "",
			args.configFilePath,
			args.cipherFolder,
			args.mountPoint ) ;

	if( args.opt.mountOptions.isEmpty() ){

		if( utility::useSiriPolkit() ){

			return utility::wrap_su( s ) ;
		}else{
			return s ;
		}
	}else{
		if( utility::useSiriPolkit() ){

			return utility::wrap_su( s + " -o " + args.opt.mountOptions ) ;
		}else{
			return s + " -o " + args.opt.mountOptions ;
		}
	}
}

siritask::status ecryptfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "operation not permitted" ) ){

		return siritask::status::ecrypfsBadExePermissions ;

	}else if( e.contains( "error: mount failed" ) ){

		return siritask::status::ecryptfsBadPassword ;
	}else{
		return siritask::status::backendFail ;
	}
}

bool ecryptfs::setsCipherPath() const
{
	return true ;
}

QString ecryptfs::configFileArgument() const
{
	return "--config" ;
}

QStringList ecryptfs::configFileNames() const
{
	return { ".ecryptfs.config","ecryptfs.config" } ;
}

bool ecryptfs::autoMountsOnCreate() const
{
	return true ;
}

QString ecryptfs::setPassword( const QString& e ) const
{
	return e ;
}

bool ecryptfs::hasGUICreateOptions() const
{
	return true ;
}

QString ecryptfs::defaultCreateOptions() const
{
	return "-o " + ecryptfscreateoptions::defaultCreateOptions() ;
}

void ecryptfs::GUICreateOptionsinstance( QWidget * parent,
					 std::function< void( const backEnd::engine::Options& ) > function ) const
{
	ecryptfscreateoptions::instance( parent,std::move( function ) ) ;
}

