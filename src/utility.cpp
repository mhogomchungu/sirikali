/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utility.h"

#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <cstdio>
#ifndef _WIN32
#include <termios.h>
#endif
#include <memory>
#include <iostream>

#include <QObject>
#include <QDir>
#include <QFileDialog>
#include <QTranslator>
#include <QEventLoop>
#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <QFile>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcessEnvironment>
#include <QtNetwork/QLocalSocket>
#include <unistd.h>
#include <QTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QEvent>
#include <QKeyEvent>

#include "utility2.h"
#include "install_prefix.h"
#include "locale_path.h"
#include "plugins.h"
#include "json.h"
#include "winfsp.h"
#include "readonlywarning.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <gcrypt.h>

#include "lxqt_wallet.h"

#include "dialogmsg.h"
#include "plugin.h"
#include "siriPolkit.h"

#include "version.h"

#ifdef __linux__

#include <sys/vfs.h>

bool utility::platformIsLinux()
{
	return true ;
}

bool utility::platformIsOSX()
{
	return false ;
}

bool utility::platformIsWindows()
{
	return false ;
}

#elif __APPLE__

#include <sys/param.h>
#include <sys/mount.h>

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsOSX()
{
	return true ;
}

bool utility::platformIsWindows()
{
    return false ;
}

#else

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsOSX()
{
	return false ;
}

bool utility::platformIsWindows()
{
	return true ;
}
#endif

static QSettings * _settings ;
static QString _securefsPath ;
static QString _winfspPath ;
static QByteArray _cookie ;
static QString _polkit_socket_path ;

static bool _use_polkit = false ;

static std::function< void() > _failed_to_connect_to_zulupolkit ;

#if QT_VERSION > QT_VERSION_CHECK( 5,0,0 )
	#include <QStandardPaths>
	QString utility::socketPath()
	{
		return QStandardPaths::writableLocation( QStandardPaths::RuntimeLocation ) ;
	}
#else
	#include <QDesktopServices>
	QString utility::socketPath()
	{
		return QDesktopServices::storageLocation( QDesktopServices::DataLocation ) ;
	}
#endif

static bool _enable_debug = false ;

void utility::enableDebug( bool e )
{
	_enable_debug = e ;
}

bool utility::debugEnabled()
{
	return _enable_debug ;
}

static bool _enable_full_debug = false ;

void utility::enableFullDebug( bool e )
{
	_enable_full_debug = e ;
}

bool utility::debugFullEnabled()
{
	return _enable_full_debug ;
}

void utility::polkitFailedWarning( std::function< void() > e )
{
	_failed_to_connect_to_zulupolkit = std::move( e ) ;
}

::Task::future< utility::Task >& utility::Task::run( const QString& exe,bool e )
{
	return utility::Task::run( exe,-1,e ) ;
}

::Task::future< utility::Task >& utility::Task::run( const QString& exe,int s,bool e )
{
	return ::Task::run( [ = ](){

		auto env = utility::systemEnvironment() ;

		return utility::Task( exe,s,env,QByteArray(),[](){},e ) ;
	} ) ;
}

void utility::Task::execute( const QString& exe,int waitTime,
			     const QProcessEnvironment& env,
			     const QByteArray& password,
			     std::function< void() > function,
			     bool polkit )
{
	if( polkit && utility::useSiriPolkit() ){

		auto _report_error = [ this ]( const char * msg ){

			m_finished   = true ;
			m_exitCode   = -1 ;
			m_exitStatus = -1 ;
			m_stdError   =  msg ;
			//m_stdOut     =  "" ;
		} ;

		QLocalSocket s ;

		s.connectToServer( utility::helperSocketPath() ) ;

		for( int i = 0 ; ; i++ ){

			if( s.waitForConnected() ){

				break ;

			}else if( i == 3 ){

				_report_error( "SiriKali: Failed To Connect To Polkit Backend" ) ;

				utility::debug() << "ERROR: Failed To Start Helper Application" ;
				return ;
			}else{
				utility::debug() << s.errorString() ;

				utility::Task::suspendForOneSecond() ;
			}
		}

		s.write( [ & ]()->QByteArray{

			nlohmann::json json ;

			json[ "cookie" ]   = _cookie.constData() ;
			json[ "password" ] = password.constData() ;
			json[ "command" ]  = exe.toLatin1().constData() ;

			return json.dump().c_str() ;
		}() ) ;

		s.waitForBytesWritten() ;

		s.waitForReadyRead() ;

		try{
			auto json = nlohmann::json::parse( s.readAll().constData() ) ;

			m_finished   = json[ "finished" ].get< bool >() ;
			m_exitCode   = json[ "exitCode" ].get< int >() ;
			m_exitStatus = json[ "exitStatus" ].get< int >() ;
			m_stdError   = json[ "stdError" ].get< std::string >().c_str() ;
			m_stdOut     = json[ "stdOut" ].get< std::string >().c_str() ;

		}catch( ... ){

			_report_error( "SiriKali: Failed To Parse Polkit Backend Output" ) ;
		}
	}else{
		auto s = ::Task::process::run( exe,{},waitTime,password,env,std::move( function  ) ).get() ;

		m_finished   = s.finished() ;
		m_exitCode   = s.exit_code() ;
		m_exitStatus = s.exit_status() ;
		m_stdOut     = s.std_out() ;
		m_stdError   = s.std_error() ;
	}
}

