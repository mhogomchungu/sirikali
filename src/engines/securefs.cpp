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

#include "custom.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = false ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
	s.usesFuseArgumentSwitch      = true ;
	s.likeSsh               = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = true ;
	s.releaseURL            = "https://api.github.com/repos/netheril96/securefs/releases" ;
	s.passwordFormat        = "%{password}\n%{password}" ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.keyFileArgument       = "--keyfile %{keyfile}" ;
	s.volumePropertiesCommands = QStringList{ "securefs info %{cipherFolder}" } ;
	s.incorrectPasswordText = QStringList{ "Invalid password","Password/keyfile is incorrect" } ;
	s.windowsUnMountCommand = QStringList{ "sirikali.exe", "-T","%{PID}" } ;
	s.configFileNames       = QStringList{ ".securefs.json","securefs.json",".config.pb","config.pb" } ;
	s.fuseNames             = QStringList{ "fuse.securefs" } ;
	s.names                 = QStringList{ "securefs" } ;
	s.failedToMountList     = QStringList{ "Error","init" } ;
	s.successfulMountedList = QStringList{ "has been started","init" } ;
	s.executableNames       = QStringList{ "securefs" } ;

	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "version",true,1,0 } } ;

	if( utility::platformIsOSX() ){

		s.unMountCommand = QStringList{ "umount","%{mountPoint}" } ;
	}

	if( utility::platformIsWindows() || utility::platformIsFlatPak() ){

		s.mountControlStructure = "mount %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}else{
		s.mountControlStructure = "mount -b %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}

	s.createControlStructure = "create %{createOptions} %{cipherFolder}" ;

	return s ;
}

securefs::securefs() : engines::engine( _setOptions() )
{
	if( this->installedVersionGreaterOrEqual( "2.0.0" ) ){

		engines::engine::updateOptions( [ this ]( BaseOptions& s ){

			s.idleString = "--max-idle-seconds %{timeout}" ;

			auto e = this->executableFullPath() ;

			if( !e.isEmpty() ){

				e = utility::cleanPath( e ) ;

				auto m = QStringList{ e,"unmount","%{mountPoint}" } ;

				if( utility::platformIsWindows() ){

					s.windowsUnMountCommand = m ;
				}else{
					s.unMountCommand = m ;
				}
			}
		} ) ;
	}
}

bool securefs::updatable( bool s ) const
{
	if( s ){

		if( utility::platformIsFlatPak() ){

			return false ;
		}else{
			return utility::archInUse( utility::arch::x64 ) ;
		}
	}else{
		return false ;
	}
}

void securefs::setUpBinary( bool add,QStringList& apps,const QString& basePath ) const
{
	engines::engine::setUpBinary( add,apps,basePath,"securefs" ) ;
}

void securefs::removeExtraFiles( const QString& e ) const
{
	if( utility::platformIsWindows() ){

		auto m = e + "/securefs.exe.sig" ;

		if( QFile::exists( m ) ){

			QFile::remove( m ) ;
		}
	}else{
		auto m = e + "/securefs.sig" ;

		if( QFile::exists( m ) ){

			QFile::remove( m ) ;
		}
	}
}

bool securefs::onlineArchiveFileName( const QString& e ) const
{
	if( utility::platformIsLinux() ){

		return e.endsWith( "securefs-linux-amd64-release.zip" ) ;

	}else if( utility::platformIsWindows() ){

		return e.endsWith( "securefs-windows-amd64-release.zip" ) ;
	}else{
		return e.endsWith( "securefs-macos-amd64-release.zip" ) ;
	}
}

engines::engine::args securefs::command( const QByteArray& password,const cmdArgsList& args,bool create ) const
{
	if( utility::platformIsFlatPak() ){

		auto m = engines::engine::command( password,args,create ) ;

		m.cmd_args.prepend( m.cmd ) ;
		m.cmd_args.prepend( "--host" ) ;

		m.cmd = "flatpak-spawn" ;

		return m ;
	}else{
		return engines::engine::command( password,args,create ) ;
	}
}

engines::engine::cmdStatus securefs::commandStatus( const engines::engine::commandStatusOpts& e ) const
{
	if( utility::platformIsWindows() || utility::platformIsFlatPak() ){

		return engines::engine::commandStatus( e ) ;
	}else{
		const auto& engine = *this ;

		if( e.creating() ){

			if( e.success() ){

				return { engines::engine::status::success,engine } ;
			}else{
				auto ss = e.stdOut() + "\n" + e.stdError() ;

				return { engines::engine::status::backendFail,engine,ss } ;
			}
		}else{
			auto ss = e.stdOut() + "\n" + e.stdError() ;

			if( this->useLogFile() ){

				ss += "\n" + this->extraLogOutput( e.args() ) ;
			}

			auto n = engine.errorCode( ss,ss,e.exitCode() ) ;

			return { n,engine,ss } ;
		}
	}
}

