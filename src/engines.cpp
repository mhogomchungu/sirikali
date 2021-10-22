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

#include "engines.h"

#include "engines/ecryptfs.h"
#include "engines/cryfs.h"
#include "engines/gocryptfs.h"
#include "engines/encfs.h"
#include "engines/sshfs.h"
#include "engines/unknown.h"
#include "engines/securefs.h"
#include "engines/fscrypt.h"
#include "engines/custom.h"
#include "engines/cryptomator.h"

#include "utility.h"
#include "settings.h"
#include "win.h"
#include "engines/options.h"

#include <QCoreApplication>

static QString _emptyQString ;

static QStringList _system_search_paths()
{
	return { "/usr/local/bin/",
		 "/usr/local/sbin/",
		 "/usr/bin/",
		 "/usr/sbin/",
		 "/bin/",
		 "/sbin/",
		 "/opt/local/bin/",
		 "/opt/local/sbin/",
		 "/opt/bin/",
		 "/opt/sbin/" } ;
}

static QStringList _search_path_0( const QString& e )
{
	QStringList s = { e,e + "\\bin\\",e + "\\.bin\\" };

	const auto a = QDir( e ).entryList( QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) ;

	for( const auto& it : a ){

		s.append( e + it + "\\" ) ;
		s.append( e + it + "\\bin\\" ) ;
		s.append( e + it + "\\.bin\\" ) ;
	}

	return s ;
}

static QStringList _search_path( const QStringList& m )
{
	const auto a = QDir::homePath().toUtf8() ;

	if( utility::platformIsWindows() ){

		auto x = _search_path_0( a + "\\bin\\" ) ;
		x += _search_path_0( QDir().currentPath() ) ;
		x += _search_path_0( settings::instance().windowsExecutableSearchPath() + "\\" ) ;

		for( const auto& it : m ){

			if( !it.isEmpty() ){

				x += _search_path_0( it + "\\" ) ;
			}
		}

		return x ;
	}else{
		const auto b = a + "/bin/" ;
		const auto c = a + "/.bin/" ;
		const auto d = QCoreApplication::applicationDirPath().toUtf8() ;

		auto m = QStringList{ b.constData(),c.constData(),d.constData() } ;

		return _system_search_paths() + m ;
	}
}

static bool _has_no_extension( const QString& e )
{
	return !e.contains( '.' ) ;
}

template< typename Function >
static QString _executableFullPath( const QString& f,Function function )
{
	if( utility::platformIsWindows() ){

		if( utility::startsWithDriveLetter( f ) ){

			if( _has_no_extension( f ) ){

				return f + ".exe " ;
			}else{
				return f ;
			}
		}
	}else{
		if( f.startsWith( "/" ) ){

			return f ;
		}
	}

	QString e = f ;

	if( utility::platformIsWindows() && _has_no_extension( e ) ){

		e += ".exe" ;
	}

	QString exe ;

	for( const auto& it : function() ){

		if( !it.isEmpty() ){

			exe = it + e ;

			if( QFile::exists( exe ) ){

				return exe ;
			}
		}
	}

	return QString() ;
}

QStringList engines::executableSearchPaths()
{
	return _search_path( SiriKali::Windows::engineInstalledDirs() ) ;
}

QStringList engines::executableSearchPaths( const engines::engine& engine )
{
	return _search_path( { SiriKali::Windows::engineInstalledDir( engine ),
			       engine.windowsExecutableFolderPath() } ) ;
}

QString engines::executableNotEngineFullPath( const QString& e )
{
	return _executableFullPath( e,[](){ return _system_search_paths() ; } ) ;
}

QString engines::executableFullPath( const QString& f )
{
	return _executableFullPath( f,[](){ return engines::executableSearchPaths() ; } ) ;
}

QString engines::executableFullPath( const QString& f,const engines::engine& engine )
{
	return _executableFullPath( f,[ &engine ](){ return engines::executableSearchPaths( engine ) ; } ) ;
}

void engines::version::logError() const
{
	auto a = QString( "%1 backend has an invalid version string (%2)" ) ;
	utility::debug() << a.arg( m_engineName,this->toString() ) ;
}

struct resolveStruct{
	const engines::engine& engine ;
	const QString& controlStructure ;
	const engines::engine::cmdArgsList& args ;
	const QByteArray& password ;
	const QStringList& opts ;
	const QStringList& fuseOpts ;
} ;

class resolve{
public:
	struct args{
		const char * first ;
		const QString& second ;
	} ;
	template< typename ... T >
	resolve( T&& ... e )
	{
		this->set( std::forward< T >( e ) ... ) ;
	}
	QString option( QString a ) const
	{
		for( const auto& it : m_opts ){

			if( !it.second.isEmpty() ){

				a.replace( it.first,it.second ) ;
			}
		}

		for( const auto& it : m_opts ){

			if( a.contains( it.first ) ) {

				return {} ;
			}
		}

		return a ;
	}
private:
	template< typename T >
	void set( T&& t )
	{
		m_opts.emplace_back( std::forward< T >( t ) ) ;
	}
	template< typename E,typename ... T >
	void set( E&& e,T&& ... t )
	{
		this->set( std::forward< E >( e ) ) ;
		this->set( std::forward< T >( t ) ... ) ;
	}
	std::vector< resolve::args > m_opts ;
};

class replace{
public:
	replace( QStringList& s,int position ) :
		m_stringList( s ),m_position( position )
	{
	}
	void set( const QString& e )
	{
		m_stringList.insert( m_position,e ) ;
	}
	void set( const QStringList& e )
	{
		for( int i = e.size() - 1 ; i >= 0 ; i-- ){

			m_stringList.insert( m_position,e.at( i ) ) ;
		}
	}
private:
	QStringList& m_stringList ;
	int m_position ;
};

template< typename ... T >
static void _resolve( QStringList& orgs,
		      const QString& name,
		      const QString& controlStructure,
		      T&& ... rrr )
{
	if( controlStructure.isEmpty() ){

		return ;
	}

	resolve rr( std::forward< T >( rrr ) ... ) ;

	auto m = utility::split( controlStructure,' ' ) ;

	if( m.size() == 1 ){

		auto a = rr.option( m.at( 0 ) ) ;

		if( !a.isEmpty() ){

			orgs.append( a ) ;
		}

	}else if( m.size() == 2 ){

		auto a = rr.option( m.at( 1 ) ) ;

		if( !a.isEmpty() ){

			orgs.append( m.at( 0 ) ) ;
			orgs.append( a ) ;
		}
	}else{
		auto s = QString( "Wrong control structure detected in custom backend named \"%1\"." ) ;
		utility::debug::logErrorWhileStarting( s.arg( name ) ) ;
	}
}

static QStringList _replace_opts( const resolveStruct& r )
{
	auto opts = r.opts ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.configFileArgument(),
		  resolve::args{ "%{cipherFolder}",r.args.cipherFolder },
		  resolve::args{ "%{configFileName}",r.engine.configFileName() },
		  resolve::args{ "%{configFilePath}",r.args.configFilePath } ) ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.keyFileArgument(),
		  resolve::args{ "%{keyfile}",r.args.keyFile } ) ;

	_resolve( opts,
		  r.engine.name(),
		  r.engine.idleString(),
		  resolve::args{ "%{timeout}",r.args.idleTimeout } ) ;

	return opts ;
}