static QString siriPolkitExe()
{
	auto exe = utility::executableFullPath( "pkexec" ) ;

	if( exe.isEmpty() ){

		return QString() ;
	}else{
		return QString( "%1 %2 %3 fork" ).arg( exe,siriPolkitPath,utility::helperSocketPath() ) ;
	}
}

static ::Task::future< utility::Task >& _start_siripolkit( const QString& e )
{
	_cookie = plugins::getRandomData( 16 ).toHex() ;

	return ::Task::run( [ = ]{

		return utility::Task( e,
				      -1,
				      utility::systemEnvironment(),
				      _cookie,
				      [](){},
				      false ) ;
	} ) ;
}

bool utility::enablePolkit( utility::background_thread thread )
{
	if( _use_polkit ){

		return true ;
	}

	auto exe = siriPolkitExe() ;

	if( !exe.isEmpty() ){

		auto socketPath = utility::helperSocketPath() ;

		if( thread == utility::background_thread::True ){

			if( _start_siripolkit( exe ).get().success() ){

				_use_polkit = true ;

				while( !utility::pathExists( socketPath ) ){

					utility::Task::waitForOneSecond() ;
				}
			}
		}else{
			if( _start_siripolkit( exe ).await().success() ){

				_use_polkit = true ;

				while( !utility::pathExists( socketPath ) ){

					utility::Task::suspendForOneSecond() ;
				}
			}
		}
	}

	return _use_polkit ;
}

void utility::initGlobals()
{
#ifdef __linux__
	auto uid = getuid() ;

	QString a = "/tmp/SiriKali-" + QString::number( uid ) ;

	_polkit_socket_path = a + "/siriPolkit.socket" ;

	QDir e ;

	e.remove( a ) ;
	e.rmdir( _polkit_socket_path ) ;

	e.mkpath( a ) ;

	auto s = a.toLatin1() ;

	if( chown( s.constData(),uid,uid ) ){}
	if( chmod( s.constData(),0700 ) ){}
#endif

#ifdef _WIN32
	auto e = "SOFTWARE\\WOW6432Node\\WinFsp\\Services\\securefs" ;
	_securefsPath = SiriKali::Winfsp::readRegister( e,"Executable" ) ;
	_winfspPath = SiriKali::Winfsp::readRegister( "SOFTWARE\\WOW6432Node\\WinFsp","InstallDir" ) ;
#endif
}

QString utility::helperSocketPath()
{
	return _polkit_socket_path ;
}

bool utility::useSiriPolkit()
{
	return _use_polkit ;
}

void utility::quitHelper()
{
#if __linux__
	auto e = utility::helperSocketPath() ;

	if( utility::pathExists( e ) ){

		QLocalSocket s ;

		s.connectToServer( e ) ;

		if( s.waitForConnected() ){

			s.write( [ & ]()->QByteArray{

				nlohmann::json json ;

				json[ "cookie" ]   = _cookie.constData() ;
				json[ "password" ] = "" ;
				json[ "command" ]  = "exit" ;

				return json.dump().c_str() ;
			}() ) ;

			s.waitForBytesWritten() ;
		}
	}

	auto a = "/tmp/SiriKali-" + QString::number( getuid() ) ;

	QDir m ;
	m.remove( a ) ;
	m.rmdir( _polkit_socket_path ) ;
#endif
}

QString utility::homePath()
{
	return QDir::homePath() ;
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener )
{
	return ::Task::run( [ = ](){

		auto e = opener + " " + utility::Task::makePath( path ) ;

		return utility::Task::run( e ).get().failed() ;
	} ) ;
}

void utility::openPath( const QString& path,const QString& opener,
			QWidget * obj,const QString& title,const QString& msg )
{
	if( !path.isEmpty() ){

		openPath( path,opener ).then( [ title,msg,obj ]( bool failed ){

			if( failed && obj ){

				DialogMsg( obj ).ShowUIOK( title,msg ) ;
			}
		} ) ;
	}
}

::Task::future< utility::fsInfo >& utility::fileSystemInfo( const QString& q )
{
	return ::Task::run( [ = ](){

		Q_UNUSED( q ) ;

		utility::fsInfo s ;

#ifndef WIN32
		struct statfs e ;

		s.valid = statfs( q.toLatin1().constData(),&e ) == 0 ;

		s.f_bavail = e.f_bavail ;
		s.f_bfree  = e.f_bfree ;
		s.f_blocks = e.f_blocks ;
		s.f_bsize  = e.f_bsize ;
#endif
		return s ;
	} ) ;
}

void utility::setSettingsObject( QSettings * s )
{
	_settings = s ;
}

