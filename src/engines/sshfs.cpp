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
	s.usesOnlyMountPoint          = false ;
	s.usesFuseArgumentSwitch      = true ;
	s.likeSsh               = true ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = true ;
	s.acceptsSubType        = true ;
	s.acceptsVolName        = true ;
	s.releaseURL            = "https://api.github.com/repos/libfuse/sshfs/releases" ;
	s.windowsInstallPathRegistryKey = "SOFTWARE\\SSHFS-Win" ;
	s.windowsInstallPathRegistryValue = "InstallDir" ;
	s.windowsUnMountCommand           = QStringList{ "taskkill","/F","/PID","%{PID}" } ;
	s.failedToMountList     = QStringList{ "ssh:","read:","Cannot create WinFsp-FUSE file system" } ;
	s.successfulMountedList = QStringList{ "has been started" } ;
	s.fuseNames             = QStringList{ "fuse.sshfs" } ;
	s.names                 = QStringList{ "sshfs" } ;
	s.executableNames       = QStringList{ "sshfs" } ;

	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "--version",true,2,0 } } ;

	s.mountControlStructure = "%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;

	if( utility::platformIsWindows() ){

		s.autoCreatesMountPoint = true ;
		s.autoDeletesMountPoint = true ;

		s.versionMinimum = "3.5.2" ;
		s.defaultFavoritesMountOptions = "create_file_umask=0000,create_dir_umask=0000,umask=0000,idmap=user,StrictHostKeyChecking=no" ;
	}else{
		s.autoCreatesMountPoint = false ;
		s.autoDeletesMountPoint = false ;
		s.defaultFavoritesMountOptions = "idmap=user,StrictHostKeyChecking=no" ;
	}

	return s ;
}

sshfs::sshfs() :
	engines::engine( _setOptions() ),
	m_environment( engines::engine::getProcessEnvironment() ),
	m_version_greater_or_equal_minimum( false,*this,this->minimumVersion() ),
	m_sshAuthSock( qgetenv( "SSH_AUTH_SOCK" ) )
{
}

bool sshfs::requiresAPassword( const engines::engine::cmdArgsList& e ) const
{
	if( !m_sshAuthSock.isEmpty() || !e.identityFile.isEmpty() || !e.identityAgent.isEmpty() ){

		return false ;
	}else{
		return engines::engine::requiresAPassword( e ) ;
	}
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
		return engines::engine::passAllRequirenments( opt ) ;
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

	if( fuseOptions.doesNotContain( "StrictHostKeyChecking=" ) ){

		fuseOptions.add( "StrictHostKeyChecking=no" ) ;
	}

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;

		if( utility::isDriveLetter( args.mountPoint ) ){

			auto _volPrefix = []( QString& m ){

				if( m.size() > 15 + 192 ){
					/*
					 * 15 is the size of "--VolumePrefix=" and 192 is
					 * the maximum size allowed to be stored in VolumePrefix.
					 * If the value stored is more than 32 characters,
					 * we truncate it to 189 characters and then add three dots.
					 *
					 * The 192 number is taken from winfsp source code: https://github.com/billziss-gh/winfsp/blob/c803ef24f8f777bc0c8c6a9571d70ce1d23116dd/inc/winfsp/fsctl.h#L83
					 */
					m = m.mid( 0,15 + 189 ) + "..." ;
				}
			} ;

			auto m = exeOptions.optionStartsWith( "--VolumePrefix=" ) ;

			if( m ){

				_volPrefix( m.value() ) ;

			}else if( args.boolOptions.unlockInReverseMode ){

				auto x = "--VolumePrefix=\\mysshfs\\" + args.cipherFolder ;
				x.replace( ":",";" ) ;

				_volPrefix( x ) ;

				exeOptions.add( x ) ;
			}
		}
	}

	if( args.key.isEmpty() ){

		fuseOptions.add( "PreferredAuthentications=publickey" ) ;

		auto _set_identity_agent = [ & ](){

			if( args.identityAgent.isEmpty() ){

				if( m_sshAuthSock.isEmpty() ){

					utility::debug() << "Sshfs: identityAgent Not Set" ;
					m_environment.remove( "SSH_AUTH_SOCK" ) ;
				}else{
					utility::debug() << "Sshfs: From Env, Setting Env Variable Of: SSH_AUTH_SOCK=" + m_sshAuthSock ;

					m_environment.insert( "SSH_AUTH_SOCK",m_sshAuthSock ) ;
				}
			}else{
				utility::debug() << "Sshfs: Setting Env Variable Of: SSH_AUTH_SOCK=" + args.identityAgent ;
				m_environment.insert( "SSH_AUTH_SOCK",args.identityAgent ) ;
			}
		} ;

		if( args.identityFile.isEmpty() ){

			_set_identity_agent() ;
		}else{
			_set_identity_agent() ;

			fuseOptions.add( "IdentityFile=" + args.identityFile ) ;
		}
	}else{
		fuseOptions.add( "PreferredAuthentications=password" ) ;
		fuseOptions.add( "password_stdin" ) ;
	}
}

engines::engine::error sshfs::errorCode( const QString& e ) const
{
	if( e.contains( "invalid argument" ) ){

		return engines::engine::error::Failed ;
	}else{
		return engines::engine::errorCode( e ) ;
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
