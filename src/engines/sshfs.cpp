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

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.autoMountsOnCreate  = true ;
	s.hasGUICreateOptions = false ;
	s.setsCipherPath      = true ;
	s.supportsMountPathsOnWindows = true ;

	s.configFileArgument  = QString() ;

	s.configFileNames = QStringList{} ;

	s.fuseNames = QStringList{ "fuse.sshfs" } ;
	s.names     = QStringList{ "sshfs" } ;

	s.notFoundCode = engines::engine::status::sshfsNotFound ;

	return s ;
}

sshfs::sshfs() : engines::engine( _setOptions() )
{
}

engines::engine::args sshfs::command( const engines::engine::cmdArgsList& args ) const
{
	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	auto fuseOptions = m.fuseOpts() ;
	auto exeOptions  = m.exeOptions() ;

	if( !args.opt.key.isEmpty() ){

		fuseOptions.add( "password_stdin" ) ;
	}

	if( fuseOptions.doesNotContain( "StrictHostKeyChecking=" ) ){

		fuseOptions.addPair( "StrictHostKeyChecking","no" ) ;
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

	auto& env = utility::globalEnvironment::instance() ;

	if( fuseOptions.contains( "IdentityAgent" ) ){

		auto m = "IdentityAgent=" ;

		auto n = fuseOptions.extractStartsWith( m ).replace( m,"" ) ;

		env.insert( "SSH_AUTH_SOCK",n ) ;
	}else{
		auto m = qgetenv( "SSH_AUTH_SOCK" ) ;

		if( m.isEmpty() ){

			env.remove( "SSH_AUTH_SOCK" ) ;
		}else{
			env.insert( "SSH_AUTH_SOCK",m ) ;
		}
	}

	QString s = "%1 %2 %3 %4 %5" ;

	auto cmd = s.arg( args.exe,
			  exeOptions.get(),
			  args.cipherFolder,
			  args.mountPoint,
			  fuseOptions.get() ) ;

	return { args,m,cmd } ;
}

engines::engine::error sshfs::errorCode( const QString& e ) const
{
	if( e.contains( "has been started" ) ){

		return engines::engine::error::Success ;

	}else if( utility::containsAtleastOne( e,"ssh:","read:","Cannot create WinFsp-FUSE file system" ) ){

		return engines::engine::error::Failed ;
	}else{
		return engines::engine::error::Continue ;
	}
}

engines::engine::status sshfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "cygfuse: initialization failed: winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString sshfs::setPassword( const QString& e ) const
{
	auto m = e ;

	/*
	 * On my linux box, sshfs prompts six times when entered password is wrong before
	 * giving up, here, we simulate replaying the password 10 times hoping it will be
	 * enough for sshfs.
	 */
	for( int i = 0 ; i < 9 ; i++ ){

		m += "\n" + m ;
	}

	return m ;
}

QString sshfs::installedVersionString() const
{
	if( m_version.isEmpty() ){

		m_version = this->baseInstalledVersionString( "--version",true,2,0 ) ;
	}

	return m_version ;
}

void sshfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	Q_UNUSED( parent ) ;
	Q_UNUSED( function ) ;
}