static bool _help()
{
	utility::debug() << VERSION_STRING << QObject::tr( "\n\
options:\n\
	-d   Path to where a volume to be auto unlocked/mounted is located.\n\
	-m   Tool to use to open a default file manager(default tool is xdg-open).\n\
	-e   Start the application without showing the GUI.\n\
	-b   A name of a backend to retrieve a password from when a volume is open from CLI.\n\
	     Supported backends are: \"internal\",\"stdin\",\"keyfile\",\"osxkeychain\",\"kwallet\" and \"libsecret\".\n\
	     The first three are always present but the rest are compile time dependencies.\n\
	     \"internal\" option causes SiriKali to read password from lxqt-wallet internal backend.\n\
	     \"stdin\" option causes SiriKali to read the password from standard input.\n\
	     \"keyfile\" option causes SiriKali to read the password from a file.\n\
	     \"libsecret\" option causes SiriKali to read password from lxqt-wallet libsecret backend.\n\
	     \"kwallet\" option causes SiriKali to read password from lxqt-wallet kwallet backend.\n\
	     \"osxkeychain\" option causes SiriKali to read password from lxqt-wallet OSX key chain backend.\n\
	-k   When opening a volume from CLI,a value of \"rw\" will open the volume in read\\write\n\
	     mode and a value of \"ro\" will open the volume in read only mode.\n\
	-z   Full path of the mount point to be used when the volume is opened from CLI.\n\
	     This option is optional.\n\
	-c   Set Volume Configuration File Path when a volume is opened from CLI.\n\
	-i   Set inactivity timeout(in minutes) to dismount the volume when mounted from CLI.\n\
	-o   Set mount options when mounting a volume from CLI.\n\
	-f   Path to keyfile.\n\
	-u   Unmount volume.\n\
	-p   Print a list of unlocked volumes.\n\
	-s   Option to trigger generation of password hash." ) ;

	return true ;
}

bool _printHelpOrVersionInfo( const QStringList& e )
{
	return  e.contains( "-h" )        ||
		e.contains( "-help" )     ||
		e.contains( "--help" )    ||
		e.contains( "-v" )        ||
		e.contains(  "-version" ) ||
		e.contains( "--version" ) ;
}

bool utility::printVersionOrHelpInfo( const QStringList& e )
{
	if( _printHelpOrVersionInfo( e ) ){

		return _help() ;
	}else{
		return false ;
	}
}

utility::wallet utility::getKey( const QString& keyID,LXQt::Wallet::Wallet& wallet,QWidget * widget )
{
	auto _getKey = []( LXQt::Wallet::Wallet& wallet,const QString& volumeID ){

		return ::Task::await( [ & ](){ return wallet.readValue( volumeID ) ; } ) ;
	} ;

	utility::wallet w{ false,false,"" } ;

	auto s = wallet.backEnd() ;

	if( s == LXQt::Wallet::BackEnd::internal ){

		auto walletName = utility::walletName() ;
		auto appName    = utility::applicationName() ;

		if( LXQt::Wallet::walletExists( s,walletName,appName ) ){

			if( wallet.opened() ){

				w.opened = true ;
			}else{
				wallet.setImage( QIcon( ":/sirikali" ) ) ;

				if( widget ){

					widget->hide() ;

					w.opened = wallet.open( walletName,appName ) ;

					widget->show() ;

				}else{
					w.opened = wallet.open( walletName,appName ) ;
				}
			}

			if( w.opened ){

				w.key = _getKey( wallet,keyID ) ;
			}
		}else{
			w.notConfigured = true ;
		}
	}else{
		w.opened = wallet.open( utility::walletName( s ),utility::applicationName() ) ;

		if( w.opened ){

			w.key = _getKey( wallet,keyID ) ;
		}
	}

	return w ;
}

bool utility::eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > function )
{
	if( watched == gui ){

		if( event->type() == QEvent::KeyPress ){

			auto keyEvent = static_cast< QKeyEvent* >( event ) ;

			if( keyEvent->key() == Qt::Key_Escape ){

				function() ;

				return true ;
			}
		}
	}

	return false ;
}

QStringList utility::executableSearchPaths()
{
	return utility2::executableSearchPaths() ;
}

QString utility::executableSearchPaths( const QString& e )
{
	if( e.isEmpty() ){

		return utility::executableSearchPaths().join( ":" ) ;
	}else{
		return e + ":" + utility::executableSearchPaths().join( ":" ) ;
	}
}

QString utility::executableFullPath( const QString& f )
{
	return utility2::executableFullPath( f,[]( const QString& e ){

		if( utility::platformIsWindows() && e == "securefs" ){

			return utility::securefsPath() ;
		}else{
			return QString() ;
		}
	} ) ;
}

QString utility::cmdArgumentValue( const QStringList& l,const QString& arg,const QString& defaulT )
{
	int j = l.size() ;

	auto _absolute_exe_path = []( const QString& exe ){

		auto e = utility::executableFullPath( exe ) ;

		if( e.isEmpty() ){

			return exe ;
		}else{
			return e ;
		}
	} ;

	for( int i = 0 ; i < j ; i++ ){

		if( l.at( i ) == arg ){

			auto e = [ & ](){

				if( i + 1 < j ){

					return l.at( i + 1 ) ;
				}else{
					return defaulT ;
				}
			} ;

			if( arg == "-m" ){

				return _absolute_exe_path( e() ) ;
			}else{
				return e() ;
			}
		}
	}

	if( defaulT == "xdg-open" ){

		return _absolute_exe_path( defaulT ) ;
	}else{
		return defaulT ;
	}
}

QString utility::getVolumeID( const QString& id,bool expand )
{
	Q_UNUSED( expand ) ;
	return id ;
}

void utility::clearFavorites()
{
	_settings->setValue( "FavoritesVolumes",QStringList() ) ;
}