template< typename Function >
static void _replace_opts( QStringList& mm,
			    const char * controlStructure,
			    Function function )
{
	for( int i = 0 ; i < mm.size() ; i++ ){

		auto& it = mm[ i ] ;

		if( it == controlStructure ){

			mm.removeAt( i ) ;

			function( { mm,i } ) ;

			break ;
		}
	}
}

static QStringList _resolve( const resolveStruct& r )
{
	auto mm = utility::split( r.controlStructure,' ' ) ;

	_replace_opts( mm,"%{cipherFolder}",[ & ]( replace s ){

		s.set( r.args.cipherFolder ) ;
	} ) ;

	_replace_opts( mm,"%{mountPoint}",[ & ]( replace s ){

		s.set( r.args.mountPoint ) ;
	} ) ;

	_replace_opts( mm,"%{password}",[ & ]( replace s ){

		s.set( r.password ) ;
	} ) ;

	_replace_opts( mm,"%{fuseOpts}",[ & ]( replace s ){

		if( !r.fuseOpts.isEmpty() ){

			if( r.engine.usesFuseArgumentSwitch() ){

				if( r.args.fuseOptionsSeparator.isEmpty() ){

					s.set( { "-o",r.fuseOpts.join( ',' ) } ) ;
				}else{
					s.set( { r.args.fuseOptionsSeparator,"-o",r.fuseOpts.join( ',' ) } ) ;
				}
			}else{
				if( r.args.fuseOptionsSeparator.isEmpty() ){

					s.set( { r.fuseOpts.join( ',' ) } ) ;
				}else{
					s.set( { r.args.fuseOptionsSeparator,r.fuseOpts.join( ',' ) } ) ;
				}
			}
		}
	} ) ;

	_replace_opts( mm,"%{createOptions}",[ & ]( replace s ){

		s.set( _replace_opts( r ) ) ;
	} ) ;

	_replace_opts( mm,"%{mountOptions}",[ & ]( replace s ){

		auto opts = _replace_opts( r ) ;

		if( r.args.boolOptions.unlockInReverseMode ){

			opts.append( r.engine.reverseString() ) ;
		}

		s.set( opts ) ;
	} ) ;

	mm.removeAll( QString() ) ;

	return mm ;
}

engines::engine::args engines::engine::command( const QByteArray& password,
						const engines::engine::cmdArgsList& args,
						bool create ) const
{
	const auto& engine = *this ;

	engines::engine::commandOptions m( create,engine,args ) ;

	auto s = [ & ](){

		if( create ){

			auto opts = args.createOptions + m.exeOptions().get() ;

			return _resolve( { engine,
					   engine.createControlStructure(),
					   args,
					   password,
					   opts,
					   m.fuseOpts().get() } ) ;
		}else{
			return _resolve( { engine,
					   engine.mountControlStructure(),
					   args,
					   password,
					   m.exeOptions().get(),
					   m.fuseOpts().get() } ) ;
		}
	}() ;

	engine.updateOptions( s,args,create ) ;

	const auto& exe = engine.executableFullPath() ;

	if( engine.needsJava() ){

		s.insert( 0,exe ) ;
		s.insert( 0,"-jar" ) ;

		return { args,m,engines::engine::javaFullPath(),s } ;
	}else{
		return { args,m,exe,s } ;
	}
}

