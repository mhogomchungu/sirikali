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

#include "encfs.h"
#include "commandOptions.h"

encfs::encfs() : backEnd::engine( "encfs" )
{
}

const QStringList& encfs::names() const
{
	return m_names ;
}

const QStringList& encfs::fuseNames() const
{
	return m_fuseNames ;
}

siritask::status encfs::notFoundCode() const
{
	return siritask::status::encfsNotFound ;
}

QString encfs::command( const backEnd::cmdArgsList& args ) const
{
	QString e = "%1 %2 %3 %4 %5" ;

	commandOptions m( args,m_names.first(),m_names.first() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.create ){

		exeOptions.add( args.opt.createOptions,"--stdinpass","--standard" ) ;
	}else{
		exeOptions.add( "--stdinpass" ) ;
	}

	if( args.opt.reverseMode ){

		exeOptions.add( "--reverse" ) ;
	}

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( "-i",args.opt.idleTimeout ) ;
	}

	return e.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      m.fuseOpts().get() ) ;
}

siritask::status encfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return siritask::status::encfsBadPassword ;

	}else if( e.contains( "winfsp" ) ){

		return siritask::status::failedToLoadWinfsp ;
	}else{
		return siritask::status::backendFail ;
	}
}

bool encfs::setsCipherPath() const
{
	return false ;
}

bool encfs::supportsConfigFile() const
{
	return true ;
}

QStringList encfs::configFileNames() const
{
	return { ".encfs6.xml",".encfs5",".encfs4" } ;
}

bool encfs::autoMountsOnCreate() const
{
	return true ;
}

bool encfs::needDoublePassword() const
{
	return true ;
}