void utility::replaceFavorite( const favorites::entry& e,const favorites::entry& f )
{
	QStringList l ;

	for( const auto& it : utility::readFavorites() ){

		if( it == e ){

			l.append( f.configString() ) ;
		}else{
			l.append( it.configString() ) ;
		}
	}

	if( !l.isEmpty() ){

		_settings->setValue( "FavoritesVolumes",l ) ;
	}
}

int utility::favoritesEntrySize()
{
	auto s = _settings->value( "FavoritesVolumes" ).toStringList() ;

	if( s.size() == 0 ){

		return 0 ;
	}else{
		return utility::split( s.first(),'\t' ).size() ;
	}
}

void utility::addToFavorite( const QStringList& e )
{
	if( !e.isEmpty() ){

		_settings->setValue( "FavoritesVolumes",[ & ](){

			auto q = utility::readFavorites() ;

			q.emplace_back( e ) ;

			QStringList l ;

			for( const auto& it : q ){

				l.append( it.configString() ) ;
			}

			return l ;
		}() ) ;
	}
}

std::vector< favorites::entry > utility::readFavorites()
{
	if( _settings->contains( "FavoritesVolumes" ) ){

		std::vector< favorites::entry > e ;

		for( const auto& it : _settings->value( "FavoritesVolumes" ).toStringList() ){

			e.emplace_back( it ) ;
		}

		return e ;
	}else{
		return {} ;
	}
}

favorites::entry utility::readFavorite( const QString& e )
{
	for( const auto& it : utility::readFavorites() ){

		if( it.volumePath == e ){

			return it ;
		}
	}

	return {} ;
}

void utility::removeFavoriteEntry( const favorites::entry& e )
{
	_settings->setValue( "FavoritesVolumes",[ & ](){

		QStringList l ;

		for( const auto& it : utility::readFavorites() ){

			if( it != e ){

				l.append( it.configString() ) ;
			}
		}

		return l ;
	}() ) ;
}

void utility::readFavorites( QMenu * m )
{
	m->clear() ;

	auto _add_action = [ m ]( const QString& e,const QString& s ){

		auto ac = new QAction( m ) ;

		ac->setText( e ) ;
		ac->setObjectName( s ) ;

		return ac ;
	} ;

	m->addAction( _add_action( QObject::tr( "Manage Favorites" ),"Manage Favorites" ) ) ;
	m->addAction( _add_action( QObject::tr( "Mount All" ),"Mount All" ) ) ;

	m->addSeparator() ;

	for( const auto& it : utility::readFavorites() ){

		const auto& e = it.volumePath ;

		m->addAction( _add_action( e,e ) ) ;
	}
}

void utility::licenseInfo( QWidget * parent )
{
	QString license = QString( "%1\n\n\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 2 of the License, or \
( at your option ) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.\n\
" ).arg( VERSION_STRING ) ;

	DialogMsg( parent,nullptr ).ShowUIInfo( QObject::tr( "about SiriKali" ),true,license ) ;
}

utility::windowDimensions utility::getWindowDimensions()
{
	QString defaults = "205 149 861 466 326 320 101 76" ;

	if( _settings->contains( "Dimensions" ) ){

		utility::windowDimensions e( _settings->value( "Dimensions" ).toString() ) ;

		if( e ){

			return e ;
		}else{
			utility::debug() << "failed to parse config option" ;
			return defaults ;
		}
	}else{
		_settings->setValue( "Dimensions",defaults ) ;
		return defaults ;
	}
}

void utility::setWindowDimensions( const utility::windowDimensions& e )
{
	_settings->setValue( "Dimensions",e.dimensions() ) ;
}

template< typename T >
static void _selectOption( QMenu * m,const T& opt )
{
	for( const auto& it : m->actions() ){

		it->setChecked( it->text().remove( "&" ) == opt ) ;
	}
}

void utility::setLocalizationLanguage( bool translate,QMenu * m,utility2::translator& e )
{
	auto r = utility::localizationLanguage().toLatin1() ;

	if( translate ){

		e.setLanguage( r ) ;
	}else{
		auto e = utility::directoryList( utility::localizationLanguagePath() ) ;

		for( auto& it : e ){

			m->addAction( it.remove( ".qm" ) )->setCheckable( true ) ;
		}

		_selectOption( m,r ) ;
	}
}

void utility::languageMenu( QMenu * m,QAction * ac,utility2::translator& s )
{
	auto e = ac->text().remove( '&' ) ;

	utility::setLocalizationLanguage( e ) ;

	utility::setLocalizationLanguage( true,m,s ) ;

	_selectOption( m,e ) ;
}

QString utility::localizationLanguage()
{
	if( _settings->contains( "Language" ) ){

		return _settings->value( "Language" ).toString() ;
	}else{
		QString s = "en_US" ;
		_settings->setValue( "Language",s ) ;
		return s ;
	}
}

void utility::setLocalizationLanguage( const QString& language )
{
	_settings->setValue( "Language",language ) ;
}

QString utility::localizationLanguagePath()
{
	return QString( TRANSLATION_PATH ) ;
}

QStringList utility::directoryList( const QString& e )
{
	QDir d( e ) ;

	auto s = d.entryList() ;

	s.removeOne( "." ) ;
	s.removeOne( ".." ) ;

	return s ;
}

