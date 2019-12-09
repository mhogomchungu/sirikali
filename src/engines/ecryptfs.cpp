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

#include "ecryptfs.h"
#include "ecryptfscreateoptions.h"
#include "../siritask.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = false ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.requiresPolkit        = true ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "ecryptfs-simple" ;
	s.incorrectPasswordText = "error: mount failed" ;
	s.configFileArgument    = "--config" ;
	s.configFileNames       = QStringList{ ".ecryptfs.config","ecryptfs.config" } ;
	s.fuseNames             = QStringList{ "ecryptfs" } ;
	s.names                 = QStringList{ "ecryptfs" } ;

	s.notFoundCode = engines::engine::status::ecryptfs_simpleNotFound ;

	return s ;
}

#ifdef Q_OS_LINUX

#include <sys/stat.h>

static bool _requiresPolkit()
{
	auto e = utility::executableFullPath( "ecryptfs-simple" ) ;

	struct stat st ;

	stat( e.toLatin1().constData(),&st ) ;

	bool root_owner = st.st_uid == 0 ;
	bool is_suid    = st.st_mode & S_ISUID ;

	return !( root_owner && is_suid ) ;
}

#else

static bool _requiresPolkit()
{
	return false ;
}

#endif

ecryptfs::ecryptfs() :
	engines::engine( _setOptions() ),
	m_requirePolkit( _requiresPolkit() ),
	m_version( [ this ]{ return this->baseInstalledVersionString( "--version",true,1,0 ) ; } )
{
}

void ecryptfs::updateOptions( engines::engine::options& opt ) const
{
	if( opt.configFilePath.isEmpty() ){

		opt.configFilePath = opt.cipherFolder + "/" + this->configFileName() ;
	}
}

bool ecryptfs::requiresPolkit() const
{
	return m_requirePolkit ;
}

template< typename Function >
static bool _unmount_ecryptfs_( Function cmd )
{
	auto s = siritask::unmountVolume( cmd(),QString(),true ) ;

	if( s && s.value().success() ){

		return true ;
	}else{		
		return false ;
	}
}

engines::engine::status ecryptfs::unmount( const QString& cipherFolder,
					   const QString& mountPoint,
					   int maxCount ) const
{
	Q_UNUSED( mountPoint )

	auto cmd = [ & ](){

		auto exe = utility::executableFullPath( "ecryptfs-simple" ) ;

		auto s = exe + " -k " + cipherFolder ;

		if( utility::useSiriPolkit() ){

			return utility::wrap_su( s ) ;
		}else{
			return s ;
		}
	} ;

	if( _unmount_ecryptfs_( cmd ) ){

		return engines::engine::status::success ;
	}else{
		for( int i = 1 ; i < maxCount ; i++ ){

			utility::Task::waitForOneSecond() ;

			if( _unmount_ecryptfs_( cmd ) ){

				return engines::engine::status::success ;
			}
		}

		return engines::engine::status::failedToUnMount ;
	}
}

engines::engine::args ecryptfs::command( const QByteArray& password,
					 const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	auto e = QString( "%1 %2 -a %3 %4 %5" ) ;

	engines::engine::commandOptions m( args,QString() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.opt.ro ){

		exeOptions.add( "--readonly" ) ;
	}

	if( args.create ){

		if( args.opt.createOptions.isEmpty() ){

			exeOptions.add( ecryptfscreateoptions::defaultCreateOptions() ) ;
		}else{
			exeOptions.add( args.opt.createOptions ) ;
		}
	}else{
		exeOptions.add( "-o","key=passphrase" ) ;
	}

	if( !args.opt.mountOptions.isEmpty() ){

		exeOptions.add( "-o",args.opt.mountOptions ) ;
	}

	auto s = e.arg( args.exe,
			exeOptions.get(),
			args.configFilePath,
			args.cipherFolder,
			args.mountPoint ) ;

	if( utility::useSiriPolkit() ){

		return { args,m,utility::wrap_su( s ) } ;
	}else{
		return { args,m,s } ;
	}
}

engines::engine::status ecryptfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( "Operation not permitted" ) ){

		return engines::engine::status::failedToStartPolkit ;

	}else if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::ecryptfsBadPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

const QString& ecryptfs::installedVersionString() const
{
	return m_version.get() ;
}

void ecryptfs::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	ecryptfscreateoptions::instance( parent,std::move( function ) ) ;
}

