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

	if( utility::platformIsWindows() ){

		s.mountControlStructure = "mount %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}else{
		s.mountControlStructure = "mount -b %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
	}

	s.createControlStructure = "create %{createOptions} %{cipherFolder}" ;

	return s ;
}

securefs::securefs() :
	engines::engine( _setOptions() ),
	m_version_greater_or_equal_0_11_1( true,*this,0,11,1 ),
	m_version_greater_or_equal_1_0_0( true,*this,1,0,0 )
{
}

engines::engine::cmdStatus securefs::commandStatus( const engines::engine::commandStatusOpts& e ) const
{
	if( utility::platformIsWindows() ){

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
			auto ss = e.stdOut() + "\n" + e.stdError() + "\n" + this->extraLogOutput( e.args() ) ;

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
	if( m_version_greater_or_equal_1_0_0 ){

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

static QByteArray _get_log( QFile&& file )
{
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
	if( m_version_greater_or_equal_1_0_0  && utility::platformIsNOTWindows() ){

		const auto& opts = e.cmd_args ;

		for( int s = 0 ; s < opts.size() ; s++ ){

			if( opts[ s ] == "--log" ){

				if( s + 1 < opts.size() ){

					return _get_log( opts[ s + 1 ] ) ;
				}
			}
		}
	}

	return {} ;
}

bool securefs::canShowVolumeProperties() const
{
	return !m_version_greater_or_equal_1_0_0 ;
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
	if( m_version_greater_or_equal_0_11_1 ){

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

	if( !creating && m_version_greater_or_equal_0_11_1 ){

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

		if( m_version_greater_or_equal_1_0_0 && utility::platformIsNOTWindows() ){

			exeOpts.add( "--log",_logPath( args.mountPoint ) ) ;
		}
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