QIcon utility::getIcon()
{
	if( utility::platformIsLinux() ){

		QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/sirikali.png" ) ;

		return QIcon::fromTheme( "sirikali",icon ) ;
	}else{
		return QIcon( ":sirikali" ) ;
	}
}

QString utility::homeConfigPath( const QString& e )
{
	return utility::homePath() + "/.SiriKali/" + e ;
}

bool utility::pathIsReadable( const QString& path,bool isFolder )
{
	QFileInfo s( path ) ;

	if( isFolder ){

		return s.isReadable() && s.isDir() ;
	}else{
		return s.isReadable() && s.isFile() ;
	}
}

bool utility::pathIsWritable( const QString& path,bool isFolder )
{
	QFileInfo s( path ) ;

	if( isFolder ){

		return s.isWritable() && s.isDir() ;
	}else{
		return s.isWritable() && s.isFile() ;
	}
}

bool utility::pathExists( const QString& path )
{
	return QFile::exists( path ) ;
}

QStringList utility::split( const QString& e,char token )
{
	return e.split( token,QString::SkipEmptyParts ) ;
}

QString utility::walletName()
{
	return "SiriKali" ;
}

QString utility::walletName( LXQt::Wallet::BackEnd s )
{
	if( s == LXQt::Wallet::BackEnd::kwallet ){

		if( _settings->contains( "KWalletName" ) ){

			return _settings->value( "KWalletName" ).toString() ;
		}else{
			QString s = "default" ;
			_settings->setValue( "KWalletName",s ) ;
			return s ;
		}
	}else{
		return utility::walletName() ;
	}
}

QString utility::applicationName()
{
	return "SiriKali" ;
}

static void _set_mount_default()
{
	if( !_settings->contains( "MountPrefix" ) ){

		_settings->setValue( "MountPrefix",utility::homePath() + "/.SiriKali" ) ;
	}
}

QString utility::mountPath()
{
	_set_mount_default() ;

	return _settings->value( "MountPrefix" ).toString() ;
}

QString utility::mountPath( const QString& path )
{
	_set_mount_default() ;

	return _settings->value( "MountPrefix" ).toString() + "/" + path ;
}

void utility::setDefaultMountPointPrefix( const QString& path )
{
	_settings->setValue( "MountPrefix",path ) ;
}

QString utility::mountPathPostFix( const QString& path )
{
	return utility::mountPathPostFix( utility::mountPath( path ),path ) ;
}

QString utility::mountPathPostFix( const QString& e,const QString& path )
{
	if( path.isEmpty() ){

		return path ;
	}else{
		auto _usable_mount_point = []( const QString& e ){

			return !utility::pathExists( e ) ;
		} ;

		if( _usable_mount_point( e ) ){

			return path ;
		}else{
			QString z ;

			for( int i = 1 ; i < 1000 ; i++ ){

				z = QString::number( i ) ;

				if( _usable_mount_point( QString( "%1_%2" ).arg( e,z ) ) ){

					return QString( "%1_%2" ).arg( path,z ) ;
				}
			}

			return path ;
		}
	}
}

bool utility::setOpenVolumeReadOnly( QWidget * parent,bool checked )
{
	return readOnlyWarning::showWarning( parent,checked ) ;
}

bool utility::getOpenVolumeReadOnlyOption()
{
	return readOnlyWarning::getOpenVolumeReadOnlyOption() ;
}

bool utility::reUseMountPoint()
{
	if( _settings->contains( "ReUseMountPoint" ) ){

		return _settings->value( "ReUseMountPoint" ).toBool() ;
	}else{
		bool e = false ;

		_settings->setValue( "ReUseMountPoint",e ) ;

		return e ;
	}
}

void utility::reUseMountPoint( bool e )
{
	_settings->setValue( "ReUseMountPoint",e ) ;
}

bool utility::autoOpenFolderOnMount()
{
	if( _settings->contains( "AutoOpenFolderOnMount" ) ){

		return _settings->value( "AutoOpenFolderOnMount" ).toBool() ;
	}else{
		bool s = true ;
		utility::autoOpenFolderOnMount( s ) ;
		return s ;
	}
}

void utility::autoOpenFolderOnMount( bool e )
{
	_settings->setValue( "AutoOpenFolderOnMount",e ) ;
}

bool utility::autoCheck()
{
	if( _settings->contains( "AutoCheckForUpdates" ) ){

		return _settings->value( "AutoCheckForUpdates" ).toBool() ;
	}else{
		bool s = false ;
		utility::autoCheck( s ) ;
		return s ;
	}
}

void utility::autoCheck( bool e )
{
	_settings->setValue( "AutoCheckForUpdates",e ) ;
}

bool utility::readOnlyWarning()
{
	if( _settings->contains( "ReadOnlyWarning" ) ){

		return _settings->value( "ReadOnlyWarning" ).toBool() ;
	}else{
		bool s = false ;
		utility::readOnlyWarning( s ) ;
		return s ;
	}
}

void utility::readOnlyWarning( bool e )
{
	_settings->setValue( "ReadOnlyWarning",e ) ;
}

