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

#include "encfscreateoptions.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.autoMountsOnCreate  = true ;
	s.hasGUICreateOptions = true ;
	s.setsCipherPath      = false ;
	s.supportsMountPathsOnWindows = true ;

	s.configFileArgument  = "--config" ;

	s.configFileNames = QStringList{ ".encfs6.xml","encfs6.xml",".encfs5",".encfs4" } ;

	s.fuseNames = QStringList{ "fuse.encfs" } ;
	s.names     = QStringList{ "encfs" } ;

	s.notFoundCode = engines::engine::status::encfsNotFound ;

	return s ;
}

encfs::encfs() : engines::engine( _setOptions() )
{
}

engines::engine::args encfs::command( const engines::engine::cmdArgsList& args ) const
{
	QString e = "%1 %2 %3 %4 %5" ;

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

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

		auto m = args.mountPoint.mid( 1,args.mountPoint.size() - 2 ) ;

		if( !utility::isDriveLetter( m ) ){

			/*
			 * A user is trying to use a folder as a mount path and encfs
			 * requires the mount path to not exist and we are deleting
			 * it because SiriKali created it previously.
			 */
			utility::removeFolder( m,5 ) ;
		}
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( "-i",args.opt.idleTimeout ) ;
	}

	auto cmd = e.arg( args.exe,
			  exeOptions.get(),
			  args.cipherFolder,
			  args.mountPoint,
			  m.fuseOpts().get() ) ;

	return { args,m,cmd } ;
}

engines::engine::error encfs::errorCode( const QString& e ) const
{
	if( utility::containsAtleastOne( e,"has been started" ) ){

		return engines::engine::error::Success ;

	}else if( e.contains( "Error" ) ){

		return engines::engine::error::Failed ;
	}else{
		return engines::engine::error::Continue ;
	}
}

engines::engine::status encfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "Error decoding volume key, password incorrect" ) ){

		return engines::engine::status::encfsBadPassword ;

	}else if( e.contains( "cygfuse: initialization failed: winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString encfs::setPassword( const QString& e ) const
{
	return e + "\n" + e ;
}

QString encfs::installedVersionString() const
{
	return this->baseInstalledVersionString( "--version",false,2,0 ) ;
}

void encfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	encfscreateoptions::instance( parent,std::move( function ) ) ;
}
