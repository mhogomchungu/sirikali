/*
 *
 *  Copyright (c) 2014-2015
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

#include "plugins.h"
#include "siritask.h"
#include "mountinfo.h"
#include "win.h"
#include "settings.h"
#include "sirikali.h"
#include "processManager.h"

#include <QDir>
#include <QString>
#include <QDebug>
#include <QFile>

bool siritask::deleteMountFolder( const QString& m )
{
	if( settings::instance().reUseMountPoint() ){

		if( utility::platformIsWindows() ){

			utility::removeFolder( m,5 ) ;
			utility::createFolder( m ) ;
		}

		return false ;
	}else{
		return utility::removeFolder( m,5 ) ;
	}
}

static engines::engine::exe_args _cmd_args_1( QString e )
{
	e.remove( 0,1 ) ;

	for( int i = 0 ; i < e.size() ; i++ ){

		if( e.at( i ) == '\"' ){

			auto a = e.mid( 0,i ) ;

			auto b = engines::executableFullPath( a ) ;

			if( !b.isEmpty() ){

				return { b,utility::split( e.mid( i + 1 ),' ' ) } ;
			}
		}
	}

	return {} ;
}

static engines::engine::exe_args _cmd_args( const QString& e )
{
	if( e.isEmpty() ){

		return {} ;
	}

	if( e.startsWith( "\"" ) ){

		return _cmd_args_1( e ) ;
	}

	auto args = utility::split( e,' ' ) ;
	auto exe = engines::executableFullPath( args.at( 0 ) ) ;

	if( exe.isEmpty() ){

		return {} ;
	}

	args.removeFirst() ;

	return { std::move( exe ),std::move( args ) } ;
}

static void _run_command( const engines::engine::exe_args_const& exe,
			  const QByteArray& password = QByteArray() )
{
	if( password.isEmpty() ){

		utility::unwrap( utility::Task::run( exe.exe,exe.args ) ) ;
	}else{
		auto e = utility::systemEnvironment() ;

		e.insert( settings::instance().environmentalVariableVolumeKey(),password ) ;

		utility::unwrap( Task::run( [ & ](){ utility::Task( exe.exe,exe.args,e,[](){} ) ; } ) ) ;
	}
}

struct run_command{

	const QString& command ;
	const QString& cipherFolder ;
	const QString& plainFolder ;
	const QString& volumeType ;
	const QString& commandType ;
	const QByteArray& password ;
};

static void _run_command( const run_command& e )
{
	if( e.command.isEmpty() ){

		return ;
	}

	auto s = _cmd_args( e.command ) ;

	if( s.exe.isEmpty() ){

		utility::debug() << "Failed to find \"" + e.commandType + "\" command : " + e.command ;
	}else{
		s.args.append( e.cipherFolder ) ;
		s.args.append( e.plainFolder ) ;
		s.args.append( e.volumeType ) ;

		_run_command( s,e.password ) ;
	}
}

static void _run_preUnmountCommand( const engines::engine::unMount& e )
{
	auto cmd = settings::instance().preUnMountCommand() ;

	int timeOut = 10000 ;

	if( !cmd.isEmpty() ){

		QStringList s ;

		s.append( e.cipherFolder ) ;
		s.append( e.mountPoint ) ;
		s.append( e.engine.name() ) ;

		utility::Task::run( cmd,s,timeOut,false ).get() ;
	}
}

static engines::engine::cmdStatus _unmount( const engines::engine::unMount& e )
{
	const auto& engine = e.engine ;

	if( engine.unknown() ){

		return { engines::engine::status::unknown,engine } ;
	}

	if( utility::platformIsWindows() || engine.runsInForeGround() ){

		auto s = processManager::get().remove( e.mountPoint ) ;

		if( s.success() ){

			return { engines::engine::status::success,engine } ;
		}else{
			return { engines::engine::status::failedToUnMount,engine } ;
		}
	}else{
		if( engine.requiresPolkit() && !utility::enablePolkit() ){

			return { engines::engine::status::failedToStartPolkit,engine } ;
		}

		auto s = utility::unwrap( Task::run( [ & ](){

			_run_preUnmountCommand( e ) ;

			return engine.unmount( e ) ;
		} ) ) ;

		return { s,engine } ;
	}
}

engines::engine::cmdStatus siritask::encryptedFolderUnMount( const siritask::unmount& e )
{
	const auto& fav = favorites::instance().readFavorite( e.cipherFolder,e.mountPoint ) ;

	const auto& fileSystem = e.engine.name() ;

	if( fav.has_value() ){

		const auto& m = fav.value() ;

		auto& a = e.cipherFolder ;
		auto& b = e.mountPoint ;

		_run_command( { m.preUnmountCommand,a,b,fileSystem,"pre unmount",QByteArray() } ) ;

		auto s = _unmount( { e.cipherFolder,e.mountPoint,e.engine,e.numberOfAttempts } ) ;

		if( s.success() ){

			_run_command( { m.postUnmountCommand,a,b,fileSystem,"post unmount",QByteArray() } ) ;
		}

		return s ;
	}else{
		return _unmount( { e.cipherFolder,e.mountPoint,e.engine,e.numberOfAttempts } ) ;
	}
}

struct cmd_args{

	const engines::engine& engine ;
	bool create ;
	const engines::engine::cmdArgsList& opts ;
	const QByteArray& password ;
};

struct run_task{

	run_task( const engines::engine::args& args,const cmd_args& e ) :
		args( args ),engine( e.engine ),password( e.password ),
		opts( e.opts ),create( e.create )
	{
	}
	const engines::engine::args& args ;
	const engines::engine& engine ;
	const QByteArray& password ;
	const engines::engine::cmdArgsList& opts ;
	bool create ;
};

static utility::Task _run_task_0( const run_task& e )
{
	if( utility::platformIsWindows() || e.engine.runsInForeGround() ){

		return processManager::get().run( { e.create,e.args,e.opts,e.engine,e.password } ) ;
	}else{
		const auto& s = e.engine.getProcessEnvironment() ;

		utility::Task::moreOptions opts( e.engine.requiresPolkit(),
						 e.engine.likeSsh(),
						 e.engine.allowLogging( e.args.cmd_args ) ) ;

		return utility::Task( e.args.cmd,
				      e.args.cmd_args,
				      -1,
				      s,
				      e.password,
				      [](){},
				      opts ) ;
	}
}

static utility::Task _run_task( const run_task& e )
{
	const auto& mm = favorites::instance().readFavorite( e.opts.cipherFolder,e.opts.mountPoint ) ;

	if( mm.has_value() ){

		const auto& m = mm.value() ;

		auto& a = e.opts.cipherFolder ;
		auto& b = e.opts.mountPoint ;
		const auto& c = e.engine.name() ;

		QByteArray key ;

		if( settings::instance().allowExternalToolsToReadPasswords() ){

			key = e.password ;
		}

		_run_command( { m.preMountCommand,a,b,c,"pre mount",key } ) ;

		auto s = _run_task_0( e ) ;

		if( s.success() ){

			_run_command( { m.postMountCommand,a,b,c,"post mount",key } ) ;
		}

		return s ;
	}else{
		return _run_task_0( e ) ;
	}
}

struct build_config_path{

	const engines::engine& engine ;
	const QString& configFilePath ;
};

static engines::engine::cmdStatus _cmd( const cmd_args& e )
{
	const auto& engine = e.engine ;

	auto cmd = engine.command( e.password,e.opts,e.create ) ;

	auto s = _run_task( { cmd,e } ) ;

	if( s.success() ){

		return { engines::engine::status::success,engine } ;
	}else{
		if( utility::platformIsWindows() ){

			if( processManager::backEndTimedOut( s.stdOut() ) ){

				return { engines::engine::status::backendTimedOut,engine } ;
			}
		}

		auto ss = s.stdError().isEmpty() ? s.stdOut() : s.stdError() ;

		auto n = engine.errorCode( ss,s.exitCode() ) ;

		return { n,engine,ss } ;
	}
}

struct result_create_mp{

	engines::engine::status status ;
	bool deleteMp ;
} ;

static result_create_mp _create_mount_point( const engines::engine& engine,
					     const engines::engine::cmdArgsList& opt,
					     bool reUseMP )
{
	QString m ;

	auto isDriveLetter = utility::isDriveLetter( opt.mountPoint ) ;

	if( isDriveLetter ){

		m = "1. Using a drive letter: " + opt.mountPoint ;
	}else{
		m = "1. Creating mount point at: " + opt.mountPoint ;
	}

	auto raii = utility2::make_raii( [ & ](){ utility::debug() << m ; } ) ;

	if( isDriveLetter ){

		return { engines::engine::status::success,false } ;

	}else if( utility::pathExists( opt.mountPoint ) ){

		if( reUseMP ){

			m += "\n2. Mount point exists, using it" ;

		}else if( settings::instance().reUseMountPoint() ){

			m += "\n2. Mount point exists, re using it" ;
		}else{
			m += "\n2. Error: Mount point already exists" ;

			return { engines::engine::status::failedToCreateMountPoint,false } ;
		}

		return { engines::engine::status::success,false } ;
	}else{
		m += "\n2. Mount point does not exist, creating it" ;

		if( engine.createMountPath( opt.mountPoint ) ){

			m += "\n3. Mount point created successfully" ;

			return { engines::engine::status::success,true } ;
		}else{
			m += "\n3. Error: Failed to create mount point" ;

			return { engines::engine::status::failedToCreateMountPoint,true } ;
		}
	}
}

static engines::engine::cmdStatus _mount( const siritask::mount& s )
{
	const auto& Engine = s.engine ;
	const auto& engine = Engine.get() ;

	auto opt = s.options ;

	opt.configFilePath = Engine.configFilePath() ;
	opt.cipherFolder   = Engine.cipherFolder() ;

	engine.updateOptions( opt,false ) ;

	auto mm = engine.passAllRequirenments( opt ) ;

	if( mm != engines::engine::status::success ){

		return { mm,engine } ;
	}

	if( engine.autoCreatesMountPoint() ){

		utility::removeFolder( opt.mountPoint,5 ) ;

		auto e = _cmd( { engine,false,opt,opt.key } ) ;

		if( e == engines::engine::status::success ){

			engine.updateVolumeList( opt ) ;
		}

		return e ;
	}else{
		auto e = [ & ]()->engines::engine::cmdStatus{

			if( engine.backendRequireMountPath() ){

				auto m = _create_mount_point( engine,opt,s.reUseMP ) ;

				if( m.status != engines::engine::status::success ){

					return { m.status,engine } ;
				}

				auto e = _cmd( { engine,false,opt,opt.key } ) ;

				if( e != engines::engine::status::success && m.deleteMp ){

					engine.deleteFolder( opt.mountPoint,5 ) ;
				}

				return e ;
			}else{
				return _cmd( { engine,false,opt,opt.key } ) ;
			}
		}() ;

		if( e == engines::engine::status::success ){

			engine.updateVolumeList( opt ) ;
		}

		return e ;
	}
}

static engines::engine::cmdStatus _create( const siritask::create& s )
{
	const auto& engine = s.engine ;
	auto opt           = s.options ;

	engine.updateOptions( opt,true ) ;

	auto mm = engine.passAllRequirenments( opt ) ;

	if( mm != engines::engine::status::success ){

		return { mm,engine } ;
	}

	if( !engine.createCipherPath( opt.cipherFolder ) ){

		return { engines::engine::status::failedToCreateMountPoint,engine } ;
	}

	if( engine.backendRequireMountPath() ){

		auto m = _create_mount_point( engine,opt,false ) ;

		if( m.status != engines::engine::status::success ){

			engine.deleteFolder( opt.cipherFolder ) ;

			return { engines::engine::status::failedToCreateMountPoint,engine } ;
		}
	}

	auto e = _cmd( { engine,true,opt,engine.setPassword( opt.key ) } ) ;

	if( e == engines::engine::status::success ){

		engine.updateVolumeList( opt ) ;

		if( !engine.autoMountsOnCreate() ){

			engines::engineWithPaths s{ engine,opt.cipherFolder,opt.configFilePath } ;

			auto e = siritask::encryptedFolderMount( { opt,true,s } ) ;

			if( e != engines::engine::status::success ){

				if( e.engine().backendRequireMountPath() ){

					engine.deleteFolder( opt.cipherFolder ) ;
					engine.deleteFolder( opt.mountPoint ) ;
				}else{
					engine.deleteFolder( opt.cipherFolder ) ;
				}
			}
		}
	}else{
		if( e.engine().backendRequireMountPath() ){

			engine.deleteFolder( opt.cipherFolder ) ;
			engine.deleteFolder( opt.mountPoint ) ;
		}else{
			engine.deleteFolder( opt.cipherFolder ) ;
		}
	}

	return e ;
}

static void _warning( const QString& e )
{
	auto a = "ERROR!!\n " + e + "siritask::encryptedFolderMount is" ;
	auto b = "running from a background thread" ;

	utility::debug() << a + b ;
}

engines::engine::cmdStatus siritask::encryptedFolderCreate( const siritask::create& e )
{
	if( utility::platformIsWindows() ){

		if( utility::miscOptions::instance().runningOnGUIThread() ){

			return _create( e ) ;
		}else{
			/*
			 * We should not take this path
			 */
			_warning( "siritask::encryptedFolderMount" ) ;

			return _create( e ) ;
		}
	}else{
		auto& s = Task::run( [ & ](){ return _create( e ) ; } ) ;

		return utility::unwrap( s ) ;
	}
}

