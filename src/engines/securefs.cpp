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

#include "securefs.h"

#include "securefscreateoptions.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.supportsMountPathsOnWindows = false ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = false ;
	s.passwordFormat        = "%{password}\n%{password}" ;
	s.executableName        = "securefs" ;
	s.incorrectPasswordText = "Invalid password" ;
	s.configFileArgument    = "--config" ;
	s.volumePropertiesCommands = QStringList{ "securefs info %{cipherFolder}" } ;
	s.windowsUnMountCommand = "sirikali.exe -T" ;
	s.configFileNames       = QStringList{ ".securefs.json","securefs.json" } ;
	s.fuseNames             = QStringList{ "fuse.securefs" } ;
	s.names                 = QStringList{ "securefs" } ;
	s.failedToMountList     = QStringList{ "Error","init" } ;
	s.successfulMountedList = QStringList{ "has been started","init" } ;
	s.notFoundCode          = engines::engine::status::securefsNotFound ;

	return s ;
}

securefs::securefs() : engines::engine( _setOptions() )
{
}

engines::engine::args securefs::command( const QString& password,
					 const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	if( args.create ){

		QString e = "%1 create %2 %3 %4" ;

		auto cmd = e.arg( args.exe,
				  args.opt.createOptions,
				  args.configFilePath,
				  args.cipherFolder ) ;

		return { args,m,cmd } ;
	}else{
		QString exe = "%1 mount %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		if( utility::platformIsNOTWindows() ){

			exeOptions.add( "-b" ) ;
		}

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

engines::engine::status securefs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::securefsBadPassword ;

	}else if( e.contains( "SecureFS cannot load WinFsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString securefs::installedVersionString() const
{
	return this->baseInstalledVersionString( "version",true,1,0 ) ;
}

void securefs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	securefscreateoptions::instance( parent,std::move( function ) ) ;
}