static engines::engine::status _check_password( const QString& m,const QStringList& e )
{
	for( const auto& s : e ){

		if( m.contains( s ) ){

			return engines::engine::status::badPassword ;
		}
	}

	return engines::engine::status::backendFail ;
}

engines::engine::status securefs::errorCode( const QString& e,const QString&,int s ) const
{
	if( this->installedVersionGreaterOrEqual( "2.0.0" ) ){

		if( s == 0 ){

			return engines::engine::status::success ;
		}else{
			return _check_password( e,this->incorrectPasswordText() ) ;
		}

	}else if( this->installedVersionGreaterOrEqual( "1.0.0" ) ){

		if( e.contains( " Fuse operations initialized" ) ){

			return engines::engine::status::success ;
		}else{
			return _check_password( e,this->incorrectPasswordText() ) ;
		}
	}else{
		if( s == 0 ){

			return engines::engine::status::success ;
		}else{
			return _check_password( e,this->incorrectPasswordText() ) ;
		}
	}
}

static QByteArray _get_log( const QString& path )
{
	QFile file( path ) ;

	for( int s = 0 ; s < 5 ; s++ ){

		if( file.open( QIODevice::ReadOnly ) ){

			QByteArray m ;

			while( true ){

				m += file.readAll() ;

				if( m.contains( "\n" ) ){

					break ;
				}else{
					utility::Task::suspendForOneSecond() ;
				}
			}

			file.remove() ;

			return m ;
		}else{
			utility::Task::suspendForOneSecond() ;
		}
	}

	return {} ;
}

QByteArray securefs::extraLogOutput( const engines::engine::args& e ) const
{
	const auto& opts = e.cmd_args ;

	for( int s = 0 ; s < opts.size() ; s++ ){

		if( opts[ s ] == "--log" ){

			if( s + 1 < opts.size() ){

				return _get_log( opts[ s + 1 ] ) ;
			}
		}
	}

	return {} ;
}

bool securefs::canShowVolumeProperties() const
{
	return !this->installedVersionGreaterOrEqual( "1.0.0" ) ;
}

static void _updateKeys( QStringList& opts,engines::engine::cmdArgsList& e )
{
	if( !e.keyFile.isEmpty() ){

		if( !e.key.isEmpty() ){

			opts.append( "--askpass" ) ;
		}
	}
}

void securefs::updateOptions( engines::engine::cmdArgsList& e,bool creating ) const
{
	if( this->installedVersionGreaterOrEqual( "0.11.1" ) ){

		if( creating ){

			_updateKeys( e.createOptions,e ) ;
		}else{
			_updateKeys( e.mountOptions,e ) ;
		}
	}
}

static QString _logPath( const QString& e )
{
	while( true ){

		auto s = e + "." + QString::number( std::rand() ) ;

		if( !QFile::exists( s ) ){

			return s ;
		}
	}
}

void securefs::updateOptions( engines::engine::commandOptions& opts,
			      const engines::engine::cmdArgsList& args,
			      bool creating ) const
{
	Q_UNUSED( args )

	if( !creating && this->installedVersionGreaterOrEqual( "0.11.1" ) ){

		auto fuseOpts = opts.fuseOpts() ;
		auto exeOpts  = opts.exeOptions() ;

		auto fsname = fuseOpts.extractStartsWith( "fsname" ) ;

		auto fssubtype = fuseOpts.extractStartsWith( "subtype" ) ;

		if( fsname ){

			exeOpts.add( "--fsname",fsname.RValue().mid( 7 ) ) ;
		}

		if( fssubtype ){

			exeOpts.add( "--fssubtype",fssubtype.RValue().mid( 8 ) ) ;
		}

		if( this->useLogFile() ){

			exeOpts.add( "--log",_logPath( args.mountPoint ) ) ;
		}
	}
}

bool securefs::useLogFile() const
{
	if( utility::platformIsWindows() || utility::platformIsFlatPak() ){

		return false ;

	}else if( this->installedVersionGreaterOrEqual( "1.0.0" ) ){

		if( this->installedVersionGreaterOrEqual( "2.0.0" ) ){

			return false ;
		}else{
			return true ;
		}
	}else{
		return false ;
	}
}

bool securefs::requiresAPassword( const engines::engine::cmdArgsList& opt ) const
{
	if( opt.mountOptions.contains( "--keyfile" ) || !opt.keyFile.isEmpty() ){

		return false ;
	}else{
		return engines::engine::requiresAPassword( opt ) ;
	}
}

void securefs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	securefscreateoptions::instance( *this,s,this->installedVersionGreaterOrEqual( "0.11.1" ) ) ;
}

void securefs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableCheckBox  = false ;
	ee.checkBoxChecked = false ;
	ee.enableIdleTime  = this->installedVersionGreaterOrEqual( "2.0.0" ) ;
	ee.enableKeyFile   = this->installedVersionGreaterOrEqual( "0.11.1" ) ;
	ee.idleTimeOutText = QObject::tr( "Automatically Unmount After Specified Seconds of Inactivity Is Reached." ) ;

	e.ShowUI() ;
}
