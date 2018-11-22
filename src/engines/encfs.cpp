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

#include "encfscreateoptions.h"

engines::engine::BaseOptions encfs::setOptions()
{
	BaseOptions s ;

	s.autoMountsOnCreate  = true ;
	s.hasGUICreateOptions = true ;
	s.setsCipherPath      = false ;

	s.configFileArgument  = "--config" ;

	s.configFileNames = QStringList{ ".encfs6.xml","encfs6.xml",".encfs5",".encfs4" } ;

	s.fuseNames = QStringList{ "fuse.encfs" } ;
	s.names     = QStringList{ "encfs" } ;

	s.notFoundCode = engines::engine::status::encfsNotFound ;

	return s ;
}

encfs::encfs() : engines::engine( this->setOptions() )
{
}

QString encfs::command( const engines::engine::cmdArgsList& args ) const
{
	QString e = "%1 %2 %3 %4 %5" ;

	commandOptions m( args,this->name(),this->name() ) ;

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

engines::engine::status encfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return engines::engine::status::encfsBadPassword ;

	}else if( e.contains( "winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString encfs::setPassword( const QString& e ) const
{
	return e + "\n" + e ;
}

void encfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	encfscreateoptions::instance( parent,std::move( function ) ) ;
}
