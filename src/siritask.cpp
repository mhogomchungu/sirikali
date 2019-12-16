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

#include <QDir>
#include <QString>
#include <QDebug>
#include <QFile>

using Opts = engines::engine::options ;
using Engs = engines::engine ;

static bool _create_folder( const QString& m )
{
	if( utility::pathExists( m ) ){

		return settings::instance().reUseMountPoint() ;
	}else{
		return utility::createFolder( m ) ;
	}
}

static QString _makePath( const QString& e )
{
	return utility::Task::makePath( e ) ;
}

static bool _illegal_path( const Opts& opts,const engines::engine& engine )
{
	if( engine.requiresPolkit() && utility::useSiriPolkit() ){

		if( engine.backendRequireMountPath() ){

			return opts.cipherFolder.contains( " " ) || opts.plainFolder.contains( " " ) ;
		}else {
			return opts.cipherFolder.contains( " " ) ;
		}
	}else{
		return false ;
	}
}

template< typename ... T >
static void _deleteFolders( const T& ... m )
{
	QDir e ;

	for( const auto& it : { m ... } ){

		utility::removeFolder( it,1 ) ;
	}
}

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

static QString _cmd_args_1( QString e )
{
	e.remove( 0,1 ) ;

	for( int i = 0 ; i < e.size() ; i++ ){

		if( e.at( i ) == '\"' ){

			auto a = e.mid( 0,i ) ;

			auto b = utility::executableFullPath( a ) ;

			if( !b.isEmpty() ){

				return utility::Task::makePath( b ) + " " + e.mid( i + 1 ) ;
			}
		}
	}

	return {} ;
}

static QString _cmd_args( const QString& e )
{
	if( e.isEmpty() ){

		return {} ;
	}

	if( e.startsWith( "\"" ) ){

		return _cmd_args_1( e ) ;
	}

	auto a = utility::split( e,' ' ) ;
	auto b = utility::executableFullPath( a.at( 0 ) ) ;

	if( b.isEmpty() ){

		return {} ;
	}

	a.removeFirst() ;

	return utility::Task::makePath( b ) + " " + a.join( ' ' ) ;
}

utility::task_result siritask::unmountVolume( const QString& exe,const QString& mountPoint,bool usePolkit )
{
	auto e = _cmd_args( settings::instance().preUnMountCommand() ) ;

	int timeOut = 10000 ;

	if( e.isEmpty() ){

		auto& s = utility::Task::run( exe + " " + mountPoint,timeOut,usePolkit ) ;

		return utility::unwrap( s ) ;
	}else{
		auto& s = utility::Task::run( e + " " + mountPoint,timeOut,false ) ;

		auto m = utility::unwrap( s ) ;

		if( m.success() ){

			auto& m = utility::Task::run( exe + " " + mountPoint,timeOut,usePolkit ) ;

			return utility::unwrap( m ) ;
		}else{
			return {} ;
		}
	}
}

engines::engine::status siritask::unmountVolume( const QString& mountPoint,
						 const QString& unMountCommand,
						 int maxCount )
{
	auto _unmount = []( const QString& cmd,const QString& mountPoint ){

		auto s = siritask::unmountVolume( cmd,mountPoint,false ) ;

		return s && s.value().success() ;
	} ;

	auto cmd = [ & ]()->QString{

		if( unMountCommand.isEmpty() ){

			if( utility::platformIsOSX() ){

				return "umount" ;
			}else{
				return "fusermount -u" ;
			}
		}else{
			return unMountCommand ;
		}
	}() ;

	if( _unmount( cmd,mountPoint ) ){

		return engines::engine::status::success ;
	}else{
		for( int i = 1 ; i < maxCount ; i++ ){

			utility::Task::waitForOneSecond() ;

			if( _unmount( cmd,mountPoint ) ){

				return engines::engine::status::success ;
			}
		}

		return engines::engine::status::failedToUnMount ;
	}
}

