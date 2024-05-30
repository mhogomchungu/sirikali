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

#include "gocryptfs.h"
#include "custom.h"

#include "gocryptfscreateoptions.h"
#include "options.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = true ;
	s.backendRequireMountPath     = true ;
	s.backendRunsInBackGround     = true ;
	s.autoCreatesMountPoint       = false ;
	s.autoDeletesMountPoint       = false ;
	s.usesOnlyMountPoint          = false ;
	s.usesFuseArgumentSwitch      = true ;
	s.likeSsh               = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = true ;
	s.autoMountsOnCreate    = false ;
	s.hasGUICreateOptions   = true ;
	s.setsCipherPath        = true ;
	s.acceptsVolName        = false ;
	s.releaseURL             = "https://api.github.com/repos/rfjakob/gocryptfs/releases" ;
	s.idleString            = "-idle %{timeout}" ;
	s.configFileArgument    = "--config %{configFilePath}" ;
	s.configFileNames       = QStringList{ "gocryptfs.conf",
					       ".gocryptfs.conf",
					       ".gocryptfs.reverse.conf",
					       "gocryptfs.reverse.conf" } ;
	s.fuseNames             = QStringList{ "fuse.gocryptfs","fuse.gocryptfs-reverse" } ;
	s.names                 = QStringList{ "gocryptfs","gocryptfs.reverse","gocryptfs-reverse" } ;
	s.notFoundCode          = engines::engine::status::engineExecutableNotFound ;
	s.versionInfo           = { { "--version",true,1,0 } } ;

	if( utility::platformIsWindows() ){

		auto aa = engines::executableNotEngineFullPath( "sirikali_cppcryptfs.exe" ) ;
		auto bb = engines::executableNotEngineFullPath( "cppcryptfsctl.exe" ) ;

		s.windowsCanUnlockInReadWriteMode = true ;

		s.createControlStructure = "--create %{createOptions} --cipherPath %{cipherFolder}" ;

		s.windowsUnMountCommand = QStringList{ aa,"--umount","--mountPath","%{mountPoint}","--cppcryptfsctl-path",bb } ;

		s.displayName           = "cppcryptfs" ;

		s.acceptsSubType        = true ;

		s.reverseString         = "--reverse" ;

		s.volumePropertiesCommands = QStringList{ "sirikali_cppcryptfs.exe --info --mountPath %{plainFolder} --cppcryptfsctl-path \"" + bb + "\"" } ;

		s.passwordFormat        = "%{password}" ;

		s.successfulMountedList = QStringList{ "Mount Success" } ;
		s.failedToMountList     = QStringList{ "Failed To Mount" } ;

		s.incorrectPasswordText = QStringList{ "cppcryptfs: password incorrect" } ;

		s.executableNames = QStringList{ "cppcryptfsctl.exe" } ;
		s.mountControlStructure  = "--mount %{mountOptions} --cipherPath %{cipherFolder} --mountPath %{mountPoint} %{fuseOpts}" ;
	}else{
		s.volumePropertiesCommands = QStringList{ "gocryptfs -info %{cipherFolder}",
							  "gocryptfs -info %{plainFolder}" } ;

		s.passwordFormat         = "%{password}" ;

		s.createControlStructure = "-q --init %{createOptions} %{cipherFolder}" ;

		s.reverseString         = "-reverse" ;

		s.acceptsSubType        = false ;

		s.incorrectPasswordText = QStringList{ "Password incorrect." } ;

		s.mountControlStructure  = "-q %{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		s.executableNames = QStringList{ "gocryptfs" } ;
	}

	return s ;
}

static QString _to_native_path( const QString& e )
{
	return QDir::toNativeSeparators( e ) ;
}

#ifdef Q_OS_WIN

gocryptfs::gocryptfs() :
	engines::engine( _setOptions() ),
	m_sirikaliCppcryptfsExe( _to_native_path( engines::executableNotEngineFullPath( "sirikali_cppcryptfs.exe" ) ) ),
	m_cppcryptfsctl( _to_native_path( engines::executableNotEngineFullPath( "cppcryptfsctl.exe" ) ) ),
	m_cppcryptfs( QString( m_cppcryptfsctl ).replace( "cppcryptfsctl","cppcryptfs" ) ),
	m_windowsUnmountCommand( engines::engine::windowsUnmountCommand() + QStringList{ "--exit" } )
{
}

#else

gocryptfs::gocryptfs() : engines::engine( _setOptions() ),
	m_version_has_error_codes( true,*this,1,2,1 )
{
}

#endif

template< typename Function >
static bool _set_if_found( const Function& function )
{
	std::array< QString,3 > m = { "gocryptfs.reverse.conf",
				      ".gocryptfs.reverse.conf",
				      "gocryptfs.reverse" } ;
	for( const auto& it : m ){

		if( function( it ) ){

			return true ;
		}
	}

	return false ;
}

