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
#include "cryfscreateoptions.h"
#include "../win.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	auto a = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26116061-4F99-4C44-A178-2153FA396308}" ;
	auto b = "InstallLocation" ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.passwordFormat        = "%{password}" ;
	s.idleString            = "--unmount-idle" ;
	s.executableName        = "cryfs" ;
	s.incorrectPasswordText = "Could not load config file. Did you enter the correct password?" ;
	s.configFileArgument    = "--config" ;
	s.windowsInstallPathRegistryKey   = a ;
	s.windowsInstallPathRegistryValue = b ;
	s.windowsUnMountCommand = SiriKali::Windows::engineInstalledDir( a,b ) + "\\bin\\cryfs-unmount.exe" ;
	s.successfulMountedList = QStringList{ "Mounting filesystem." } ;
	s.configFileNames       = QStringList{ "cryfs.config",".cryfs.config" } ;
	s.fuseNames             = QStringList{ "fuse.cryfs" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "cryfs" } ;
	s.notFoundCode          = engines::engine::status::cryfsNotFound ;

	return s ;
}

cryfs::cryfs() :
	engines::engine( _setOptions() ),
	m_env( engines::engine::getProcessEnvironment() ),
	m_version( [ this ]{ return this->baseInstalledVersionString( "--version",true,2,0 ) ; } )
{
	m_env.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
	m_env.insert( "CRYFS_FRONTEND","noninteractive" ) ;
}

bool cryfs::takesTooLongToUnlock() const
{
	return this->versionGreaterOrEqualTo( "0.10.0" ) ;
}

const QProcessEnvironment& cryfs::getProcessEnvironment() const
{
	return m_env ;
}

engines::engine::args cryfs::command( const QByteArray& password,
				      const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	auto separator = [ & ](){

		if( this->versionGreaterOrEqualTo( "0.10.0" ) ){

			return "" ;
		}else{
			return "--" ;
		}
	}() ;

	auto e = QString( "%1 %2 %3 %4 %5 %6" ) ;

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	auto exeOptions = m.exeOptions() ;

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;
	}

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( this->idleString(),args.opt.idleTimeout ) ;
	}

	if( args.create ){

		exeOptions.add( args.opt.createOptions ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	auto cmd = e.arg( args.exe,
			  exeOptions.get(),
			  args.cipherFolder,
			  args.mountPoint,
			  separator,
			  m.fuseOpts().get() ) ;

	return { args,m,cmd } ;
}

engines::engine::status cryfs::errorCode( const QString& e,int s ) const
{
	auto m = [ & ](){

		if( utility::platformIsWindows() ){

			return false ;
		}else{
			return this->versionGreaterOrEqualTo( "0.9.9" ) ;
		}
	}() ;

	if( m ){

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
		if( utility::containsAtleastOne( e,"Error 11:",this->incorrectPasswordText() ) ){

			return engines::engine::status::cryfsBadPassword ;

		}else if( e.contains( "This filesystem is for CryFS" ) &&
			  e.contains( "It has to be migrated" ) ){

			return engines::engine::status::cryfsMigrateFileSystem ;
		}
	}

	return engines::engine::status::backendFail ;
}

const QString& cryfs::installedVersionString() const
{	
	return m_version.get() ;
}

void cryfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	cryfscreateoptions::instance( parent,std::move( function ) ) ;
}
