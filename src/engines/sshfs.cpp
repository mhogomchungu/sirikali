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

#include "sshfs.h"
#include "../settings.h"
#include "options.h"

#include "custom.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	/*
	 * On my linux box, sshfs prompts six times when entered password is wrong before
	 * giving up, here, we simulate replaying the password 10 times hoping it will be
	 * enough for sshfs.
	 */

	s.passwordFormat              = "%{password}\n%{password}\n%{password}\n%{password}\n%{password}\n%{password}\n%{password}\n%{password}\n%{password}\n%{password}" ;
	s.backendTimeout              = settings::instance().sshfsBackendTimeout() ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
	s.likeSsh               = true ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = false ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = true ;
	s.executableName        = "sshfs" ;
	s.releaseURL            = "https://api.github.com/repos/libfuse/sshfs/releases" ;
	s.windowsInstallPathRegistryKey = "SOFTWARE\\SSHFS-Win" ;
	s.windowsInstallPathRegistryValue = "InstallDir" ;
	s.windowsUnMountCommand           = QStringList{ "taskkill","/F","/PID","%{PID}" } ;
	s.failedToMountList     = QStringList{ "ssh:","read:","Cannot create WinFsp-FUSE file system" } ;
	s.successfulMountedList = QStringList{ "has been started" } ;
	s.fuseNames             = QStringList{ "fuse.sshfs" } ;
	s.names                 = QStringList{ "sshfs" } ;
	s.notFoundCode          = engines::engine::status::sshfsNotFound ;
	s.versionInfo           = { { "--version",true,2,0 } } ;

	s.mountControlStructure = "%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;

	if( utility::platformIsWindows() ){

		s.autoCreatesMountPoint = true ;
		s.autoDeletesMountPoint = true ;

		s.minimumVersion = "3.5.2" ;
		s.sshOptions = "create_file_umask=0000,create_dir_umask=0000,umask=0000,idmap=user,StrictHostKeyChecking=no,UseNetworkDrive=no" ;
	}else{
		s.autoCreatesMountPoint = false ;
		s.autoDeletesMountPoint = false ;
		s.sshOptions = "idmap=user,StrictHostKeyChecking=no" ;
	}

	return s ;
}

sshfs::sshfs() :
	engines::engine( _setOptions() ),
	m_environment( engines::engine::getProcessEnvironment() ),
	m_version_greater_or_equal_minimum( false,*this,this->minimumVersion() )
{
}

engines::engine::status sshfs::passAllRequirenments( const engines::engine::cmdArgsList& opt ) const
{
	if( utility::platformIsWindows() ){

		auto m = engines::engine::passAllRequirenments( opt ) ;

		if( m != engines::engine::status::success ){

			return m ;

		}else if( m_version_greater_or_equal_minimum ){

			return engines::engine::status::success ;
		}else{
			return engines::engine::status::backEndFailedToMeetMinimumRequirenment ;
		}
	}else{
		return engines::engine::status::success ;
	}
}

const QProcessEnvironment& sshfs::getProcessEnvironment() const
{
	return m_environment ;
}

void sshfs::updateOptions( engines::engine::commandOptions& opts,
			   const engines::engine::cmdArgsList& args,
			   bool creating ) const
{
	Q_UNUSED( creating )

	auto fuseOptions = opts.fuseOpts() ;
	auto exeOptions  = opts.exeOptions() ;

	if( !args.key.isEmpty() ){

		fuseOptions.add( "password_stdin" ) ;
	}

	if( fuseOptions.doesNotContain( "StrictHostKeyChecking=" ) ){

		fuseOptions.add( "StrictHostKeyChecking=no" ) ;
	}

	auto s = fuseOptions.extractStartsWith( "UseNetworkDrive=" ) ;

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;

		if( utility::isDriveLetter( args.mountPoint ) ){

			if( !exeOptions.contains( "--VolumePrefix=" ) ){

				if( utility::endsWithAtLeastOne( s,"yes","Yes","YES" ) ){

					auto x = args.cipherFolder ;
					x.replace( ":",";" ) ;
					exeOptions.add ( "--VolumePrefix=\\mysshfs\\" + x ) ;
				}
			}
		}
	}

	if( fuseOptions.contains( "IdentityAgent" ) ){

		auto m = "IdentityAgent=" ;

		auto n = fuseOptions.extractStartsWith( m ).replace( m,"" ) ;

		m_environment.insert( "SSH_AUTH_SOCK",n ) ;

		utility::debug() << "Sshfs: Setting Env Variable Of: SSH_AUTH_SOCK=" + n ;
	}else{
		auto m = qgetenv( "SSH_AUTH_SOCK" ) ;

		if( m.isEmpty() ){

			m_environment.remove( "SSH_AUTH_SOCK" ) ;
		}else{
			utility::debug() << "Sshfs: Setting Env Variable Of: SSH_AUTH_SOCK=" + m ;

			m_environment.insert( "SSH_AUTH_SOCK",m ) ;
		}
	}
}

engines::engine::args sshfs::command( const QByteArray& password,
				      const engines::engine::cmdArgsList& args,
				      bool create ) const
{
	return custom::set_command( *this,password,args,create ) ;
}

engines::engine::status sshfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( "cygfuse: initialization failed: winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

void sshfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableCheckBox  = false ;
	ee.enableIdleTime  = false ;
	ee.enableConfigFile = false ;
	ee.enableKeyFile = false ;

	e.ShowUI() ;
}
