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

static bool _illegal_path( const engines::engine::options& opts,const engines::engine& engine )
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

utility::result< utility::Task > siritask::unmountVolume( const QString& exe,
							  const QString& mountPoint,
							  bool usePolkit )
{
	auto e = _cmd_args( settings::instance().preUnMountCommand() ) ;

	int timeOut = 10000 ;

	if( e.isEmpty() ){

		return utility::unwrap( utility::Task::run( exe,timeOut,usePolkit ) ) ;
	}else{
		auto m = utility::unwrap( utility::Task::run( e + " " + mountPoint,timeOut,false ) ) ;

		if( m.success() ){

			return utility::unwrap( utility::Task::run( exe,timeOut,usePolkit ) ) ;
		}else{
			return {} ;
		}
	}
}

static bool _unmount_rest_( const QString& cmd,const QString& mountPoint )
{
	auto s = siritask::unmountVolume( cmd,mountPoint,false ) ;

	return s && s.value().success() ;
}

bool siritask::unmountVolume( const QString& mountPoint,const QString& unMountCommand,int maxCount )
{
	auto cmd = [ & ](){

		if( unMountCommand.isEmpty() ){

			if( utility::platformIsOSX() ){

				return "umount " + mountPoint ;
			}else{
				return "fusermount -u " + mountPoint ;
			}
		}else{
			return unMountCommand + " " + mountPoint ;
		}
	}() ;

	if( _unmount_rest_( cmd,mountPoint ) ){

		return true ;
	}else{
		for( int i = 1 ; i < maxCount ; i++ ){

			utility::Task::waitForOneSecond() ;

			if( _unmount_rest_( cmd,mountPoint ) ){

				return true ;
			}
		}

		return false ;
	}
}

static void _run_command( const QString& command,const QString& password = QString() )
{
	if( password.isEmpty() ){

		utility::unwrap( utility::Task::run( command ) ) ;
	}else{
		auto e = utility::systemEnvironment() ;

		e.insert( settings::instance().environmentalVariableVolumeKey(),password ) ;

		utility::unwrap( Task::run( [ & ](){ utility::Task( command,e,[](){} ) ; } ) ) ;
	}
}

static void _run_command( const QString& command,
			  const QString& cipherFolder,
			  const QString& plainFolder,
			  const QString& volumeType,
			  const QString& commandType,
			  const QString& password = QString() )
{
	if( command.isEmpty() ){

		return ;
	}

	auto exe = _cmd_args( command ) ;

	if( exe.isEmpty() ){

		utility::debug() << "Failed to find \"" + commandType + "\" command : " + command ;
	}else{
		auto m = QString( "%1 %2 %3 %4" ).arg( exe,cipherFolder,plainFolder,volumeType ) ;

		_run_command( m,password ) ;
	}
}

static siritask::taskResult _encrypted_unmount( const QString& cipherFolder,
						const QString& mountPoint,
						const QString& fileSystem,int numberOfAttempts )
{
	const auto& engine = engines::instance().getByName( fileSystem ) ;

	if( utility::platformIsWindows() ){

		auto m = engine.windowsUnMountCommand() ;

		return { SiriKali::Windows::unmount( m,_makePath( mountPoint ) ).success(),engine } ;
	}else{

		if( engine.unknown() ){

			return { false,engine } ;
		}else{
			if( engine.requiresPolkit() ){

				if( !utility::enablePolkit() ){

					return { engines::engine::status::failedToStartPolkit } ;
				}
			}

			auto a = _makePath( cipherFolder ) ;
			auto b = _makePath( mountPoint ) ;

			auto& e = Task::run( [ & ](){

				return engine.unmount( a,b,numberOfAttempts ) ;
			} ) ;

			return { utility::unwrap( e ),engine } ;
		}
	}
}

siritask::taskResult siritask::encryptedFolderUnMount( const QString& cipherFolder,
						       const QString& mountPoint,
						       const QString& fileSystem,
						       int numberOfAttempts )
{
	auto fav = favorites::instance().readFavorite( cipherFolder,mountPoint ) ;

	if( fav.has_value() ){

		const auto& m = fav.value() ;

		auto a = utility::Task::makePath( cipherFolder ) ;
		auto b = utility::Task::makePath( mountPoint ) ;

		_run_command( m.preUnmountCommand,a,b,fileSystem,"pre unmount" ) ;

		auto s = _encrypted_unmount( cipherFolder,
					     mountPoint,
					     fileSystem,
					     numberOfAttempts ) ;

		if( s.success() ){

			_run_command( m.postUnmountCommand,a,b,fileSystem,"post unmount" ) ;
		}

		return s ;
	}else{
		return _encrypted_unmount( cipherFolder,
					   mountPoint,
					   fileSystem,
					   numberOfAttempts ) ;
	}
}