engines::engine::status engines::engine::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( "cygfuse: initialization failed: winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;

	}else if( e.contains( "cannot load WinFsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;

	}else if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::badPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

void engines::engine::updateVolumeList( const engines::engine::cmdArgsList& e ) const
{
	Q_UNUSED( e )
}

volumeInfo::List engines::engine::mountInfo( const volumeInfo::List& e ) const
{
	Q_UNUSED( e )
	return {} ;
}

Task::future< QString >& engines::engine::volumeProperties( const QString& cipherFolder,
							    const QString& mountPoint ) const
{
	return Task::run( [ this,cipherFolder,mountPoint ](){

		for( const auto& it : this->volumePropertiesCommands() ){

			auto a = utility::split( it,' ' ) ;

			auto exe = [ & ](){

				if( a.at( 0 ) == this->executableName() ){

					return this->executableFullPath() ;
				}else{
					return engines::executableFullPath( a.at( 0 ) ) ;
				}
			}() ;

			if( !exe.isEmpty() ){

				a.removeFirst() ;

				for( auto& it : a ){

					if( it == "%{cipherFolder}" ){

						it = cipherFolder ;

					}else if( it == "%{plainFolder}" ){

						it = mountPoint ;
					}
				}

				auto e = utility::unwrap( utility::Task::run( exe,a ) ) ;

				if( e.success() ){

					return QString( e.stdOut() ) ;
				}
			}
		}

		return QString() ;
	} ) ;
}

bool engines::engine::unmountVolume( const engines::engine::exe_args_const& exe,bool usePolkit ) const
{
	int timeOut = 10000 ;

	auto& s = utility::Task::run( exe.exe,exe.args,timeOut,usePolkit ) ;

	return utility::unwrap( s ).success() ;
}

engines::engine::status engines::engine::unmount( const engines::engine::unMount& e ) const
{
	auto cmd = this->unMountCommand( e.mountPoint ) ;

	if( this->unmountVolume( cmd,false ) ){

		return engines::engine::status::success ;
	}else{
		for( int i = 1 ; i < e.numberOfAttempts ; i++ ){

			utility::Task::waitForOneSecond() ;

			if( this->unmountVolume( cmd,false ) ){

				return engines::engine::status::success ;
			}
		}

		return engines::engine::status::failedToUnMount ;
	}
}

const QProcessEnvironment& engines::engine::getProcessEnvironment() const
{
	return m_processEnvironment ;
}

utility2::LOGLEVEL engines::engine::allowLogging( const QStringList& args ) const
{
	Q_UNUSED( args )
	return utility2::LOGLEVEL::COMMAND_AND_UNLOCK_DURATION ;
}

static QString _sanitizeVersionString( const QString& s )
{
	auto e = s ;

	e.replace( "v","" ).replace( ";","" ) ;

	QString m ;

	for( int s = 0 ; s < e.size() ; s++ ){

		auto n = e.at( s ) ;

		if( n == '.' || ( n >= '0' && n <= '9' ) ){

			m += n ;
		}else{
			break ;
		}
	}

	return m ;
}

static engines::engineVersion _installedVersion( engines::exeFullPath& javaExe,
						 const engines::engine& e,
						 const QProcessEnvironment env,
						 const engines::engine::BaseOptions::vInfo& v )
{
	const auto& cmd = e.executableFullPath() ;

	const auto r = [ & ](){

		if( e.needsJava() ){

			return utility::unwrap( ::Task::process::run( javaExe.get(),{ "-jar",cmd,v.versionArgument },-1,{},env ) ) ;
		}else{
			return utility::unwrap( ::Task::process::run( cmd,{ v.versionArgument },-1,{},env ) ) ;
		}
	}() ;

	const auto m = utility::split( v.readFromStdOut ? r.std_out() : r.std_error(),'\n' ) ;

	if( m.size() > v.argumentLine ){

		const auto e = utility::split( m.at( v.argumentLine ),' ' ) ;

		if( e.size() > v.argumentNumber ){

			return _sanitizeVersionString( e.at( v.argumentNumber ) ) ;
		}
	}

	return {} ;
}

template< typename T >
static engines::engineVersion _installedVersion( engines::exeFullPath& javaExe,
						 const engines::engine& e,
						 const QProcessEnvironment env,
						 const T& v )
{
	for( const auto& it : v ){

		auto m = _installedVersion( javaExe,e,env,it ) ;

		if( m.valid() ){

			return m ;
		}else{
			QString s = "An attempt to get version info for backend \"%2\" has failed, retrying..." ;
			utility::debug() << s.arg( e.name() ) ;
		}
	}

	utility::debug() << QString( "Failed to get version info for backend \"%2\"." ).arg( e.name() ) ;

	return {} ;
}

static QProcessEnvironment _set_env( const engines::engine& engine )
{
	auto m = utility::systemEnvironment() ;

	if( utility::platformIsWindows() ){

		auto e = engines::executableSearchPaths( engine ).join( ';' ) ;

		m.insert( "PATH",e + ";" + m.value( "PATH" ) ) ;
	}

	return m ;
}

engines::engine::~engine()
{
}

static engines::engine::terminate_result _failed_to_finish( QString exe,QStringList args )
{
	auto a = Task::process::result( "BackendFailedToFinish",
					QByteArray(),
					-1,
					0,
					true ) ;

	return { std::move( a ),std::move( exe ),std::move( args ) } ;
}

engines::engine::terminate_result
engines::engine::terminateProcess( const engines::engine::terminate_process& e ) const
{
	auto aaa = this->unMountCommand( e ) ;

	auto exe = std::move( aaa.exe ) ;
	auto args = std::move( aaa.args ) ;

	if( exe.isEmpty() ){

		auto a = "SiriKali Error: Unmount Command Not Set" ;
		auto b = Task::process::result( a,QByteArray(),-1,0,true ) ;

		return { std::move( b ),{},{} } ;
	}

	if( exe == "SIGTERM" ){

		utility::debug() << "Terminating a process by sending it SIGTERM" ;

		e.terminate() ;

		if( e.waitForFinished() ){

			auto a = Task::process::result( QByteArray(),
							QByteArray(),
							0,
							0,
							true ) ;

			return { std::move( a ),std::move( exe ),std::move( args ) } ;
		}else{
			return _failed_to_finish( std::move( exe ),std::move( args ) ) ;
		}
	}

	utility::logger logger ;

	logger.showText( exe,args ) ;

	auto m = utility::unwrap( Task::process::run( exe,args,-1,"",this->getProcessEnvironment() ) ) ;

	logger.showText( m ) ;

	if( m.success() ){

		if( e.waitForFinished() ){

			return { std::move( m ),std::move( exe ),std::move( args ) } ;
		}else{
			return _failed_to_finish( std::move( exe ),std::move( args ) ) ;
		}
	}else{
		return { std::move( m ),std::move( exe ),std::move( args ) } ;
	}
}

engines::engine::terminate_result engines::engine::terminateProcess( const QString& mountPath ) const
{
	Q_UNUSED( mountPath )
	return engines::engine::terminate_result() ;
}

static engines::engine::BaseOptions _update( engines::engine::BaseOptions m )
{
	for( auto& it : m.names ){

		if( !it.isEmpty() ){

			it.replace( 0,1,it.at( 0 ).toUpper() ) ;
		}
	}

	if( !m.displayName.isEmpty() ){

		m.displayName.replace( 0,1,m.displayName.at( 0 ).toUpper() ) ;
	}

	return m ;
}

engines::exeFullPath engines::engine::m_exeJavaFullPath( [](){

	return engines::executableNotEngineFullPath( "java" ) ;
} ) ;

engines::exeFullPath engines::engine::m_exeFuserMount( [](){

	return engines::executableNotEngineFullPath( "fusermount" ) ;
} ) ;

static std::function< QString() > _exe_full_path( const QStringList& exe,const engines::engine& engine )
{
	return [ & ](){

		for( const auto& it : exe ){

			auto s = engines::executableFullPath( it,engine ) ;

			if( !s.isEmpty() ){

				return s ;
			}
		}

		return QString() ;
	} ;
}

engines::engine::engine( engines::engine::BaseOptions o ) :
	m_Options( _update( std::move( o ) ) ),
	m_processEnvironment( _set_env( *this ) ),
	m_exeFullPath( _exe_full_path( m_Options.executableNames,*this ) ),
	m_version( this->name(),[ this ](){ return _installedVersion( m_exeJavaFullPath,*this,m_processEnvironment,m_Options.versionInfo ) ; } )
{
}

const QString& engines::engine::executableFullPath() const
{
	return m_exeFullPath.get() ;
}

const QString& engines::engine::javaFullPath()
{
	return m_exeJavaFullPath.get() ;
}

const QString& engines::engine::fuserMountPath()
{
	return m_exeFuserMount.get() ;
}

bool engines::engine::needsJava() const
{
	return this->executableFullPath().endsWith( ".jar" ) ;
}

bool engines::engine::usesFuseArgumentSwitch() const
{
	return m_Options.usesFuseArgumentSwitch ;
}

bool engines::engine::isInstalled() const
{
	return !this->isNotInstalled() ;
}

bool engines::engine::isNotInstalled() const
{
	return this->executableFullPath().isEmpty() ;
}

bool engines::engine::unknown() const
{
	return this->name().isEmpty() ;
}

bool engines::engine::known() const
{
	return !this->unknown() ;
}

bool engines::engine::setsCipherPath() const
{
	return m_Options.setsCipherPath ;
}

bool engines::engine::autoMountsOnCreate() const
{
	return m_Options.autoMountsOnCreate ;
}

bool engines::engine::hasGUICreateOptions() const
{
	return m_Options.hasGUICreateOptions ;
}

bool engines::engine::hasConfigFile() const
{
	return m_Options.hasConfigFile ;
}

bool engines::engine::supportsMountPathsOnWindows() const
{
	return m_Options.supportsMountPathsOnWindows ;
}

bool engines::engine::requiresAPassword( const engines::engine::cmdArgsList& opts ) const
{
	Q_UNUSED( opts )

	return this->requiresAPassword() ;
}

bool engines::engine::requiresAPassword() const
{
	return m_Options.requiresAPassword ;
}

bool engines::engine::requiresNoPassword() const
{
	return !this->requiresAPassword() ;
}

bool engines::engine::usesOnlyMountPoint() const
{
	return m_Options.usesOnlyMountPoint ;
}

bool engines::engine::customBackend() const
{
	return m_Options.customBackend ;
}

bool engines::engine::autorefreshOnMountUnMount() const
{
	return m_Options.autorefreshOnMountUnMount ;
}

bool engines::engine::backendRequireMountPath() const
{
	return m_Options.backendRequireMountPath ;
}

bool engines::engine::runsInBackGround() const
{
	return m_Options.backendRunsInBackGround ;
}

bool engines::engine::runsInForeGround() const
{
	return !this->runsInBackGround() ;
}

bool engines::engine::acceptsSubType() const
{
	return m_Options.acceptsSubType ;
}

bool engines::engine::acceptsVolName() const
{
	return m_Options.acceptsVolName ;
}

bool engines::engine::likeSsh() const
{
	return m_Options.likeSsh ;
}

bool engines::engine::autoCreatesMountPoint() const
{
	return m_Options.autoCreatesMountPoint ;
}

bool engines::engine::autoDeletesMountPoint() const
{
	return m_Options.autoDeletesMountPoint ;
}

bool engines::engine::takesTooLongToUnlock() const
{
	return m_Options.takesTooLongToUnlock ;
}

bool engines::engine::requiresPolkit() const
{
	return m_Options.requiresPolkit ;
}

bool engines::engine::createMountPath( const QString& e ) const
{
	return utility::createFolder( e ) ;
}

bool engines::engine::createCipherPath( const QString& e ) const
{
	return utility::createFolder( e ) ;
}

bool engines::engine::deleteFolder( const QString& e,int count ) const
{
	return utility::removeFolder( e,count ) ;
}

void engines::engine::GUICreateOptions( const engines::engine::createGUIOptions& e ) const
{
	Q_UNUSED( e )
	e.fCreateOptions( {} ) ;
}

void engines::engine::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& m = options::instance( *this,s ) ;

	auto& mm = m.GUIOptions() ;

	mm.enableKeyFile      = false ;
	mm.enableCheckBox     = false ;
	mm.enableIdleTime     = false ;
	mm.enableConfigFile   = true ;
	mm.enableMountOptions = false ;

	m.ShowUI() ;
}

