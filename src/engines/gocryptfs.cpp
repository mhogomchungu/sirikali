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

#include "gocryptfscreateoptions.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.supportsMountPathsOnWindows = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.passwordFormat        = "%{password}" ;
	s.volumePropertiesCommands = QStringList{ "gocryptfs -info %{cipherFolder}",
						  "gocryptfs -info %{plainFolder}" } ;
	s.reverseString         = "-reverse" ;
	s.idleString            = "-idle" ;
	s.executableName        = "gocryptfs" ;
	s.incorrectPasswordText = "Password incorrect." ;
	s.configFileArgument    = "--config" ;
	s.configFileNames       = QStringList{ "gocryptfs.conf",
					       ".gocryptfs.conf",
					       ".gocryptfs.reverse.conf",
					       "gocryptfs.reverse.conf" } ;
	s.fuseNames             = QStringList{ "fuse.gocryptfs","fuse.gocryptfs-reverse" } ;
	s.names                 = QStringList{ "gocryptfs","gocryptfs.reverse","gocryptfs-reverse" } ;
	s.notFoundCode          = engines::engine::status::gocryptfsNotFound ;

	return s ;
}

gocryptfs::gocryptfs() : engines::engine( _setOptions() )
{
}

engines::engine::args gocryptfs::command( const QString& password,
					  const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password ) ;

	engines::engine::commandOptions m( args,this->name() ) ;

	auto exeOptions  = m.exeOptions() ;
	auto fuseOptions = m.fuseOpts() ;

	exeOptions.add( "-q" ) ;

	if( args.opt.reverseMode ){

		exeOptions.add( this->reverseString() ) ;
	}

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( this->idleString(),args.opt.idleTimeout ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	QString cmd ;

	if( args.create ){

		exeOptions.add( "--init",args.opt.createOptions ) ;

		QString e = "%1 %2 %3" ;

		cmd = e.arg( args.exe,exeOptions.get(),args.cipherFolder ) ;
	}else{
		QString e = "%1 %2 %3 %4 %5" ;

		if( !utility::platformIsLinux() ){

			fuseOptions.extractStartsWith( "volname=" ) ;
		}

		cmd = e.arg( args.exe,
			     exeOptions.get(),
			     args.cipherFolder,
			     args.mountPoint,
			     fuseOptions.get() ) ;
	}

	return { args,m,cmd } ;
}

engines::engine::status gocryptfs::errorCode( const QString& e,int s ) const
{
	/*
	 * This error code was added in gocryptfs 1.2.1
	 */
	if( s == 12 || e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::gocryptfsBadPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString gocryptfs::installedVersionString() const
{
	return this->baseInstalledVersionString( "--version",true,1,0 ) ;
}

void gocryptfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	gocryptfscreateoptions::instance( parent,std::move( function ) ) ;
}