static utility::Task _run_task_0( const engines::engine::args& args,
				  const engines::engine& engine,
				  const QString& password,
				  const engines::engine::options& opts,
				  bool create,
				  bool require_polkit )
{
	if( utility::platformIsWindows() ){

		if( create ){

			return SiriKali::Windows::create( { args,opts,engine,password } ) ;
		}else{
			return SiriKali::Windows::mount( { args,opts,engine,password } ) ;
		}
	}else{
		return utility::Task( args.cmd,-1,engine.getProcessEnvironment(),
				      password.toLatin1(),[](){},require_polkit ) ;
	}
}

static utility::Task _run_task( const engines::engine::args& args,
				const engines::engine& engine,
				const QString& password,
				const engines::engine::options& opts,
				bool create,
				bool require_polkit )
{
	auto fav = favorites::instance().readFavorite( opts.cipherFolder,opts.plainFolder ) ;

	if( fav.has_value() ){

		const auto& m = fav.value() ;

		auto a = utility::Task::makePath( opts.cipherFolder ) ;
		auto b = utility::Task::makePath( opts.plainFolder ) ;
		const auto& c = engine.name() ;

		QString key ;

		if( settings::instance().allowExternalToolsToReadPasswords() ){

			key = password ;
		}

		_run_command( m.preMountCommand,a,b,c,"pre mount",key ) ;

		auto s = _run_task_0( args,engine,password,opts,create,require_polkit ) ;

		if( s.success() ){

			_run_command( m.postMountCommand,a,b,c,"post mount",key ) ;
		}

		return s ;
	}else{
		return _run_task_0( args,engine,password,opts,create,require_polkit ) ;
	}
}

static utility::result< QString > _build_config_file_path( const engines::engine& engine,
							   const QString& configFilePath )
{
	if( configFilePath.isEmpty() ){

		return QString() ;
	}else{
		auto s = engine.setConfigFilePath( _makePath( configFilePath ) ) ;

		if( s.isEmpty() ){

			return {} ;
		}else{
			return s ;
		}
	}
}

static engines::engine::cmdStatus _cmd( const engines::engine& engine,
					bool create,
					const engines::engine::options& opts,
					const QString& password,
					const QString& configFilePath )
{
	auto exe = engine.executableFullPath() ;

	if( exe.isEmpty() ){

		return engine.notFoundCode() ;
	}else{
		if( engine.requiresPolkit() ){

			if( !utility::enablePolkit() ){

				return { engines::engine::status::failedToStartPolkit } ;
			}
		}

		exe = utility::Task::makePath( exe ) ;

		auto _run = [ & ]()->engines::engine::cmdStatus{

			auto m = _build_config_file_path( engine,configFilePath ) ;

			if( m ){

				auto cc = _makePath( opts.cipherFolder ) ;
				auto mm = _makePath( opts.plainFolder ) ;

				auto cmd = engine.command( password,{ exe,opts,m.value(),cc,mm,create } ) ;

				auto s = _run_task( cmd,engine,password,opts,create,engine.requiresPolkit() ) ;

				if( s.success() ){

					return { engines::engine::status::success,s.exitCode() } ;
				}else{				
					if( SiriKali::Windows::backEndTimedOut( s.stdOut() ) ){

						return engines::engine::status::backendTimedOut ;
					}

					auto m = s.stdError().isEmpty() ? s.stdOut() : s.stdError() ;

					auto n = engine.errorCode( m,s.exitCode() ) ;

					return { n,s.exitCode(),m } ;
				}
			}else{
				return engines::engine::status::backEndDoesNotSupportCustomConfigPath ;
			}
		} ;

		auto s = _run() ;

		if( s == engines::engine::status::ecrypfsBadExePermissions ){

			if( utility::enablePolkit() ){

				s = _run() ;
			}
		}

		return s ;
	}
}