const QStringList& engines::engine::names() const
{
	return m_Options.names ;
}

const QStringList& engines::engine::fuseNames() const
{
	return m_Options.fuseNames ;
}

const QStringList& engines::engine::configFileNames() const
{
	return m_Options.configFileNames ;
}

const QStringList& engines::engine::fileExtensions() const
{
	return m_Options.fileExtensions ;
}

const QString& engines::engine::reverseString() const
{
	return m_Options.reverseString ;
}

const QString& engines::engine::idleString() const
{
	return m_Options.idleString ;
}

const QString& engines::engine::releaseURL() const
{
	return m_Options.releaseURL ;
}

const QString& engines::engine::executableName() const
{
	if( m_Options.executableNames.isEmpty() ){

		return _emptyQString ;
	}else{
		return m_Options.executableNames.at( 0 ) ;
	}
}

const QString& engines::engine::name() const
{
	if( m_Options.names.isEmpty() ){

		return _emptyQString ;
	}else{
		return m_Options.names.first() ;
	}
}

const QString& engines::engine::configFileName() const
{
	if( m_Options.configFileNames.isEmpty() ){

		return _emptyQString ;
	}else{
		return m_Options.configFileNames.first() ;
	}
}

const QString& engines::engine::keyFileArgument() const
{
	return m_Options.keyFileArgument ;
}

const QString& engines::engine::mountControlStructure() const
{
	return m_Options.mountControlStructure ;
}

const QString& engines::engine::createControlStructure() const
{
	return m_Options.createControlStructure ;
}

const QString& engines::engine::incorrectPasswordText() const
{
	return m_Options.incorrectPasswordText ;
}

const QString& engines::engine::incorrectPasswordCode() const
{
	return m_Options.incorrectPassWordCode ;
}

engines::engine::exe_args engines::engine::unMountCommand( const engines::engine::terminate_process& e ) const
{
	auto _replace_opts = [ & ]( QStringList a )->engines::engine::exe_args{

		for( int it = 0 ; it != a.size() ; it++ ){

			auto& s = a[ it ] ;

			if( s == "%{PID}" ){

				s = QString::number( e.PID() ) ;

			}else if( s == "%{mountPoint}" ){

				s = e.mountPath() ;
			}
		}

		auto e = a.takeAt( 0 ) ;

		return { std::move( e ),std::move( a ) } ;
	} ;

	if( utility::platformIsWindows() ){

		if( m_Options.windowsUnMountCommand.isEmpty() ){

			return {} ;
		}else{
			return _replace_opts( m_Options.windowsUnMountCommand ) ;
		}
	}else{
		if( m_Options.unMountCommand.isEmpty() ){

			if( utility::platformIsOSX() ){

				return { "umount",{ e.mountPath() } } ;
			}else{
				return { engines::engine::fuserMountPath(),{ "-u",e.mountPath() } } ;
			}
		}else{
			return _replace_opts( m_Options.unMountCommand ) ;
		}
	}
}

const QString& engines::engine::configFileArgument() const
{
	return m_Options.configFileArgument ;
}

const QString& engines::engine::windowsInstallPathRegistryKey() const
{
	return m_Options.windowsInstallPathRegistryKey ;
}

const QString& engines::engine::windowsInstallPathRegistryValue() const
{
	return m_Options.windowsInstallPathRegistryValue ;
}

const QString& engines::engine::windowsExecutableFolderPath() const
{
	return m_Options.windowsExecutableFolderPath ;
}

const QStringList& engines::engine::volumePropertiesCommands() const
{
	return m_Options.volumePropertiesCommands ;
}

const engines::version& engines::engine::installedVersion() const
{
	return m_version ;
}

const QString& engines::engine::displayName() const
{
	return m_Options.displayName ;
}

const QString& engines::engine::uiName() const
{
	const auto& s = this->displayName() ;

	if( s.isEmpty() ){

		return this->name() ;
	}else {
		return s ;
	}
}

const QString& engines::engine::defaultFavoritesMountOptions() const
{
	return m_Options.defaultFavoritesMountOptions ;
}

const QString& engines::engine::minimumVersion() const
{
	return m_Options.versionMinimum ;
}

const QString& engines::engine::sirikaliMinimumVersion() const
{
	return m_Options.sirikaliMinimumVersion ;
}

static bool _contains( const QString& e,const QStringList& m )
{
	for( const auto& it : m ){

		if( e.contains( it ) ){

			return true ;
		}
	}

	return false ;
}

engines::engine::error engines::engine::errorCode( const QString& e ) const
{
	if( _contains( e,m_Options.successfulMountedList ) ){

		return engines::engine::error::Success ;

	}else if( _contains( e,m_Options.failedToMountList ) ){

		return engines::engine::error::Failed ;
	}else{
		return engines::engine::error::Continue ;
	}
}

static bool _illegal_path( const engines::engine::cmdArgsList& opts,const engines::engine& engine )
{
	if( engine.backendRequireMountPath() ){

		return opts.cipherFolder.contains( " " ) || opts.mountPoint.contains( " " ) ;
	}else {
		return opts.cipherFolder.contains( " " ) ;
	}
}