bool utility::doNotShowReadOnlyWarning()
{
	if( _settings->contains( "DoNotShowReadOnlyWarning" ) ){

		return _settings->value( "DoNotShowReadOnlyWarning" ).toBool() ;
	}else{
		bool s = false ;
		utility::doNotShowReadOnlyWarning( s ) ;
		return s ;
	}
}

void utility::doNotShowReadOnlyWarning( bool e )
{
	_settings->setValue( "DoNotShowReadOnlyWarning",e ) ;
}

bool utility::autoMountFavoritesOnStartUp()
{
	if( _settings->contains( "AutoMountFavoritesOnStartUp" ) ){

		return _settings->value( "AutoMountFavoritesOnStartUp" ).toBool() ;
	}else{
		bool s = false ;
		utility::autoMountFavoritesOnStartUp( s ) ;
		return s ;
	}
}

void utility::autoMountFavoritesOnStartUp( bool e )
{
	_settings->setValue( "AutoMountFavoritesOnStartUp",e ) ;
}

void utility::autoMountBackEnd( const utility::walletBackEnd& e )
{
	_settings->setValue( "AutoMountPassWordBackEnd",[ & ]()->QString{

		if( e.isInvalid() ){

			return "none" ;

		}else if( e == LXQt::Wallet::BackEnd::internal ){

			return "internal" ;

		}else if( e == LXQt::Wallet::BackEnd::libsecret ){

			return "libsecret" ;

		}else if( e == LXQt::Wallet::BackEnd::kwallet ){

			return "kwallet" ;

		}else if( e == LXQt::Wallet::BackEnd::osxkeychain ){

			return "osxkeychain" ;
		}else{
			return "none" ;
		}
	}() ) ;
}

utility::walletBackEnd utility::autoMountBackEnd()
{
	if( _settings->contains( "AutoMountPassWordBackEnd" ) ){

		auto e = _settings->value( "AutoMountPassWordBackEnd" ).toString() ;

		if( e == "libsecret" ){

			return LXQt::Wallet::BackEnd::libsecret ;

		}else if( e == "kwallet" ){

			return LXQt::Wallet::BackEnd::kwallet ;

		}else if( e == "internal" ){

			return LXQt::Wallet::BackEnd::internal ;

		}else if( e == "osxkeychain" ){

			return LXQt::Wallet::BackEnd::osxkeychain ;
		}else{
			return utility::walletBackEnd() ;
		}
	}else{
		_settings->setValue( "AutoMountPassWordBackEnd",QString( "none" ) ) ;
		return utility::walletBackEnd() ;
	}
}

void utility::autoMountFavoritesOnAvailable( bool e )
{
	_settings->setValue( "AutoMountFavoritesOnAvailable",e ) ;
}

bool utility::autoMountFavoritesOnAvailable()
{
	if( _settings->contains( "AutoMountFavoritesOnAvailable" ) ){

		return _settings->value( "AutoMountFavoritesOnAvailable" ).toBool() ;
	}else{
		utility::autoMountFavoritesOnAvailable( false ) ;
		return false ;
	}
}

int utility::networkTimeOut()
{
	if( _settings->contains( "NetworkTimeOut" ) ){

		return _settings->value( "NetworkTimeOut" ).toInt() ;
	}else{
		int s = 5 ;
		_settings->setValue( "NetworkTimeOut",s ) ;
		return s ;
	}
}

bool utility::showMountDialogWhenAutoMounting()
{
	if( _settings->contains( "ShowMountDialogWhenAutoMounting" ) ){

		return _settings->value( "ShowMountDialogWhenAutoMounting" ).toBool() ;
	}else{
		utility::showMountDialogWhenAutoMounting( false ) ;
		return false ;
	}
}

void utility::showMountDialogWhenAutoMounting( bool e )
{
	_settings->setValue( "ShowMountDialogWhenAutoMounting",e ) ;
}

int utility::checkForUpdateInterval()
{
	if( _settings->contains( "CheckForUpdateInterval" ) ){

		return _settings->value( "CheckForUpdateInterval" ).toInt() * 1000 ;
	}else{
		int s = 10 ;
		_settings->setValue( "CheckForUpdateInterval",s ) ;
		return s * 1000 ;
	}
}

void utility::setWindowOptions( QDialog * w )
{
	if( utility::platformIsOSX() ){

		w->setWindowFlags( w->windowFlags() | Qt::WindowStaysOnTopHint ) ;
		w->setWindowModality( Qt::WindowModal ) ;
	}
}

void utility::setParent( QWidget * parent,QWidget ** localParent,QDialog * dialog )
{
	auto _default_parent = [](){

		if( _settings->contains( "UseDefaultWidgetRelationship" ) ){

			return _settings->value( "UseDefaultWidgetRelationship" ).toBool() ;
		}else{
			bool e = true ;
			_settings->setValue( "UseDefaultWidgetRelationship",e ) ;
			return e ;
		}
	}() ;

	if( _default_parent ){

		*localParent = dialog ;
	}else{
		*localParent = parent ;
	}
}

