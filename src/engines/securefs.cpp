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

#include "options.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
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
	s.releaseURL            = "https://api.github.com/repos/netheril96/securefs/releases" ;
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
	s.versionInfo           = { { "version",true,1,0 } } ;

	return s ;
}

securefs::securefs() :
	engines::engine( _setOptions() ),
	m_version_greater_or_equal_0_11_1( true,*this,0,11,1 )
{
}

engines::engine::args securefs::command( const QByteArray& password,
					 const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	if( args.create ){

		auto exeOptions  = m.exeOptions() ;

		exeOptions.add( args.opt.createOptions ) ;

		if( m_version_greater_or_equal_0_11_1 ){

			if( !args.opt.keyFile.isEmpty() ){

				exeOptions.addPair( "--keyfile",utility::Task::makePath( args.opt.keyFile ) ) ;

				if( !args.opt.key.isEmpty() ){

					exeOptions.add( "--askpass" ) ;
				}
			}
		}

		QString e = "%1 create %2 %3 %4" ;

		auto cmd = e.arg( args.exe,
				  exeOptions.get(),
				  args.configFilePath,
				  args.cipherFolder ) ;

		return { args,m,cmd } ;
	}else{
		QString exe = "%1 mount %2 %3 %4 %5" ;

		auto exeOptions  = m.exeOptions() ;
		auto fuseOptions = m.fuseOpts() ;

		if( utility::platformIsNOTWindows() ){

			exeOptions.add( "-b" ) ;
		}

		if( m_version_greater_or_equal_0_11_1 ){

			/*
			 * This version going forward takes fsname and fssubtype options
			 * through exe options and not fuse options
			 */
			auto fsname    = fuseOptions.extractStartsWith( "fsname=" ).mid( 7 ) ;
			auto fssubtype = fuseOptions.extractStartsWith( "subtype=" ).mid( 8 ) ;

			exeOptions.addPair( "--fsname",fsname ) ;
			exeOptions.addPair( "--fssubtype",fssubtype ) ;

			if( !args.opt.keyFile.isEmpty() ){

				exeOptions.addPair( "--keyfile",utility::Task::makePath( args.opt.keyFile ) ) ;

				if( !args.opt.key.isEmpty() ){

					exeOptions.add( "--askpass" ) ;
				}
			}
		}

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		auto cmd = exe.arg( args.exe,
				    exeOptions.get(),
				    args.cipherFolder,
				    args.mountPoint,
				    fuseOptions.get() ) ;

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

bool securefs::requiresAPassword( const engines::engine::cmdArgsList::options& opt ) const
{
	if( opt.mountOptions.contains( "--keyfile" ) || !opt.keyFile.isEmpty() ){

		return false ;
	}else{
		return engines::engine::requiresAPassword( opt ) ;
	}
}

void securefs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	securefscreateoptions::instance( *this,s,m_version_greater_or_equal_0_11_1 ) ;
}

void securefs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableIdleTime = false ;
	ee.enableCheckBox = false ;
	ee.enableKeyFile  = m_version_greater_or_equal_0_11_1 ;

	e.ShowUI() ;
}