engines::engine::status engines::engine::passAllRequirenments( const engines::engine::cmdArgsList& opt ) const
{
	if( this->unknown() ){

		return engines::engine::status::unknown ;
	}

	if( this->executableFullPath().isEmpty() ){

		return this->notFoundCode() ;
	}

	if( this->needsJava() && this->javaFullPath().isEmpty() ){

		return engines::engine::status::javaNotFound ;
	}

	if( opt.key.isEmpty() && this->requiresAPassword( opt ) ){

		return engines::engine::status::backendRequiresPassword ;
	}

	if( utility::platformIsLinux() ){

		if( this->requiresPolkit() ){

			if( _illegal_path( opt,*this ) ){

				return engines::engine::status::IllegalPath ;
			}

			if( !utility::enablePolkit() ){

				return engines::engine::status::failedToStartPolkit ;
			}
		}
	}

	if( this->configFileArgument().isEmpty() && !opt.configFilePath.isEmpty() ){

		return engines::engine::status::backEndDoesNotSupportCustomConfigPath ;
	}

	if( utility::platformIsWindows() ){

		if( !utility::isDriveLetter( opt.mountPoint ) ){

			if( utility::folderNotEmpty( opt.mountPoint ) ){

				return engines::engine::status::mountPointFolderNotEmpty ;
			}

			auto a = SiriKali::Windows::driveHasSupportedFileSystem( opt.mountPoint ) ;

			if( !a.first ){

				utility::debug() << a.second ;
				return engines::engine::status::notSupportedMountPointFolderPath ;
			}
		}
	}

	if( this->customBackend() ){

		const auto& e = this->sirikaliMinimumVersion() ;

		if( !e.isEmpty() ){

			engines::engineVersion m( e ) ;

			if( m.valid() ){

				if( m > utility::SiriKaliVersion() ){

					return engines::engine::status::backendFailedToMeetSiriKaliMinimumVersion ;
				}
			}else{
				QString s = "Warning, failed to get sirikali required minimum version for backend \"%1\"" ;
				utility::debug() << s.arg( this->name() ) ;
			}
		}

		const auto& v = this->minimumVersion() ;

		if( !v.isEmpty() ){

			engines::engineVersion engineRequiredMinimum( v ) ;

			const auto& engineInstalledVersion = this->installedVersion().get() ;

			if( engineInstalledVersion.valid() ){

				if( engineRequiredMinimum.valid() ){

					if( engineInstalledVersion < engineRequiredMinimum ){

						return engine::engine::status::backEndFailedToMeetMinimumRequirenment ;
					}
				}else{
					QString s = "Warning, failed to get required minimum version for backend \"%1\"" ;

					utility::debug() << s.arg( this->name() ) ;
				}
			}else{
				QString s = "Warning, failed to get installed version for backend \"%1\"" ;

				utility::debug() << s.arg( this->name() ) ;
			}
		}
	}

	return engines::engine::status::success ;
}

void engines::engine::updateOptions( engines::engine::cmdArgsList& e,bool s ) const
{
	if( !s && this->likeSsh() ){

		e.cipherFolder = utility::likeSshRemovePortNumber( e.cipherFolder ) ;
	}
}

void engines::engine::updateOptions( QStringList& opts,
				     const engines::engine::cmdArgsList& args,
				     bool creating ) const
{
	Q_UNUSED( creating )
	Q_UNUSED( opts )
	Q_UNUSED( args )
}

void engines::engine::updateOptions( engines::engine::commandOptions& opts,
				     const engines::engine::cmdArgsList& args,
				     bool creating ) const
{
	Q_UNUSED( creating )
	Q_UNUSED( opts )
	Q_UNUSED( args )
}

QByteArray engines::engine::setPassword( const QByteArray& e ) const
{
	auto s = m_Options.passwordFormat ;
	s.replace( "%{password}",e ) ;
	return s ;
}

engines::engine::status engines::engine::notFoundCode() const
{
	return m_Options.notFoundCode ;
}

int engines::engine::backendTimeout() const
{
	return m_Options.backendTimeout ;
}

const engines& engines::instance()
{
	static engines v ;
	return v ;
}

bool engines::atLeastOneDealsWithFiles() const
{
	for( const auto& it : this->supportedEngines() ){

		if( it->fileExtensions().size() > 0 ){

			return true ;
		}
	}

	return false ;
}

volumeInfo::List engines::mountInfo( const volumeInfo::List& m ) const
{
	volumeInfo::List s ;

	for( const auto& e : this->supportedEngines() ){

		for( auto&& it : e->mountInfo( m ) ){

			s.emplace_back( std::move( it ) ) ;
		}
	}

	return s ;
}

template< typename Engines,typename Function >
static QStringList _configs( const Engines& engines,Function function )
{
	QStringList m ;

	for( const auto& it : engines ){

		if( function( it.get() ) ){

			const auto& e = it->displayName() ;

			if( e.isEmpty() ){

				m.append( it->name() ) ;
			}else{
				m.append( e ) ;
			}
		}
	}

	return m ;
}

QStringList engines::enginesWithNoConfigFile() const
{
	return _configs( this->supportedEngines(),[]( const engines::engine& engine ){

		return !engine.hasConfigFile() ;
	} ) ;
}

QStringList engines::enginesWithConfigFile() const
{
	return _configs( this->supportedEngines(),[]( const engines::engine& engine ){

		return engine.hasConfigFile() ;
	} ) ;
}

const std::vector< engines::engine::Wrapper >& engines::supportedEngines() const
{
	return m_backendWrappers ;
}

const engines::engine& engines::getUnKnown() const
{
	return **( m_backends.data() ) ;
}

engines::engines()
{
	m_backends.emplace_back( std::make_unique< unknown >() ) ;

	if( utility::platformIsWindows() ){

		m_backends.emplace_back( std::make_unique< securefs >() ) ;
		m_backends.emplace_back( std::make_unique< cryfs >() ) ;
		m_backends.emplace_back( std::make_unique< encfs >() ) ;
		m_backends.emplace_back( std::make_unique< sshfs >() ) ;

	}else if( utility::platformIsOSX() ){

		m_backends.emplace_back( std::make_unique< securefs >() ) ;
		m_backends.emplace_back( std::make_unique< cryfs >() ) ;
		m_backends.emplace_back( std::make_unique< gocryptfs >() ) ;
		m_backends.emplace_back( std::make_unique< encfs >() ) ;
	}else{
		m_backends.emplace_back( std::make_unique< securefs >() ) ;
		m_backends.emplace_back( std::make_unique< cryfs >() ) ;
		m_backends.emplace_back( std::make_unique< gocryptfs >() ) ;
		m_backends.emplace_back( std::make_unique< encfs >() ) ;
		m_backends.emplace_back( std::make_unique< ecryptfs >() ) ;
		m_backends.emplace_back( std::make_unique< sshfs >() ) ;
		m_backends.emplace_back( std::make_unique< fscrypt >() ) ;
		m_backends.emplace_back( std::make_unique< cryptomator >() ) ;
	}

	custom::addEngines( m_backends ) ;

	for( size_t i = 1 ; i < m_backends.size() ; i++ ){

		m_backendWrappers.emplace_back( *( m_backends[ i ] ) ) ;
	}
}

template< typename Compare,typename listSource >
bool _found( const listSource& list,const Compare& cmp )
{
	for( const auto& xt : list ){

		if( cmp( xt ) ){

			return true ;
		}
	}

	return false ;
}

