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
	s.likeSsh               = false ;
	s.requiresPolkit        = true ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.releaseURL            = "https://api.github.com/repos/mhogomchungu/ecryptfs-simple/releases" ;
	s.passwordFormat        = "%{password}" ;
	s.incorrectPasswordText = "error: mount failed" ;
	s.configFileArgument    = "--config=%{configFilePath}" ;
	s.executableNames       = QStringList{ "ecryptfs-simple" } ;

	s.configFileNames       = QStringList{ ".ecryptfs.config","ecryptfs.config" } ;
	s.fuseNames             = QStringList{ "ecryptfs" } ;
	s.names                 = QStringList{ "ecryptfs" } ;
	s.versionInfo           = { { "--version",true,1,0 } } ;

	s.notFoundCode = engines::engine::status::ecryptfs_simpleNotFound ;

	s.createControlStructure = "%{createOptions} %{cipherFolder} %{mountPoint}" ;
	s.mountControlStructure  = "%{mountOptions} %{cipherFolder} %{mountPoint}" ;

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
	m_exeSUFullPath( [](){ return engines::executableNotEngineFullPath( "su" ) ; } )
{
}

void ecryptfs::updateOptions( engines::engine::cmdArgsList& opt,bool creating ) const
{
	QStringList createOptions ;
	QStringList mountOptions ;

	if( creating ){

		if( opt.createOptions.isEmpty() ){

			createOptions.append( "-o " + ecryptfscreateoptions::defaultCreateOptions() ) ;
		}else{
			createOptions.append( "-o " + opt.createOptions.join( ',' ) ) ;
		}
	}else{
		if( opt.mountOptions.isEmpty() ){

			mountOptions.append( "-o key=passphrase" ) ;
		}else{
			mountOptions.append( "-o " + opt.mountOptions.join( ',' ) ) ;
		}
	}

	if( opt.configFilePath.isEmpty() ){

		opt.configFilePath = opt.cipherFolder + "/" + this->configFileName() ;
	}

	if( opt.boolOptions.unlockInReadOnly ){

		mountOptions.append( "--readonly" ) ;
	}

	mountOptions.append( "-a" ) ;

	opt.mountOptions  = mountOptions ;
	opt.createOptions = createOptions ;
}

bool ecryptfs::requiresPolkit() const
{
	return m_requirePolkit ;
}

engines::engine::exe_args ecryptfs::wrapSU( const QString& s ) const
{
	const auto& su = m_exeSUFullPath.get() ;

	if( su.isEmpty() ){

		return {} ;
	}else{
		return { su,{ "-","-c",s } } ;
	}
}

engines::engine::status ecryptfs::unmount( const engines::engine::unMount& e ) const
{
	auto usePolkit = utility::miscOptions::instance().usePolkit() ;

	auto cmd = [ & ]()->engines::engine::exe_args{

		auto exe = this->executableFullPath() ;

		if( usePolkit ){

			return this->wrapSU( exe + " -k " + e.cipherFolder ) ;
		}else{
			return { exe,{ "-k",e.cipherFolder } } ;
		}
	} ;

	if( this->unmountVolume( cmd(),usePolkit ) ){

		return engines::engine::status::success ;
	}else{
		for( int i = 1 ; i < e.numberOfAttempts ; i++ ){

			utility::Task::waitForOneSecond() ;

			if( this->unmountVolume( cmd(),usePolkit ) ){

				return engines::engine::status::success ;
			}
		}

		return engines::engine::status::failedToUnMount ;
	}
}

engines::engine::args ecryptfs::command( const QByteArray& password,
					 const engines::engine::cmdArgsList& args,
					 bool create ) const
{
	auto m = engines::engine::command( password,args,create ) ;

	if( utility::miscOptions::instance().usePolkit() ){

		auto exe = this->executableFullPath() + " " + m.cmd_args.join( ' ' ) ;

		auto s = this->wrapSU( exe ) ;

		engines::engine::args e( m ) ;

		e.cmd      = s.exe ;
		e.cmd_args = s.args ;

		return e ;
	}else{
		return m ;
	}
}

engines::engine::status ecryptfs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( "Operation not permitted" ) ){

		return engines::engine::status::failedToStartPolkit ;
	}else{
		return engines::engine::errorCode( e,s ) ;
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