static void _run_command_on_mount( const siritask::mount& e )
{
	const auto& opt  = e.options ;
	const auto& type = e.engine->name() ;

	auto s = settings::instance().runCommandOnMount() ;

	if( !s.isEmpty() ){

		QStringList opts = { opt.cipherFolder,opt.mountPoint,type } ;

		Task::exec( [ = ](){

			utility::logger logger ;

			logger.showText( s,opts ) ;

			auto r = [ & ](){

				if( settings::instance().allowExternalToolsToReadPasswords() ){

					auto e = utility::systemEnvironment() ;
					auto& m = settings::instance() ;

					e.insert( m.environmentalVariableVolumeKey(),opt.key ) ;

					return Task::process::run( s,opts,-1,{},e ).get() ;
				}else{
					const auto& e = utility::systemEnvironment() ;

					return Task::process::run( s,opts,-1,{},e ).get() ;
				}
			}() ;

			logger.showText( r ) ;
		} ) ;
	}
}

engines::engine::cmdStatus siritask::encryptedFolderMount( const engines::engine::cmdArgsList& s )
{
	return siritask::encryptedFolderMount( { s,false,{ s.cipherFolder,s.configFilePath } } ) ;
}

engines::engine::cmdStatus siritask::encryptedFolderMount( const siritask::mount& e )
{
	auto s = [ & ](){

		if( utility::platformIsWindows() ){

			if( utility::miscOptions::instance().runningOnGUIThread() ){

				return _mount( e ) ;
			}else{
				/*
				 * We should not take this path
				 */
				_warning( "siritask::encryptedFolderMount" ) ;
				return _mount( e ) ;
			}
		}else{
			auto& s = Task::run( [ & ](){

				return _mount( e ) ;
			} ) ;

			return utility::unwrap( s ) ;
		}
	}() ;

	if( s == engines::engine::status::success ){

		_run_command_on_mount( e ) ;
	}

	return s ;
}