engines::engine::ownsCipherFolder engines::engine::ownsCipherPath( const QString& cipherPath,
								   const QString& configFilePath ) const
{
	auto a = this->name() + " " ;

	auto m = this->displayName() + " " ;

	if( cipherPath.startsWith( m,Qt::CaseInsensitive ) ){

		return { true,cipherPath.mid( m.size() ),configFilePath } ;

	}else if( cipherPath.startsWith( a,Qt::CaseInsensitive ) ){

		return { true,cipherPath.mid( a.size() ),configFilePath } ;

	}else if( utility::pathIsFile( cipherPath ) ){

		auto a = _found( this->fileExtensions(),[ & ]( const QString& e ){

			return cipherPath.endsWith( e ) ;
		} ) ;

		return { a,cipherPath,configFilePath } ;

	}else if( configFilePath.isEmpty() ){

		QString configPath ;

		auto a = _found( this->configFileNames(),[ & ]( const QString& e ){

			configPath = cipherPath + "/" + e ;

			return utility::pathExists( configPath ) ;
		} ) ;

		return { a,cipherPath,std::move( configPath ) } ;
	}else{
		auto ee = [ & ]( const QString& e ){

			return configFilePath.endsWith( e ) ;
		} ;

		if( _found( this->configFileNames(),ee ) ){

			return { true,cipherPath,configFilePath } ;
		}else{
			auto a = "[[[" + this->name() + "]]]" ;

			if( configFilePath.startsWith( a ) ){

				return { true,cipherPath,configFilePath.mid( a.size() ) } ;
			}else{
				return { false,QString(),QString() } ;
			}
		}
	}
}

engines::engineWithPaths engines::getByPaths( const QString& cipherPath,
					      const QString& configFilePath ) const
{
	for( size_t i = 1 ; i < m_backends.size() ; i++ ){

		const auto& m = m_backends[ i ] ;

		auto mm = m->ownsCipherPath( cipherPath,configFilePath ) ;

		if( mm.yes ){

			return { *m,std::move( mm ) } ;
		}
	}

	return {} ;
}

const engines::engine& engines::getByFsName( const QString& e ) const
{
	auto cmp = [ & ]( const QString& s ){ return !e.compare( s,Qt::CaseInsensitive ) ; } ;

	for( size_t i = 1 ; i < m_backends.size() ; i++ ){

		const auto& m = m_backends[ i ] ;

		if( _found( m->fuseNames(),cmp ) ){

			return *m ;
		}
	}

	return this->getUnKnown() ;
}

const engines::engine& engines::getByName( const QString& e ) const
{
	auto cmp = [ & ]( const QString& s ){ return !e.compare( s,Qt::CaseInsensitive ) ; } ;

	for( size_t i = 1 ; i < m_backends.size() ; i++ ){

		const auto& m = m_backends[ i ] ;

		const auto& s = m->displayName() ;

		if( s.isEmpty() ){

			if( _found( m->names(),cmp ) ){

				return *m ;
			}

		}else if( !s.compare( e,Qt::CaseInsensitive ) ){

			return *m ;
		}
	}

	return this->getUnKnown() ;
}

engines::engineWithPaths::engineWithPaths()
{
}

engines::engineWithPaths::engineWithPaths( const QString& e ) :
	m_engine( engines::instance().getByName( e ) )
{
}

engines::engineWithPaths::engineWithPaths( const QString& cipherPath,
					   const QString& configFilePath )
{
	*this = engines::instance().getByPaths( cipherPath,configFilePath ) ;
}

engines::engineWithPaths::engineWithPaths( const engines::engine& e,
					   engines::engine::ownsCipherFolder s ) :
	m_engine( e ),
	m_cipherPath( std::move( s.cipherPath ) ),
	m_configPath( std::move( s.configPath ) )
{
}

engines::engineWithPaths::engineWithPaths( const engines::engine& e,
					   const QString& cipherPath,
					   const QString& configFilePath ) :
	m_engine( e ),
	m_cipherPath( cipherPath ),
	m_configPath( configFilePath )
{
}

engines::engine::cmdStatus::cmdStatus()
{
}

engines::engine::cmdStatus::cmdStatus( engines::engine::status s,
				       const engines::engine& n,
				       const QString& e ) :
	m_status( s ),m_message( e ),m_engine( n )
{
	while( true ){

		if( m_message.endsWith( '\n' ) ){

			m_message.truncate( m_message.size() - 1 ) ;
		}else{
			break ;
		}
	}
}

engines::engine::status engines::engine::cmdStatus::status() const
{
	return m_status ;
}

bool engines::engine::cmdStatus::operator==( engines::engine::status s ) const
{
	return m_status == s ;
}

bool engines::engine::cmdStatus::operator!=( engines::engine::status s ) const
{
	return m_status != s ;
}

QString engines::engine::cmdStatus::toMiniString() const
{
	return m_message ;
}

const engines::engine& engines::engine::cmdStatus::engine() const
{
	return m_engine.get() ;
}

bool engines::engine::cmdStatus::success() const
{
	return m_status == engines::engine::status::success ;
}

QString engines::engine::cmdStatus::toString() const
{
	switch( m_status ){

	case engines::engine::status::success :

		/*
		 * Should not get here
		 */

		return "Success" ;

	case engines::engine::status::failedToUnMount :

		return QObject::tr( "Failed To Unmount %1 Volume" ).arg( m_engine->name() ) ;

	case engines::engine::status::volumeCreatedSuccessfully :

		return QObject::tr( "Volume Created Successfully." ) ;

	case engines::engine::status::backendRequiresPassword :

		return QObject::tr( "Backend Requires A Password." ) ;

	case engines::engine::status::badPassword :

		return QObject::tr( "Failed To Unlock \"%1\" Volume.\nWrong Password Entered." ).arg( m_engine->name() ) ;

	case engines::engine::status::fscryptKeyFileRequired :

		return QObject::tr( "This Fscrypt Volume Requires A KeyFile." ) ;

	case engines::engine::status::failedToStartPolkit :

		return QObject::tr( "Backend Requires Polkit Support and SiriKali Failed To Start It." ) ;

	case engines::engine::engine::status::IllegalPath :

		return QObject::tr( "A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit." ) ;

	case engines::engine::status::backEndDoesNotSupportCustomConfigPath :

		return QObject::tr( "Backend Does Not Support Custom Configuration File Path." ) ;

	case engines::engine::status::engineExecutableNotFound :

		return QObject::tr( "Failed To Complete The Request.\n%1 Executable Could Not Be Found." ).arg( m_engine->name() ) ;

	case engines::engine::status::backendTimedOut :

		return QObject::tr( "Something Is Wrong With The Backend And It Took Too Long To Respond." ) ;

	case engines::engine::status::cryfsMigrateFileSystem :

		return QObject::tr( "This Volume Of Cryfs Needs To Be Upgraded To Work With The Version Of Cryfs You Are Using.\n\nThe Upgrade is IRREVERSIBLE And The Volume Will No Longer Work With Older Versions of Cryfs.\n\nTo Do The Upgrade, Check The \"Upgrade File System\" Option And Unlock The Volume Again." ) ;

	case engines::engine::status::cryfsReplaceFileSystem :

		return QObject::tr( "This Volume Of Cryfs Is Different From The Known One.\n\nCheck The \"Replace File System\" Option And Unlock The Volume Again To Replace The Previous File System." ) ;

	case engines::engine::status::cryfsVersionTooOldToMigrateVolume :

		return QObject::tr( "Atleast Version 0.9.9 Of Cryfs Is Required To Be Able To Upgrade A Volume and Installed Version Is \"%1\"." ).arg( m_engine->installedVersion().toString() ) ;

	case engines::engine::status::notSupportedMountPointFolderPath :

		return QObject::tr( "Mount Point Folder Path Must Reside In An NTFS FileSystem." ) ;

	case engines::engine::status::mountPointFolderNotEmpty :

		return QObject::tr( "Mount Point Folder Path Is Not Empty." ) ;

	case engines::engine::status::ecryptfs_simpleNotFound :

		return QObject::tr( "Failed To Complete The Request.\nEcryptfs-simple Executable Could Not Be Found." ) ;

	case engines::engine::status::javaNotFound :

		return QObject::tr( "Failed To Complete The Request.\nJava Executable Could Not Be Found." ) ;

	case engines::engine::status::failedToCreateMountPoint :

		return QObject::tr( "Failed To Create Mount Point." ) ;

	case engines::engine::status::failedToLoadWinfsp :

		return QObject::tr( "Backend Could Not Load WinFsp. Please Make Sure You Have WinFsp Properly Installed." ) ;

	case engines::engine::status::unknown :

		return QObject::tr( "Failed To Unlock The Volume.\nNot Supported Volume Encountered." ) ;

	case engines::engine::status::backendFailedToMeetSiriKaliMinimumVersion :

		return QObject::tr( "Backend Requires Atleast Version \"%1\" Of SiriKali." ).arg( m_engine->sirikaliMinimumVersion() ) ;

	case engines::engine::status::backEndFailedToMeetMinimumRequirenment :

	{
		const auto& s = m_engine.get() ;

		const auto& a = s.name() ;
		const auto& b = s.minimumVersion() ;

		return QObject::tr( "Installed \"%1\" Version Is Too Old.\n Please Update To Atleast Version %2." ).arg( a,b ) ;
	}

	case engines::engine::status::fscryptPartialVolumeClose :

		return QObject::tr( "Folder Not Fully Locked Because Some Files Are Still In Use." ) ;

	case engines::engine::status::invalidConfigFileName :

	{
		const auto& e = m_engine.get().configFileNames() ;

		if( e.size() == 1 ){

			const auto& s = e.first() ;
			return QObject::tr( "Invalid Config File Name.\nIts Name Must End With \"%1\"" ).arg( s ) ;
		}else{
			auto s = e.join( ", " ) ;
			return QObject::tr( "Invalid Config File Name.\nIt Must End With One Of The Following:\n\"%1\"" ).arg( s ) ;
		}
	}

	case engines::engine::status::backendFail : break ;

	}

	auto e = QObject::tr( "Failed To Complete The Task And Below Log was Generated By The Backend.\n" ) ;
	return e + "\n----------------------------------------\n" + m_message ;
}