void utility::scaleGUI()
{
#if QT_VERSION >= 0x050600

	bool e = [](){

		if( _settings->contains( "EnableHighDpiScaling" ) ){

			return _settings->value( "EnableHighDpiScaling" ).toBool() ;
		}else{
			bool s = false ;
			_settings->setValue( "EnableHighDpiScaling",s ) ;
			_settings->setValue( "EnabledHighDpiScalingFactor",QString( "1" ) ) ;
			return s ;
		}
	}() ;

	if( e ){

		QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;

		qputenv( "QT_SCALE_FACTOR",[](){

			if( _settings->contains( "EnabledHighDpiScalingFactor" ) ){

				return _settings->value( "EnabledHighDpiScalingFactor" ).toString().toLatin1() ;
			}else{
				_settings->setValue( "EnabledHighDpiScalingFactor",QString( "1" ) ) ;
				return QByteArray( "1" ) ;
			}
		}() ) ;
	}

#endif
}

bool utility::createFolder( const QString& m )
{
	return QDir().mkpath( m ) ;
}

QString utility::preUnMountCommand()
{
	if( !_settings->contains( "PreUnMountCommand" ) ){

		_settings->setValue( "PreUnMountCommand",QString() ) ;
	}

	return _settings->value( "PreUnMountCommand" ).toString() ;
}

void utility::preUnMountCommand( const QString& e )
{
	_settings->setValue( "PreUnMountCommand",e ) ;
}

void utility::runCommandOnMount( const QString& e )
{
	_settings->setValue( "RunCommandOnMount",e ) ;
}

QString utility::runCommandOnMount()
{
	if( _settings->contains( "RunCommandOnMount" ) ){

		return _settings->value( "RunCommandOnMount" ).toString() ;
	}else{
		QString s ;
		_settings->setValue( "RunCommandOnMount",s ) ;

		return s ;
	}
}

static QString _file_manager()
{
	auto s = utility::platformIsLinux() ? "xdg-open" : "open" ;

	auto e = utility::executableFullPath( s ) ;

	if( e.isEmpty() ){

		return s ;
	}else{
		return e ;
	}
}

void utility::setFileManager( const QString& e )
{
	if( e.isEmpty() ){

		_settings->setValue( "FileManagerOpener",_file_manager() ) ;
	}else{
		_settings->setValue( "FileManagerOpener",e ) ;
	}
}

QString utility::fileManager()
{
	if( _settings->contains( "FileManagerOpener" ) ){

		auto e = _settings->value( "FileManagerOpener" ).toString() ;

		if( e.isEmpty() ){

			utility::setFileManager( QString() ) ;

			return _settings->value( "FileManagerOpener" ).toString() ; ;
		}else{
			return e ;
		}
	}else{
		utility::setFileManager( QString() ) ;
		return _settings->value( "FileManagerOpener" ).toString() ;
	}
}

static int _readPasswordMaximumLength()
{
	if( _settings->contains( "ReadPasswordMaximumLength" ) ){

		return _settings->value( "ReadPasswordMaximumLength" ).toInt() ;
	}else{
		int s = 1024 ;

		_settings->setValue( "ReadPasswordMaximumLength",s ) ;

		return s ;
	}
}

bool utility::startMinimized()
{
	if( _settings->contains( "StartMinimized" ) ){

		return _settings->value( "StartMinimized" ).toBool() ;
	}else{
		bool s = false  ;

		_settings->setValue( "StartMinimized",s ) ;

		return s ;
	}
}

void utility::setStartMinimized( bool e )
{
	_settings->setValue( "StartMinimized",e ) ;
}

#ifndef WIN32

static inline bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old;
	current->c_lflag &= ~ECHO;

	if( tcsetattr( 1,TCSAFLUSH,current ) != 0 ){

		return false ;
	}else{
		return true ;
	}
}

QString utility::readPassword( bool addNewLine )
{
	std::cout << "Password: " << std::flush ;

	struct termios old ;
	struct termios current ;

	_terminalEchoOff( &old,&current ) ;

	QString s ;
	int e ;

	int m = _readPasswordMaximumLength() ;

	for( int i = 0 ; i < m ; i++ ){

		e = std::getchar() ;

		if( e == '\n' || e == -1 ){

			break ;
		}else{
			s += static_cast< char >( e ) ;
		}
	}

	tcsetattr( 1,TCSAFLUSH,&old ) ;

	if( addNewLine ){

		std::cout << std::endl ;
	}

	return s ;
}

#else

QString utility::readPassword( bool addNewLine )
{
	std::cout << "Password: " << std::flush ;

	QString s ;
	int e ;

	int m = _readPasswordMaximumLength() ;

	for( int i = 0 ; i < m ; i++ ){

		e = std::getchar() ;

		if( e == '\n' || e == -1 ){

			break ;
		}else{
			s += static_cast< char >( e ) ;
		}
	}

	if( addNewLine ){

		std::cout << std::endl ;
	}

	return s ;
}

#endif

QString utility::externalPluginExecutable()
{
	if( _settings->contains( "ExternalPluginExecutable" ) ){

		return _settings->value( "ExternalPluginExecutable" ).toString() ;
	}else{
		utility::setExternalPluginExecutable( QString() ) ;

		return _settings->value( "ExternalPluginExecutable" ).toString() ;
	}
}

void utility::setExternalPluginExecutable( const QString& e )
{
	if( e.isEmpty() ){

		QString s = "gpg --no-tty --yes --no-mdc-warning --no-verbose --passphrase-fd 0 -d" ;
		_settings->setValue( "ExternalPluginExecutable",s ) ;
	}else{
		_settings->setValue( "ExternalPluginExecutable",e ) ;
	}
}