static void _run_command( const QString& command,const QByteArray& password = QByteArray() )
{
	if( password.isEmpty() ){

		utility::unwrap( utility::Task::run( command ) ) ;
	}else{
		auto e = utility::systemEnvironment() ;

		e.insert( settings::instance().environmentalVariableVolumeKey(),password ) ;

		utility::unwrap( Task::run( [ & ](){ utility::Task( command,e,[](){} ) ; } ) ) ;
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

	auto exe = _cmd_args( e.command ) ;

	if( exe.isEmpty() ){

		utility::debug() << "Failed to find \"" + e.commandType + "\" command : " + e.command ;
	}else{
		auto m = QString( "%1 %2 %3 %4" ).arg( exe,e.cipherFolder,e.plainFolder,e.volumeType ) ;

		_run_command( m,e.password ) ;
	}
}

static engines::engine::cmdStatus _unmount( const siritask::unmount& e )
{
	const auto& engine = engines::instance().getByName( e.fileSystem ) ;

	if( engine.unknown() ){

		return { engines::engine::status::unknown,engine } ;
	}

	if( utility::platformIsWindows() ){

		auto m = engine.windowsUnMountCommand() ;

		auto s = SiriKali::Windows::unmount( m,_makePath( e.mountPoint ) ) ;

		if( s.success() ){

			return { engines::engine::status::success,engine } ;
		}else{
			return { engines::engine::status::failedToUnMount,engine } ;
		}
	}else{
		if( engine.requiresPolkit() && !utility::enablePolkit() ){

			return { engines::engine::status::failedToStartPolkit,engine } ;
		}

		auto a = _makePath( e.cipherFolder ) ;
		auto b = _makePath( e.mountPoint ) ;

		auto s = utility::unwrap( Task::run( [ & ](){

			return engine.unmount( a,b,e.numberOfAttempts ) ;
		} ) ) ;

		return { s,engine } ;
	}
}

engines::engine::cmdStatus siritask::encryptedFolderUnMount( const siritask::unmount& e )
{
	auto fav = favorites::instance().readFavorite( e.cipherFolder,e.mountPoint ) ;

	if( fav.has_value() ){

		const auto& m = fav.value() ;

		auto a = utility::Task::makePath( e.cipherFolder ) ;
		auto b = utility::Task::makePath( e.mountPoint ) ;

		_run_command( { m.preUnmountCommand,a,b,e.fileSystem,"pre unmount",QByteArray() } ) ;

		auto s = _unmount( e ) ;

		if( s.success() ){

			_run_command( { m.postUnmountCommand,a,b,e.fileSystem,"post unmount",QByteArray() } ) ;
		}

		return s ;
	}else{
		return _unmount( e ) ;
	}
}

struct cmd_args{

	const engines::engine& engine ;
	bool create ;
	const engines::engine::options& opts ;
	const QByteArray& password ;
	const QString& configFilePath ;
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
	const engines::engine::options& opts ;
	bool create ;
};

static utility::Task _run_task_0( const run_task& e )
{
	if( utility::platformIsWindows() ){

		return SiriKali::Windows::run( { e.create,e.args,e.opts,e.engine,e.password } ) ;
	}else{
		const auto& s = e.engine.getProcessEnvironment() ;

		return utility::Task( e.args.cmd,-1,s,e.password,[](){},e.engine.requiresPolkit() ) ;
	}
}

static utility::Task _run_task( const run_task& e )
{
	auto fav = favorites::instance().readFavorite( e.opts.cipherFolder,e.opts.plainFolder ) ;

	if( fav.has_value() ){

		const auto& m = fav.value() ;

		auto a = utility::Task::makePath( e.opts.cipherFolder ) ;
		auto b = utility::Task::makePath( e.opts.plainFolder ) ;
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

static utility::qstring_result _build_config_file_path( const build_config_path& e )
{
	if( e.configFilePath.isEmpty() ){

		return QString() ;
	}else{
		auto s = e.engine.setConfigFilePath( _makePath( e.configFilePath ) ) ;

		if( s.isEmpty() ){

			return {} ;
		}else{
			return s ;
		}
	}
}

static engines::engine::cmdStatus _cmd( const cmd_args& e )
{
	const engines::engine& engine        = e.engine ;
	const engines::engine::options& opts = e.opts ;
	const QByteArray& password           = e.password ;
	const QString& configFilePath        = e.configFilePath ;
	bool create                          = e.create ;

	auto exe = engine.executableFullPath() ;

	if( exe.isEmpty() ){

		return { engine.notFoundCode(),engine } ;
	}

	if( engine.requiresPolkit() && !utility::enablePolkit() ){

		return { engines::engine::status::failedToStartPolkit,engine } ;
	}

	auto m = _build_config_file_path( { engine,configFilePath } ) ;

	if( !m ){

		return { engines::engine::status::backEndDoesNotSupportCustomConfigPath,engine } ;
	}

	auto cc = _makePath( opts.cipherFolder ) ;
	auto mm = _makePath( opts.plainFolder ) ;

	exe = utility::Task::makePath( exe ) ;

	auto cmd = engine.command( password,{ exe,opts,m.value(),cc,mm,create } ) ;

	auto s = _run_task( { cmd,e } ) ;

	if( s.success() ){

		return { engines::engine::status::success,engine } ;
	}else{
		if( utility::platformIsWindows() ){

			if( SiriKali::Windows::backEndTimedOut( s.stdOut() ) ){

				return { engines::engine::status::backendTimedOut,engine } ;
			}
		}

		auto ss = s.stdError().isEmpty() ? s.stdOut() : s.stdError() ;

		auto n = engine.errorCode( ss,s.exitCode() ) ;

		return { n,engine,ss } ;
	}
}

static utility::qstring_result _path_exist( QString e,const QString& m )
{
	e.remove( 0,m.size() ) ;

	if( utility::pathExists( e ) ){

		return e ;
	}else{
		return {} ;
	}
}

siritask::Engine siritask::mountEngine( const siritask::mount& e )
{
	const QString& cipherFolder    = e.cipherFolder ;
	const QString& configFilePath  = e.configFilePath ;
	const siritask::Engine& Engine = e.engine ;

	if( Engine.get().known() ){

		const auto& engine = Engine.get() ;

		auto a = "[[[" + engine.name() + "]]]" ;

		auto b = engine.name() + " " ;

		if( configFilePath.startsWith( a ) ){

			return { { engine,configFilePath.mid( a.size() ),cipherFolder } } ;

		}else if( cipherFolder.startsWith( b,Qt::CaseInsensitive ) ){

			return { { engine,configFilePath,cipherFolder.mid( b.size() ) } } ;
		}else{
			return { { engine,configFilePath,cipherFolder } } ;
		}
	}

	const auto& engines = engines::instance() ;

	if( utility::pathIsFile( cipherFolder ) ){

		const auto& engine = engines.getByFileExtension( cipherFolder ) ;

		return { { engine,configFilePath,cipherFolder } } ;
	}

	for( const auto& it : engines.supportedEngines() ){

		if( cipherFolder.startsWith( it->name() + " ",Qt::CaseInsensitive ) ){

			const auto& engine = it.get() ;

			if( engine.known() ){

				return { { engine,configFilePath,cipherFolder.mid( it->name().size() + 1 ) } } ;
			}
		}
	}

	if( configFilePath.isEmpty() ){

		auto m = engines.getByConfigFileNames( [ & ]( const QString& e ){

			return utility::pathExists( cipherFolder + "/" + e ) ;
		} ) ;

		if( m.first.known() ){

			return { { m.first,"",cipherFolder } } ;
		}else{
			const auto& engine = engines.getByName( "fscrypt" ) ;

			return { { engine.proveEngine( cipherFolder ),"",cipherFolder } } ;
		}

	}else if( utility::pathExists( configFilePath ) ){

		auto m = engines.getByConfigFileNames( [ & ]( const QString& e ){

			return configFilePath.endsWith( e ) ;
		} ) ;

		return { { m.first,configFilePath,cipherFolder } } ;
	}else{
		for( const auto& it : engines.supportedEngines() ){

			const auto& engine = it.get() ;

			for( const auto& xt: engine.names() ){

				auto s = "[[[" + xt + "]]]" ;

				if( configFilePath.startsWith( s ) ){

					auto m = _path_exist( configFilePath,s ) ;

					if( m ){

						return { { engine,m.value(),cipherFolder } } ;
					}
				}
			}
		}

		return {} ;
	}
}

static engines::engine::cmdStatus _mount( Opts opt,bool reUseMP,const siritask::Engine& eng )
{
	auto Engine = siritask::mountEngine( { opt.cipherFolder,opt.configFilePath,eng } ) ;

	const auto& engine  = Engine.get() ;

	opt.configFilePath  = Engine.configFilePath() ;
	opt.cipherFolder    = Engine.cipherFolder() ;

	engine.updateOptions( opt ) ;

	if( engine.unknown() ){

		return { engines::engine::status::unknown,engine } ;
	}

	auto mm = engine.passMinimumVersion() ;

	if( mm != engines::engine::status::success ){

		return { mm,engine } ;
	}

	if( opt.key.isEmpty() && engine.requiresAPassword() ){

		return { engines::engine::status::backendRequiresPassword,engine } ;
	}

	if( _illegal_path( opt,engine ) ){

		return { engines::engine::status::IllegalPath,engine } ;
	}

	if( engine.backendRequireMountPath() ){

		if( !( _create_folder( opt.plainFolder ) || reUseMP ) ){

			return { engines::engine::status::failedToCreateMountPoint,engine } ;
		}
	}

	auto e = _cmd( { engine,false,opt,opt.key,opt.configFilePath } ) ;

	if( e != engines::engine::status::success ){

		if( engine.backendRequireMountPath() ){

			siritask::deleteMountFolder( opt.plainFolder ) ;
		}
	}else{
		engine.updateVolumeList( opt ) ;
	}

	return e ;
}

static utility::qstring_result _configFilePath( const Engs& engine,const Opts& opt )
{
	if( opt.configFilePath.isEmpty() ){

		auto opts = opt ;

		engine.updateOptions( opts ) ;

		return opts.configFilePath ;
	}else{
		auto m = QDir().absoluteFilePath( opt.configFilePath ) ;

		for( const auto& it : engine.configFileNames() ){

			if( m.endsWith( it ) ){

				return m ;
			}
		}

		return {} ;
	}
}

static engines::engine::cmdStatus _create( const Opts& opt,const Engs& engine )
{
	if( engine.unknown() ){

		return { engines::engine::status::unknown,engine } ;
	}

	auto mm = engine.passMinimumVersion() ;

	if( mm != engines::engine::status::success ){

		return { mm,engine } ;
	}

	if( _illegal_path( opt,engine ) ){

		return { engines::engine::status::IllegalPath,engine } ;
	}

	if( opt.key.isEmpty() && engine.requiresAPassword() ){

		return { engines::engine::status::backendRequiresPassword,engine } ;
	}

	auto configPath = _configFilePath( engine,opt ) ;

	if( !configPath ){

		return { engines::engine::status::invalidConfigFileName,engine } ;
	}

	if( !_create_folder( opt.cipherFolder ) ){

		return { engines::engine::status::failedToCreateMountPoint,engine } ;
	}

	if( engine.backendRequireMountPath() ){

		if( !_create_folder( opt.plainFolder ) ){

			_deleteFolders( opt.cipherFolder ) ;

			return { engines::engine::status::failedToCreateMountPoint,engine } ;
		}
	}

	auto e = _cmd( { engine,true,opt,engine.setPassword( opt.key ),configPath.value() } ) ;

	if( e == engines::engine::status::success ){

		engine.updateVolumeList( opt ) ;

		if( !engine.autoMountsOnCreate() ){

			auto e = siritask::encryptedFolderMount( opt,true ) ;

			if( e != engines::engine::status::success ){

				if( e.engine().backendRequireMountPath() ){

					_deleteFolders( opt.plainFolder,opt.cipherFolder ) ;
				}else{
					_deleteFolders( opt.cipherFolder ) ;
				}
			}
		}
	}else{
		if( e.engine().backendRequireMountPath() ){

			_deleteFolders( opt.plainFolder,opt.cipherFolder ) ;
		}else{
			_deleteFolders( opt.cipherFolder ) ;
		}
	}

	return e ;
}

static void _warning( const QString& e )
{
	auto a = "ERROR!!\n " + e + "siritask::encryptedFolderMount is"  ;
	auto b = "running from a background thread" ;

	utility::debug() << a + b ;
}

engines::engine::cmdStatus siritask::encryptedFolderCreate( const Opts& opt,const Engs& e )
{
	if( utility::platformIsWindows() ){

		if( utility::runningOnGUIThread() ){

			return _create( opt,e ) ;
		}else{
			/*
			 * We should not take this path
			 */
			_warning( "siritask::encryptedFolderMount" ) ;

			return _create( opt,e ) ;
		}
	}else{
		auto& s = Task::run( [ & ](){ return _create( opt,e ) ; } ) ;

		return utility::unwrap( s ) ;
	}
}

static void _run_command_on_mount( const Opts& opt,const QString& type )
{
	auto exe = _cmd_args( settings::instance().runCommandOnMount() ) ;

	if( !exe.isEmpty() ){

		auto a = _makePath( opt.cipherFolder ) ;
		auto b = _makePath( opt.plainFolder ) ;

		exe = QString( "%1 %2 %3 %4" ).arg( exe,a,b,type ) ;

		Task::exec( [ = ](){

			auto r = [ & ](){

				if( settings::instance().allowExternalToolsToReadPasswords() ){

					auto e = utility::systemEnvironment() ;
					auto& m = settings::instance() ;

					e.insert( m.environmentalVariableVolumeKey(),opt.key ) ;

					return Task::process::run( exe,{},-1,{},e ).get() ;
				}else{
					const auto& e = utility::systemEnvironment() ;

					return Task::process::run( exe,{},-1,{},e ).get() ;
				}
			}() ;

			utility::logCommandOutPut( r,exe ) ;
		} ) ;
	}
}

engines::engine::cmdStatus
siritask::encryptedFolderMount( const Opts& opt,bool m,const siritask::Engine& engine )
{
	auto s = [ & ](){

		if( utility::platformIsWindows() ){

			if( utility::runningOnGUIThread() ){

				return _mount( opt,m,engine ) ;
			}else{
				/*
				 * We should not take this path
				 */
				_warning( "siritask::encryptedFolderMount" ) ;
				return _mount( opt,m,engine ) ;
			}
		}else{
			auto& e = Task::run( [ & ](){

				return _mount( opt,m,engine ) ;
			} ) ;

			return utility::unwrap( e ) ;
		}
	}() ;

	if( s == engines::engine::status::success ){

		_run_command_on_mount( opt,engine.get().name() ) ;
	}

	return s ;
}