engines::engine::createGUIOptions::createOptions::createOptions( const QString& cOpts,
								 const QString& configFile,
								 const QString& keyFile,
								 const engines::engine::booleanOptions& r ) :
	configFile( configFile ),
	keyFile( keyFile ),
	opts( r ),
	success( true )
{
	if( !cOpts.isEmpty() ){

		createOpts = utility::split( cOpts,',' ) ;
	}
}

engines::engine::createGUIOptions::createOptions::createOptions( const QString& cOpts,
								 const QString& configFile,
								 const QString& keyFile ) :
	configFile( configFile ),
	keyFile( keyFile ),
	success( true )
{
	if( !cOpts.isEmpty() ){

		createOpts = utility::split( cOpts,',' ) ;
	}
}

engines::engine::createGUIOptions::createOptions::createOptions( const engines::engine::booleanOptions& r ) :
	opts( r )
{
}

engines::engine::createGUIOptions::createOptions::createOptions() : success( false )
{
}

engines::engine::mountGUIOptions::mountOptions::mountOptions( const favorites::entry& e ) :
	idleTimeOut( e.idleTimeOut ),
	configFile( e.configFilePath ),
	keyFile( e.keyFile ),
	identityFile( e.identityFile ),
	identityAgent( e.identityAgent )
{
	opts.unlockInReverseMode = e.reverseMode ;

	if( !e.mountOptions.isEmpty() ){

		mountOpts = utility::split( e.mountOptions,',' ) ;
	}
}

engines::engine::mountGUIOptions::mountOptions::mountOptions( const QString& idleTimeOut,
							      const QString& configFile,
							      const QString& mountOptions,
							      const QString& keyFile,
							      const QString& identityFile,
							      const QString& identityAgent,
							      const engines::engine::booleanOptions& r ) :
	idleTimeOut( idleTimeOut ),
	configFile( configFile ),
	keyFile( keyFile ),
	identityFile( identityFile ),
	identityAgent( identityAgent ),
	opts( r )
{
	if( !mountOptions.isEmpty() ){

		mountOpts = utility::split( mountOptions,',' ) ;
	}
}

engines::engine::mountGUIOptions::mountOptions::mountOptions() : success( false )
{
}

engines::engine::cmdArgsList::cmdArgsList( const favorites::entry& e,const QByteArray& volumeKey ) :
	cipherFolder( e.volumePath ),
	mountPoint( e.mountPointPath ),
	key( volumeKey ),
	idleTimeout( e.idleTimeOut ),
	configFilePath( e.configFilePath ),
	identityFile( e.identityFile ),
	identityAgent( e.identityAgent ),
	mountOptions( e.mountOptions.isEmpty() ? QStringList() : utility::split( e.mountOptions,',' ) )
{
	boolOptions.unlockInReadOnly    = e.readOnlyMode.defined() ? e.readOnlyMode.True() : false ;
	boolOptions.unlockInReverseMode = e.reverseMode ;
}

engines::engine::cmdArgsList::cmdArgsList( const QString& cipher_folder,
					   const QString& plain_folder,
					   const QByteArray& volume_key,
					   const engines::engine::createGUIOptions::createOptions& e ) :
	cipherFolder( cipher_folder ),
	mountPoint( plain_folder ),
	key( volume_key ),
	idleTimeout( e.idleTimeOut ),
	configFilePath( e.configFile ),
	createOptions( e.createOpts ),
	keyFile( e.keyFile ),
	boolOptions( e.opts )
{
}

engines::engine::cmdArgsList::cmdArgsList( const QString& cipher_folder,
					   const QString& plain_folder,
					   const QByteArray& volume_key,
					   const engines::engine::mountGUIOptions::mountOptions& e ) :
	cipherFolder( cipher_folder ),
	mountPoint( plain_folder ),
	key( volume_key ),
	idleTimeout( e.idleTimeOut ),
	configFilePath( e.configFile ),
	identityFile( e.identityFile ),
	identityAgent( e.identityAgent ),
	mountOptions( e.mountOpts ),
	createOptions( QString() ),
	keyFile( e.keyFile ),
	boolOptions( e.opts )
{
}

engines::engine::args::args( const engines::engine::cmdArgsList& m,
			     const engines::engine::commandOptions& s,
			     const QString& c,
			     const QStringList& l ) :
	cmd( c ),
	cipherPath( m.cipherFolder ),
	mountPath( m.mountPoint ),
	mode( s.mode() ),
	subtype( s.subType() ),
	cmd_args( l ),
	fuseOptions( s.constFuseOpts() )
{
}

engines::engine::args::args()
{
}

struct _args{

	const char * first ;
	const char * second ;

	template< typename T >
	static void replace( QString& e,const T& s )
	{
		for( const auto& it : s ){

			e.replace( it.first,it.second ) ;
		}
	}
} ;