bool utility::enableRevealingPasswords()
{
	if( _settings->contains( "EnableRevealingPasswords" ) ){

		return _settings->value( "EnableRevealingPasswords" ).toBool() ;
	}else{
		bool e = true ;

		_settings->setValue( "EnableRevealingPasswords",e ) ;

		return e ;
	}
}

QProcessEnvironment utility::systemEnvironment()
{
	auto e = QProcessEnvironment::systemEnvironment() ;

	e.insert( "CRYFS_NO_UPDATE_CHECK","TRUE" ) ;
	e.insert( "CRYFS_FRONTEND","noninteractive" ) ;

	e.insert( "LANG","C" ) ;

	e.insert( "PATH",utility::executableSearchPaths( e.value( "PATH" ) ) ) ;

	return e ;
}

void utility::windowDimensions::setDimensions( const QStringList& e )
{
	m_ok = int( e.size() ) == int( m_array.size() ) ;

	if( m_ok ){

		using tp = decltype( m_array.size() ) ;

		for( tp i = 0 ; i < m_array.size() ; i++ ){

			m_array[ i ] = e.at( i ).toInt( &m_ok ) ;

			if( !m_ok ){

				break ;
			}
		}
	}else{
		utility::debug() << "window dimensions do not match data structure size" ;
	}
}

utility::windowDimensions::windowDimensions( const QStringList& e )
{
	this->setDimensions( e ) ;
}

utility::windowDimensions::windowDimensions( const QString& e )
{
	this->setDimensions( utility::split( e,' ' ) ) ;
}

utility::windowDimensions::windowDimensions( const std::array< int,size >& e )
	: m_array( e ),m_ok( true )
{
}

utility::windowDimensions::operator bool()
{
	return m_ok ;
}

int utility::windowDimensions::columnWidthAt( std::array< int,size >::size_type s ) const
{
	auto e = s + 4 ;

	if( e < m_array.size() ){

		return m_array[ e ] ;
	}else{
		utility::debug() << "window dimension index out of range" ;
		return 0 ;
	}
}

QRect utility::windowDimensions::geometry() const
{
	auto e = m_array.data() ;

	return { *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) } ;
}

QString utility::windowDimensions::dimensions() const
{
	auto _number = []( const int * s,size_t n ){ return QString::number( *( s + n ) ) ; } ;

	auto s = m_array.data() ;

	auto e = _number( s,0 ) ;

	using tp = decltype( m_array.size() ) ;

	for( tp i = 1 ; i < m_array.size() ; i++ ){

		e += " " + _number( s,i ) ;
	}

	return e ;
}

QString utility::configFilePath( QWidget * s,const QString& e )
{
	return [ = ](){

		QFileDialog dialog( s ) ;

		dialog.setFileMode( QFileDialog::AnyFile ) ;

		dialog.setDirectory( utility::homePath() ) ;

		dialog.setAcceptMode( QFileDialog::AcceptSave ) ;

		dialog.selectFile( [ = ](){

			if( e == "cryfs" ){

				return "cryfs.config" ;

			}else if( e == "encfs" ){

				return "encfs6.xml" ;

			}else if( e == "gocryptfs" ){

				return "gocryptfs.conf" ;

			}else if( e == "securefs" ){

				return "securefs.json" ;

			}else if( e == "ecryptfs" ){

				return "ecryptfs.config" ;
			}else{
				return "" ;
			}
		}() ) ;

		if( dialog.exec() ){

			auto q = dialog.selectedFiles() ;

			if( !q.isEmpty() ){

				return q.first() ;
			}
		}

		return QString() ;
	}() ;
}

QString utility::getExistingDirectory( QWidget * w,const QString& caption,const QString& dir )
{
	auto e = QFileDialog::getExistingDirectory( w,caption,dir,QFileDialog::ShowDirsOnly ) ;

	while( true ){

		if( e == "/" ){

			break ;

		}else if( e.endsWith( '/' ) ){

			e.truncate( e.length() - 1 ) ;
		}else{
			break ;
		}
	}

	return e ;
}

void utility::setWindowsMountPointOptions( QWidget * obj,QLineEdit * e,QPushButton * s )
{
	auto menu = new QMenu( obj ) ;

	QList< QAction* > actions ;

	char m[ 3 ] = { 'G',':','\0' } ;

	for( ; *m < 'Z' ; *m += 1 ){

		auto ac = new QAction( m,obj ) ;
		ac->setObjectName( m ) ;

		actions.append( ac ) ;
	}

	QObject::connect( menu,&QMenu::triggered,[ e ]( QAction * ac ){

		e->setText( ac->objectName() ) ;
	} ) ;

	menu->addActions( actions ) ;

	s->setMenu( menu ) ;
	s->setIcon( QIcon( ":/harddrive.png" ) ) ;
}

QString utility::securefsPath()
{
	return _securefsPath ;
}

QString utility::winFSPpath()
{
	return _winfspPath ;
}

int utility::winFSPpollingInterval()
{
	if( !_settings->contains( "WinFSPpollingInterval" ) ){

		_settings->setValue( "WinFSPpollingInterval",2 ) ;
	}

	return _settings->value( "WinFSPpollingInterval" ).toInt() ;
}
