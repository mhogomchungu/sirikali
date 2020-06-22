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
#include "options.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = false ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
	s.requiresPolkit        = true ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.releaseURL            = "https://api.github.com/repos/mhogomchungu/ecryptfs-simple/releases" ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "ecryptfs-simple" ;
	s.incorrectPasswordText = "error: mount failed" ;
	s.configFileArgument    = "--config" ;
	s.configFileNames       = QStringList{ ".ecryptfs.config","ecryptfs.config" } ;
	s.fuseNames             = QStringList{ "ecryptfs" } ;
	s.names                 = QStringList{ "ecryptfs" } ;
	s.versionInfo           = { { "--version",true,1,0 } } ;

	s.notFoundCode = engines::engine::status::ecryptfs_simpleNotFound ;

	return s ;
}

#ifdef Q_OS_LINUX

#include <sys/stat.h>

static bool _requiresPolkit( const engines::engine& engine )
{
	auto e = engine.executableFullPath() ;

	struct stat st ;

	stat( e.toLatin1().constData(),&st ) ;

	bool root_owner = st.st_uid == 0 ;
	bool is_suid    = st.st_mode & S_ISUID ;

	return !( root_owner && is_suid ) ;
}

#else

static bool _requiresPolkit( const engines::engine& engine )
{
	Q_UNUSED( engine )
	return false ;
}

#endif

ecryptfs::ecryptfs() :
	engines::engine( _setOptions() ),
	m_requirePolkit( _requiresPolkit( *this ) ),
	m_exeSUFullPath( [](){ return engines::executableFullPath( "su" ) ; } )
{
}

void ecryptfs::updateOptions( engines::engine::cmdArgsList::options& opt,bool creating ) const
{
	Q_UNUSED( creating )

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
	auto s = siritask::unmountVolume( cmd(),true ) ;

	if( s && s.value().success() ){

		return true ;
	}else{		
		return false ;
	}
}

siritask::exe ecryptfs::wrapSU( const QString& s ) const
{
	const auto& su = m_exeSUFullPath.get() ;

	if( su.isEmpty() ){

		return {} ;
	}else{
		return { su,{ "-","-c",s } } ;
	}
}

engines::engine::status ecryptfs::unmount( const QString& cipherFolder,
					   const QString& mountPoint,
					   int maxCount ) const
{
	Q_UNUSED( mountPoint )

	auto cmd = [ & ]()->siritask::exe{

		auto exe = this->executableFullPath() ;

		if( utility::useSiriPolkit() ){

			auto s = this->wrapSU( exe ) ;

			s.args[ 2 ].append( " -k " + cipherFolder ) ;

			return s ;
		}else{
			return { exe,{ "-k",cipherFolder } } ;
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

	engines::engine::commandOptions m( args,QString() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.opt.boolOptions.unlockInReadOnly ){

		exeOptions.add( "--readonly" ) ;
	}

	if( args.create ){

		if( args.opt.createOptions.isEmpty() ){

			exeOptions.add( "-o",ecryptfscreateoptions::defaultCreateOptions() ) ;
		}else{
			exeOptions.add( "-o",args.opt.createOptions ) ;
		}
	}else{
		exeOptions.add( "-o","key=passphrase" ) ;
	}

	if( !args.opt.mountOptions.isEmpty() ){

		exeOptions.add( "-o",args.opt.mountOptions ) ;
	}

	exeOptions.add( "-a",this->configFileArgument() + "=" + args.opt.configFilePath ) ;

	exeOptions.add( args.cipherFolder,args.mountPoint ) ;

	if( utility::useSiriPolkit() ){

		auto s = this->wrapSU( args.exe ) ;

		s.args[ 2 ].append( " " + exeOptions.get().join( ' ' ) ) ;

		return { args,m,s.exe,s.args } ;
	}else{
		return { args,m,args.exe,exeOptions.get() } ;
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

void ecryptfs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	ecryptfscreateoptions::instance( *this,s ) ;
}

void ecryptfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableCheckBox = false ;
	ee.enableConfigFile = false ;
	ee.enableIdleTime = false ;
	ee.enableMountOptions = false ;
	ee.enableKeyFile = false ;

	e.ShowUI() ;
}
