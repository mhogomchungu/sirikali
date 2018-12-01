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
#include "commandOptions.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.autoMountsOnCreate  = true ;
	s.hasGUICreateOptions = false ;
	s.setsCipherPath      = true ;

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

QString sshfs::command( const engines::engine::cmdArgsList& args ) const
{
	commandOptions m( args,this->name(),this->name() ) ;

	auto fuseOptions = m.fuseOpts() ;

	if( !args.opt.key.isEmpty() ){

		fuseOptions.add( "password_stdin" ) ;
	}

	if( fuseOptions.doesNotContain( "StrictHostKeyChecking=" ) ){

		fuseOptions.addPair( "StrictHostKeyChecking","no" ) ;
	}

	auto exeOptions = m.exeOptions() ;

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;
	}

	QString s = "%1 %2 %3 %4 %5" ;

	return s.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      fuseOptions.get() ) ;
}

engines::engine::status sshfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return engines::engine::status::sshfsBadPassword ;

	}else if( e.contains( "winfsp" ) ){

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
	return this->baseInstalledVersionString( "--version",true,2,0 ) ;
}

void sshfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	Q_UNUSED( parent ) ;
	Q_UNUSED( function ) ;
}