static void _replace( QString& e )
{
	Q_UNUSED( e )
}

template< typename B,typename ... C >
static void _replace( QString& a,B&& b,C&& ... c )
{
	a.replace( b.first,b.second ) ;

	_replace( a,std::forward< C >( c ) ... ) ;
}

void engines::engine::encodeSpecialCharacters( QString& e )
{
	_replace( e,std::make_pair( ",","SiriKaliSpecialCharacter001" ) ) ;
}

void engines::engine::decodeSpecialCharacters( QString& e )
{
	_replace( e,std::make_pair( "SiriKaliSpecialCharacter001","," ),
		    std::make_pair( "\\012","\n" ),
		    std::make_pair( "\\040"," " ),
		    std::make_pair( "\\134","\\" ),
		    std::make_pair( "\\011","\\t" ) ) ;
}

QString engines::engine::encodeMountPath( const QString& e )
{
	auto m = e ;
	/*
	 * linux's /proc/self/mountinfo makes these substitutions and we make
	 * them too to be consistent with linux
	 */
#if 0
	_replace( m,std::make_pair( "\n","\\012" ),
		    std::make_pair( " ","\\040" ),
		    std::make_pair( "\\","\\134" ),
		    std::make_pair( "\\t","\\011" ) ) ;
#else
	_replace( m,std::make_pair( " ","\\040" ) ) ;
#endif
	return m ;
}

QString engines::engine::decodeSpecialCharactersConst( const QString& e )
{
	auto m = e ;

	engines::engine::decodeSpecialCharacters( m ) ;

	return m ;
}

engines::engine::commandOptions::commandOptions()
{
}

engines::engine::commandOptions::commandOptions( bool creating,
						 const engines::engine& engine,
						 const engines::engine::cmdArgsList& e )
{
	auto cipherFolder = [ & ]( QString s ){

		engines::engine::encodeSpecialCharacters( s ) ;

		return s ;
	} ;

	auto _volname = []( QString& e ){

		if( e.size() > 8 + 32 ){
			/*
			 * 8 is the size of "volname=" and 32 is the maximum size allowed
			 * to be stored in volname. If the value stored is more than 32
			 * characters, we truncate it to 29 characters and then add three dots.
			 */
			e = e.mid( 0,8 + 29 ) + "..." ;
		}
	} ;

	bool acceptsVolname = engine.acceptsVolName() ;

	bool hasNoVolname = true ;

	bool notLinux = !utility::platformIsLinux() ;

	m_fuseOptions = e.mountOptions ;

	for( int i = 0 ; i < m_fuseOptions.size() ; i++ ){

		auto& e = m_fuseOptions[ i ] ;

		if( e.startsWith( '-' ) ){

			if( engine.likeSsh() && e.startsWith( "-o " ) ){

				auto a = utility::split( e,' ' ) ;

				if( a.size() > 1 ){

					e = a.at( 1 ) ;
				}
			}else{
				m_exeOptions.append( utility::split( e,' ' ) ) ;

				m_fuseOptions.removeAt( i ) ;
			}

			i-- ;

		}else if( e.startsWith( "volname=" ) ){

			if( notLinux && acceptsVolname ){

				hasNoVolname = false ;

				_volname( e ) ;
			}else{
				m_fuseOptions.removeAt( i ) ;

				i-- ;
			}
		}
	}

	if( notLinux && hasNoVolname && acceptsVolname ){

		QString s ;

		if( utility::platformIsOSX() ){

			s = utility::split( e.mountPoint,'/' ).last() ;
		}else{
			s = utility::split( cipherFolder( e.cipherFolder ),'/' ).last() ;
		}

		if( !s.isEmpty() ){

			auto v = "volname=" + s ;

			_volname( v ) ;

			m_fuseOptions.append( v ) ;
		}
	}

	const auto& name = engine.name() ;

	if( engine.acceptsSubType() ){

		m_subtype = name ;

		m_fuseOptions.insert( 0,"subtype=" + m_subtype ) ;
	}

	auto m = QString( "fsname=%1@%2" ).arg( name,cipherFolder( e.cipherFolder ) ) ;

	m_fuseOptions.insert( 0,m ) ;

	if( e.boolOptions.unlockInReadOnly ){

		m_mode = "ro" ;
	}else{
		m_mode = "rw" ;
	}

	m_fuseOptions.insert( 0,m_mode ) ;

	m_fuseOptions.removeAll( QString() ) ;

	engine.updateOptions( *this,e,creating ) ;
}

engines::engineVersion::engineVersion() : m_valid( false )
{
}

engines::engineVersion::engineVersion( int major,int minor,int patch ) :
	m_valid( true ),m_major( major ),m_minor( minor ),m_patch( patch )
{
}

engines::engineVersion::engineVersion( const QString& e )
{
	auto s = utility::split( e,'.' ) ;

	int m = s.size() ;

	if( m == 1 ){

		m_major = s.at( 0 ).toInt( &m_valid ) ;

	}else if( m == 2 ){

		m_major = s.at( 0 ).toInt( &m_valid ) ;

		if( m_valid ){

			m_minor = s.at( 1 ).toInt( &m_valid ) ;
		}

	}else if( m >= 3 ) {

		m_major = s.at( 0 ).toInt( &m_valid ) ;

		if( m_valid ){

			m_minor = s.at( 1 ).toInt( &m_valid ) ;

			if( m_valid ){

				m_patch = s.at( 2 ).toInt( &m_valid ) ;
			}
		}
	}
}

bool engines::engineVersion::valid() const
{
	return m_valid ;
}

bool engines::engineVersion::operator==( const engines::engineVersion& other ) const
{
	return m_major == other.m_major && m_minor == other.m_minor && m_patch == other.m_patch ;
}

bool engines::engineVersion::operator<( const engines::engineVersion& other ) const
{
	if( m_major < other.m_major ){

		return true ;

	}else if( m_major == other.m_major ){

		if( m_minor < other.m_minor ){

			return true ;

		}else if( m_minor == other.m_minor ){

			return m_patch < other.m_patch ;
		}
	}

	return false ;
}

QString engines::engineVersion::toString() const
{
	auto a = QString::number( m_major ) ;
	auto b = QString::number( m_minor ) ;
	auto c = QString::number( m_patch ) ;

	return a + "." + b + "." + c ;
}

void engines::booleanCache::silenceWarning()
{
}

void engines::exeFullPath::silenceWarning()
{
}

template< typename ... T >
static bool _result( bool m,const engines::engine& engine,T&& ... t )
{
	const auto& e = engine.installedVersion() ;

	auto s = e.greaterOrEqual( std::forward< T >( t ) ... ) ;

	if( s.has_value() ){

		return s.value() ;
	}else{
		e.logError() ;

		return m ;
	}
}

bool engines::versionGreaterOrEqual::setCallback( bool m,const engines::engine& engine,
						  int major,int minor,int patch )
{
	return _result( m,engine,major,minor,patch ) ;
}

bool engines::versionGreaterOrEqual::setCallback( bool m,
						  const engines::engine& engine,
						  const QString& u )
{
	return _result( m,engine,u ) ;
}

void engines::versionGreaterOrEqual::silenceWarning()
{
}