static siritask::taskResult _mount( bool reUseMountPoint,
				    const engines::engine& engine,
				    const engines::engine::options& opt,
				    const QString& configFilePath )
{
	if( _illegal_path( opt,engine ) ){

		return { engines::engine::status::IllegalPath } ;
	}

	if( engine.backendRequireMountPath() ){

		if( !( _create_folder( opt.plainFolder ) || reUseMountPoint ) ){

			return { engines::engine::status::failedToCreateMountPoint } ;
		}
	}

	auto e = _cmd( engine,false,opt,opt.key,configFilePath ) ;

	if( e != engines::engine::status::success ){

		if( engine.backendRequireMountPath() ){

			siritask::deleteMountFolder( opt.plainFolder ) ;
		}

		return e ;
	}else{
		return { e,engine } ;
	}
}

static utility::result< QString > _path_exist( QString e,const QString& m )
{
	e.remove( 0,m.size() ) ;

	if( utility::pathExists( e ) ){

		return e ;
	}else{
		return {} ;
	}
}

siritask::Engine siritask::mountEngine( const QString& cipherFolder,
					const QString& configFilePath,
					const siritask::Engine& engine )
{
	if( engine.engine().known() ){

		auto a = "[[[" + engine.engine().name() + "]]]" ;

		auto b = engine.engine().name() + " " ;

		if( configFilePath.startsWith( a ) ){

			return { engine.engine(),cipherFolder,configFilePath.mid( a.size() ) } ;

		}else if( cipherFolder.startsWith( b,Qt::CaseInsensitive ) ){

			return { engine.engine(),cipherFolder.mid( b.size() ),configFilePath.mid( b.size() ) } ;
		}else{
			return { engine.engine(),cipherFolder,configFilePath } ;
		}
	}

	const auto& engines = engines::instance() ;

	if( utility::pathIsFile( cipherFolder ) ){

		const auto& engine = engines.getByFileExtension( cipherFolder ) ;

		return { { engine,configFilePath,cipherFolder } } ;
	}

	for( const auto& it : engines.supported() ){

		if( cipherFolder.startsWith( it + " ",Qt::CaseInsensitive ) ){

			const auto& engine = engines.getByName( it ) ;

			if( engine.known() ){

				return { { engine,configFilePath,cipherFolder.mid( it.size() + 1 ) } } ;
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
			const auto& engine = engines::instance().getByName( "fscrypt" ) ;

			return { { engine.proveEngine( cipherFolder ),"",cipherFolder } } ;
		}

	}else if( utility::pathExists( configFilePath ) ){

		auto m = engines.getByConfigFileNames( [ & ]( const QString& e ){

			return configFilePath.endsWith( e ) ;
		} ) ;

		return { { m.first,configFilePath,cipherFolder } } ;
	}else{
		for( const auto& it : engines.supported() ){

			const auto& engine = engines.getByName( it.toLower() ) ;

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

static siritask::taskResult _encrypted_folder_mount( const engines::engine::options& opts,
						     bool reUseMP,
						     const siritask::Engine& eng )
{	
	auto Engine = siritask::mountEngine( opts.cipherFolder,opts.configFilePath,eng ) ;

	const auto& engine  = Engine.engine() ;
	auto configFilePath = Engine.configFilePath() ;

	engines::engine::options opt = opts ;

	opt.cipherFolder = Engine.cipherFolder() ;
	opt.type         = Engine.engine().name() ;

	if( engine.unknown() ){

		return { engines::engine::status::unknown } ;
	}

	auto mm = engine.passMinimumVersion() ;

	if( mm != engines::engine::status::success ){

		return { mm } ;
	}

	if( opt.key.isEmpty() && engine.requiresAPassword() ){

		return { engines::engine::status::backendRequiresPassword } ;
	}

	engine.updateMountOptions( opt,configFilePath ) ;

	return _mount( reUseMP,engine,opt,configFilePath ) ;
}

static utility::result< QString > _configFilePath( const engines::engine& engine,
						   const engines::engine::options& opt )
{
	if( opt.configFilePath.isEmpty() ){

		if( engine.name() == "ecryptfs" ){

			return opt.cipherFolder + "/" + engine.configFileName() ;
		}else{
			return QString() ;
		}
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

static siritask::taskResult _encrypted_folder_create( const engines::engine::options& opt )
{
	const auto& engine = engines::instance().getByName( opt ) ;

	if( engine.unknown() ){

		return { engines::engine::status::unknown } ;
	}

	auto mm = engine.passMinimumVersion() ;

	if( mm != engines::engine::status::success ){

		return { mm } ;
	}

	if( _illegal_path( opt,engine ) ){

		return { engines::engine::status::IllegalPath } ;
	}

	if( opt.key.isEmpty() && engine.requiresAPassword() ){

		return { engines::engine::status::backendRequiresPassword } ;
	}

	auto configPath = _configFilePath( engine,opt ) ;

	if( !configPath ){

		return { { engines::engine::status::invalidConfigFileName,engine.configFileNames() } } ;
	}

	if( !_create_folder( opt.cipherFolder ) ){

		return { engines::engine::status::failedToCreateMountPoint } ;
	}

	if( engine.backendRequireMountPath() ){

		if( !_create_folder( opt.plainFolder ) ){

			_deleteFolders( opt.cipherFolder ) ;

			return { engines::engine::status::failedToCreateMountPoint } ;
		}
	}

	auto e = _cmd( engine,true,opt,engine.setPassword( opt.key ),configPath.value() ) ;

	if( e == engines::engine::status::success ){

		if( !engine.autoMountsOnCreate() ){

			auto e = siritask::encryptedFolderMount( opt,true ) ;

			if( e.cmdStatus() != engines::engine::status::success ){

				if( engine.backendRequireMountPath() ){

					_deleteFolders( opt.plainFolder,opt.cipherFolder ) ;
				}else{
					_deleteFolders( opt.cipherFolder ) ;
				}
			}
		}

		return { e,engine } ;
	}else{
		if( engine.backendRequireMountPath() ){

			_deleteFolders( opt.plainFolder,opt.cipherFolder ) ;
		}else{
			_deleteFolders( opt.cipherFolder ) ;
		}

		return e ;
	}	
}

siritask::taskResult siritask::encryptedFolderCreate( const engines::engine::options& opt )
{
	if( utility::platformIsWindows() ){

		if( utility::runningOnGUIThread() ){

			return _encrypted_folder_create( opt ) ;
		}else{
			/*
			 * We should not take this path
			 */
			utility::debug() << "ERROR!!\nsiritask::encryptedFolderCreate is running\nfrom a background thread" ;
			return _encrypted_folder_create( opt ) ;
		}
	}else{
		auto& e = Task::run( [ & ](){ return _encrypted_folder_create( opt ) ; } ) ;

		return utility::unwrap( e ) ;
	}
}

static void _run_command_on_mount( const engines::engine::options& opt )
{
	auto exe = _cmd_args( settings::instance().runCommandOnMount() ) ;

	if( !exe.isEmpty() ){

		auto a = _makePath( opt.cipherFolder ) ;
		auto b = _makePath( opt.plainFolder ) ;

		exe = QString( "%1 %2 %3 %4" ).arg( exe,a,b,opt.type ) ;

		Task::exec( [ = ](){

			auto r = [ & ](){

				if( settings::instance().allowExternalToolsToReadPasswords() ){

					auto e = utility::systemEnvironment() ;

					e.insert( settings::instance().environmentalVariableVolumeKey(),opt.key ) ;

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

siritask::taskResult siritask::encryptedFolderMount( const engines::engine::options& opt,
						     bool reUseMountPoint,
						     const siritask::Engine& engine )
{
	auto s = [ & ](){

		if( utility::platformIsWindows() ){

			if( utility::runningOnGUIThread() ){

				return _encrypted_folder_mount( opt,reUseMountPoint,engine ) ;
			}else{
				/*
				 * We should not take this path
				 */
				utility::debug() << "ERROR!!\nsiritask::encryptedFolderMount is running from a background thread" ;

				return _encrypted_folder_mount( opt,reUseMountPoint,engine ) ;
			}
		}else{
			auto& e = Task::run( [ & ](){

				return _encrypted_folder_mount( opt,reUseMountPoint,engine ) ;
			} ) ;

			return utility::unwrap( e ) ;
		}
	}() ;

	if( s.cmdStatus() == engines::engine::status::success ){

		_run_command_on_mount( opt ) ;
	}

	return s ;
}