void gocryptfs::updateOptions( engines::engine::cmdArgsList& opt,bool creating ) const
{
	if( creating ){

		if( utility::platformIsWindows() ){

			opt.cipherFolder   = _to_native_path( opt.cipherFolder ) ;
			opt.mountPoint     = _to_native_path( opt.mountPoint ) ;
			opt.configFilePath = _to_native_path( opt.configFilePath ) ;
		}
		if( opt.boolOptions.unlockInReverseMode ){

			opt.createOptions.append( this->reverseString() ) ;
		}
	}else{
		if( utility::platformIsWindows() ){

			if( opt.configFilePath.isEmpty() ){

				opt.configFilePath = opt.cipherFolder + "/" + this->configFileName() ;
			}

			opt.cipherFolder   = _to_native_path( opt.cipherFolder ) ;
			opt.mountPoint     = _to_native_path( opt.mountPoint ) ;
			opt.configFilePath = _to_native_path( opt.configFilePath ) ;
		}

		opt.boolOptions.unlockInReverseMode = [ & ](){

			if( opt.configFilePath.isEmpty() ){

				return _set_if_found( [ & ]( const QString& e ){

					return utility::pathExists( opt.cipherFolder + "/" + e ) ;
				} ) ;
			}else{
				return _set_if_found( [ & ]( const QString& e ){

					return opt.configFilePath.endsWith( e ) ;
				} ) ;
			}
		}() ;
	}
}

engines::engine::status gocryptfs::errorCode( const QString& e,const QString& err,int s ) const
{
#ifdef Q_OS_WIN
	/*
	 * This error code was added in gocryptfs 1.2.1
	 */

	if( s == 12 ){

		return engines::engine::status::badPassword ;
	}

	return engines::engine::errorCode( e,err,s ) ;
#else
	if( m_version_has_error_codes ){

		/*
		 * This error code was added in gocryptfs 1.2.1
		 */

		if( s == 12 ){

			return engines::engine::status::badPassword ;
		}
	}

	return engines::engine::errorCode( e,err,s ) ;
#endif
}

void gocryptfs::GUICreateOptions( const engines::engine::createGUIOptions& s ) const
{
	gocryptfscreateoptions::instance( *this,s ) ;
}

void gocryptfs::GUIMountOptions( const engines::engine::mountGUIOptions& s ) const
{
	auto& e = options::instance( *this,s ) ;

	auto& ee = e.GUIOptions() ;

	ee.enableKeyFile = false ;

	ee.checkBoxChecked = s.mOpts.opts.unlockInReverseMode ;

	ee.updateOptions = []( const ::options::Options& s ){

		engines::engine::booleanOptions e ;

		e.unlockInReverseMode = s.checkBoxChecked ;

		return e ;
	} ;

	e.ShowUI() ;
}

void gocryptfs::parseVolumeProperties( QString& e ) const
{
	while( e.contains( ":  " ) ){

		e.replace( ":  ",": " ) ;
	}
}

void gocryptfs::aboutToExit() const
{
	if( utility::platformIsWindows() && !m_cppcryptfsPid.isEmpty() ){

		auto id = this->getcppcryptfsPid() ;

		if( m_cppcryptfsPid == id ){

			Task::process::run( m_cppcryptfsctl,QStringList{ "--exit" } ).await() ;
		}
	}
}

const QStringList& gocryptfs::windowsUnmountCommand() const
{
	if( m_cppcryptfsPid.isEmpty() ){

		return engines::engine::windowsUnmountCommand() ;
	}else{
		return m_windowsUnmountCommand ;
	}
}

QByteArray gocryptfs::prepareBackend() const
{
	if( utility::platformIsNOTWindows() ){

		return {} ;
	}

	auto m = this->getcppcryptfsPid() ;

	if( m.isEmpty() ){

		for( int i = 0 ; i < 5 ; i++ ){

			utility::debug() << "Starting An Instance Of Cppcryptfs" ;

			Task::process::run( m_cppcryptfs,QStringList{ "--tray" } ).start() ;

			utility::Task::suspendForOneSecond() ;

			m_cppcryptfsPid = this->getcppcryptfsPid() ;

			if( !m_cppcryptfsPid.isEmpty() ){

				auto a = "Cppcryptfs Status: Running" ;
				auto b = "\nManaging instance of cppcryptfs with a PID of " + m_cppcryptfsPid ;

				utility::debug() << a + b ;

				return {} ;
			}
		}

		return "Failed To Start An Instance of \"Cppcryptfs\"" ;
	}else{
		QString a = "Cppcryptfs Status: Running\n" ;

		if( m_cppcryptfsPid.isEmpty() ){

			utility::debug() << a + "Cppcryptfs is running and somebody else started it" ;
		}else{
			if( m == m_cppcryptfsPid ){

				utility::debug() << a + "Cppcryptfs is running and we started it" ;
			}else{
				utility::debug() << a + "Cppcryptfs is running and it was restarted by somebody else" ;
				m_cppcryptfsPid.clear() ;
			}
		}

		return {} ;
	}
}

engines::engine::args gocryptfs::command( const QByteArray& password,
					  const cmdArgsList& args,
					  bool create ) const
{
	if( utility::platformIsWindows() ){

		auto m = engines::engine::command( password,args,create ) ;

		m.cmd = m_sirikaliCppcryptfsExe ;
		m.cmd_args.append( "--cppcryptfsctl-path" ) ;
		m.cmd_args.append( m_cppcryptfsctl ) ;

		return m ;
	}else{
		return engines::engine::command( password,args,create ) ;
	}
}

QString gocryptfs::getcppcryptfsPid() const
{
	auto m = Task::process::run( "taskList",{ "/fi","IMAGENAME eq cppcryptfs.exe" } ).await() ;

	auto s = m.std_out() ;

	if( s.contains( "cppcryptfs.exe" ) ){

		const auto e = utility::split( s,'\n' ) ;

		for( const auto& it : e ){

			if( it.startsWith( "cppcryptfs.exe" ) ){

				auto ss = utility::split( it,' ' ) ;

				if( ss.size() > 1 ){

					return ss[ 1 ] ;
				}
			}
		}
	}

	return {} ;
}
