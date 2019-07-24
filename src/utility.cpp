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
#include <QStandardPaths>

#include "utility2.h"
#include "install_prefix.h"
#include "locale_path.h"
#include "plugins.h"
#include "crypto.h"
#include "json_parser.hpp"
#include "win.h"
#include "readonlywarning.h"

#include "lxqt_wallet.h"

#include "dialogmsg.h"
#include "plugin.h"
#include "siriPolkit.h"
#include "settings.h"
#include "version.h"
#include "runinthread.h"

#ifdef Q_OS_LINUX

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

#endif

#ifdef Q_OS_MACOS

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

#endif

#ifdef Q_OS_WIN

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

static QByteArray _cookie ;
static QString _polkit_socket_path ;

static debugWindow * _debugWindow ;

static bool _use_polkit = false ;

static std::function< void() > _failed_to_connect_to_zulupolkit ;

static bool _enable_debug = false ;

static bool _enable_full_debug = false ;

static QThread * _main_gui_thread ;

static QWidget * _mainQWidget ;

void utility::setMainQWidget( QWidget * m )
{
	_mainQWidget = m ;
}

QWidget * utility::mainQWidget()
{
	return _mainQWidget ;
}

void utility::enableDebug( bool e )
{
	_enable_debug = e ;
}

bool utility::debugEnabled()
{
	return _enable_debug ;
}

void utility::enableFullDebug( bool e )
{
	_enable_full_debug = e ;
}

bool utility::debugFullEnabled()
{
	return _enable_full_debug ;
}

void utility::setDebugWindow( debugWindow * w )
{
	_debugWindow = w ;
}

static void _set_debug_window_text( const QString& e )
{
	if( utility::debugFullEnabled() ){

		std::cout << e.toLatin1().constData() << std::endl ;
	}

	_debugWindow->UpdateOutPut( e,utility::debugFullEnabled() ) ;
}

utility::SocketPaths utility::socketPath()
{
	if( utility::platformIsWindows() ){

		return { QString(),"\\\\.\\pipe\\SiriKaliSocket" } ;
	}else{
		auto a = QStandardPaths::writableLocation( QStandardPaths::RuntimeLocation ) ;
		return { a,a + "/SiriKali.socket" } ;
	}
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

		const auto& env = utility::systemEnvironment() ;

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

				utility::waitForOneSecond() ;
			}
		}

		s.write( [ & ]()->QByteArray{

			sirikali::json json ;

			json[ "cookie" ]   = _cookie ;
			json[ "password" ] = password ;
			json[ "command" ]  = exe ;

			return json.structure() ;
		}() ) ;

		s.waitForBytesWritten() ;

		s.waitForReadyRead() ;

		try{
			sirikali::json json( s.readAll(),sirikali::json::type::CONTENTS ) ;

			m_finished   = json.getBool( "finished" ) ;
			m_exitCode   = json.getInterger( "exitCode" ) ;
			m_exitStatus = json.getInterger( "exitStatus" ) ;
			m_stdError   = json.getByteArray( "stdError" ) ;
			m_stdOut     = json.getByteArray( "stdOut" ) ;

			utility::logCommandOutPut( { m_stdOut,m_stdError,m_exitCode,m_exitStatus,m_finished },exe ) ;

		}catch( ... ){

			_report_error( "SiriKali: Failed To Parse Polkit Backend Output" ) ;
		}
	}else{
		auto& ss = ::Task::process::run( exe,{},waitTime,password,env,std::move( function ) ) ;

		auto s = utility::unwrap( ss ) ;

		m_finished   = s.finished() ;
		m_exitCode   = s.exit_code() ;
		m_exitStatus = s.exit_status() ;
		m_stdOut     = s.std_out() ;
		m_stdError   = s.std_error() ;

		utility::logCommandOutPut( s,exe ) ;
	}
}

static void _build_debug_msg( const QString& b )
{
	QString a = "***************************\n" ;
	QString c = "\n***************************" ;

	_set_debug_window_text( a + b + c ) ;
}

utility::debug utility::debug::operator<<( const QString& e )
{
	_build_debug_msg( e ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( int e )
{
	_build_debug_msg( QString::number( e ) ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const char * e )
{
	_build_debug_msg( e ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const QByteArray& e )
{
	_build_debug_msg( e ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const QStringList& e )
{
	_build_debug_msg( e.join( "\n" ) ) ;
	return utility::debug() ;
}

void utility::logCommandOutPut( const QString& e )
{
	_set_debug_window_text( "---------\n" + e + "\n---------\n" ) ;
}

void utility::logCommandOutPut( const ::Task::process::result& m,const QString& exe )
{
	auto _trim = []( QString e ){

		while( true ){

			if( e.endsWith( '\n' ) ){

				e.truncate( e.size() - 1 ) ;
			}else{
				break ;
			}
		}

		return e ;
	} ;

	QString s = "Exit Code: %1\nExit Status: %2\n-------\nStdOut: %3\n-------\nStdError: %4\n-------\nCommand: %5\n-------\n" ;

	auto e = s.arg( QString::number( m.exit_code() ),
			QString::number( m.exit_status() ),
			_trim( m.std_out() ),
			_trim( m.std_error() ),
			exe ) ;

	_set_debug_window_text( e ) ;
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
	_cookie = crypto::getRandomData( 16 ).toHex() ;

	return ::Task::run( [ = ]{

		return utility::Task( e,
				      -1,
				      utility::systemEnvironment(),
				      _cookie,
				      [](){},
				      false ) ;
	} ) ;
}

bool utility::enablePolkit()
{
	if( _use_polkit ){

		return true ;
	}

	auto exe = siriPolkitExe() ;

	if( !exe.isEmpty() ){

		auto socketPath = utility::helperSocketPath() ;

		if( utility::unwrap( _start_siripolkit( exe ) ).success() ){

			_use_polkit = true ;

			while( !utility::pathExists( socketPath ) ){

				utility::waitForOneSecond() ;
			}
		}
	}

	return _use_polkit ;
}

void utility::initGlobals()
{
	favorites::instance().updateFavorites() ;
	utility::setGUIThread() ;
#ifdef Q_OS_LINUX
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
#ifdef Q_OS_LINUX
	auto e = utility::helperSocketPath() ;

	if( utility::pathExists( e ) ){

		QLocalSocket s ;

		s.connectToServer( e ) ;

		if( s.waitForConnected() ){

			s.write( [ & ]()->QByteArray{

				sirikali::json json ;

				json[ "cookie" ]   = _cookie ;
				json[ "password" ] = "" ;
				json[ "command" ]  = "exit" ;

				return json.structure() ;
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

			if( !utility::platformIsWindows() ){

				if( failed && obj ){

					DialogMsg( obj ).ShowUIOK( title,msg ) ;
				}
			}
		} ) ;
	}
}

::Task::future< utility::fsInfo >& utility::fileSystemInfo( const QString& q )
{
	return ::Task::run( [ = ](){

		Q_UNUSED( q ) ;

		utility::fsInfo s ;

#ifndef Q_OS_WIN
		struct statfs e ;

		s.valid = statfs( q.toLatin1().constData(),&e ) == 0 ;

		s.f_bavail = e.f_bavail ;
		s.f_bfree  = e.f_bfree ;
		s.f_blocks = e.f_blocks ;
		s.f_bsize  = static_cast< decltype( s.f_bsize ) >( e.f_bsize ) ;
#endif
		return s ;
	} ) ;
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

QString utility::getKey( const QString& cipherPath,const secrets& secret )
{
	if( utility::runningOnBackGroundThread() ){

		return QString() ;
	}

	auto& settings = settings::instance() ;

	if( !settings.allowExternalToolsToReadPasswords() ){

		return QString() ;
	}

	auto bk = settings.autoMountBackEnd() ;

	if( bk.isInvalid() ){

		return QString() ;
	}

	auto m = utility::getKey( cipherPath,secret.walletBk( bk.bk() ).bk() ) ;

	return m.key ;
}

utility::wallet utility::getKey( const QString& keyID,LXQt::Wallet::Wallet& wallet,QWidget * widget )
{
	auto _getKey = []( LXQt::Wallet::Wallet& wallet,const QString& volumeID ){

		return ::Task::await( [ & ](){ return wallet.readValue( volumeID ) ; } ) ;
	} ;

	utility::wallet w{ false,false,"" } ;

	auto s = wallet.backEnd() ;
	auto& wlt = settings::instance() ;

	if( s == LXQt::Wallet::BackEnd::internal ){

		auto walletName = wlt.walletName() ;
		auto appName    = wlt.applicationName() ;

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
		w.opened = wallet.open( wlt.walletName( s ),wlt.applicationName() ) ;

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
	return engines::executableSearchPaths() ;
}

QString utility::executableSearchPaths( const QString& e )
{
	auto m = [](){

		if( utility::platformIsWindows() ){

			return ";" ;
		}else{
			return ":" ;
		}
	}() ;

	if( e.isEmpty() ){

		return utility::executableSearchPaths().join( m ) ;
	}else{
		return e + m + utility::executableSearchPaths().join( m ) ;
	}
}

QString utility::executableFullPath( const QString& e )
{
	return engines::executableFullPath( e ) ;
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
	return settings::instance().homePath() + "/.SiriKali/" + e ;
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

bool utility::pathIsFile( const QString& path )
{
	return QFileInfo( path ).isFile() ;
}

bool utility::pathExists( const QString& path )
{
	return QFile::exists( path ) ;
}

bool utility::pathNotExists( const QString& path )
{
	return !utility::pathExists( path ) ;
}

QStringList utility::split( const QString& e,char token )
{
	if( e.isEmpty() ){

		return QStringList( QString() ) ;
	}else{
		return e.split( token,QString::SkipEmptyParts ) ;
	}
}

QString utility::removeOption( const QStringList& e,const QString& s )
{
	QString n ;

	for( const auto& it : e ){

		if( it != s ){

			n += it + "," ;
		}
	}

	if( n.endsWith( "," ) ){

		n.remove( n.size() - 1,1 ) ;
	}

	return n ;
}

QString utility::removeOption( const QString& e,const QString& s )
{
	return utility::removeOption( utility::split( e,',' ),s ) ;
}

QString utility::mountPathPostFix( const QString& path )
{
	return utility::mountPathPostFix( settings::instance().mountPath( path ),path ) ;
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

void utility::setWindowOptions( QDialog * w )
{
	if( utility::platformIsOSX() ){

		w->setWindowFlags( w->windowFlags() | Qt::WindowStaysOnTopHint ) ;
		w->setWindowModality( Qt::WindowModal ) ;
	}
}

bool utility::folderIsEmpty( const QString& m )
{
	return QDir( m ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System ).count() == 0 ;
}

bool utility::folderNotEmpty( const QString& m )
{
	return !utility::folderIsEmpty( m ) ;
}

bool utility::createFolder( const QString& m )
{
	if( utility::isDriveLetter( m ) ){

		return true ;
	}else{
		return QDir().mkpath( m ) ;
	}
}

bool utility::removeFolder( const QString& e,int attempts )
{
	if( utility::isDriveLetter( e ) ){

		return true ;
	}

	QDir dir ;

	dir.rmdir( e ) ;

	if( utility::pathNotExists( e ) ){

		return true ;
	}else{
		for( int i = 1 ; i < attempts ; i++ ){

			utility::waitForOneSecond() ;

			dir.rmdir( e ) ;

			if( utility::pathNotExists( e ) ){

				return true ;
			}
		}

		return false ;
	}
}

#ifdef Q_OS_WIN

QString utility::readPassword( bool addNewLine )
{
	std::cout << "Password: " << std::flush ;

	QString s ;
	int e ;

	int m = settings::instance().readPasswordMaximumLength() ;

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

#else

#include <termios.h>

static inline bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old;
	current->c_lflag &= static_cast< unsigned int >( ~ECHO ) ;

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

	int m = settings::instance().readPasswordMaximumLength() ;

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

#endif

const QProcessEnvironment& utility::systemEnvironment()
{
	return utility::globalEnvironment().instance().get() ;
}

QString utility::configFilePath( QWidget * s,const QString& e )
{
	QFileDialog dialog( s ) ;

	dialog.setFileMode( QFileDialog::AnyFile ) ;

	dialog.setDirectory( settings::instance().homePath() ) ;

	dialog.setAcceptMode( QFileDialog::AcceptSave ) ;

	dialog.selectFile( [ = ](){

		return engines::instance().getByName( e ).configFileName() ;

	}() ) ;

	if( dialog.exec() ){

		auto q = dialog.selectedFiles() ;

		if( !q.isEmpty() ){

			return q.first() ;
		}
	}

	return QString() ;
}

QString utility::getExistingFile( QWidget * w,const QString& caption,const QString& dir )
{
	return QFileDialog::getOpenFileName( w,caption,dir ) ;
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

QString utility::freeWindowsDriveLetter()
{
	char m[ 3 ] = { 'Z',':','\0' } ;

	for( ; *m > 'D' ; *m -= 1 ){

		if( !utility::pathExists( m ) ){

			return m ;
		}
	}

	return "Z:" ;
}

bool utility::startsWithDriveLetter( const QString& path )
{
	return utility::isDriveLetter( path.mid( 0,3 ) ) ;
}

bool utility::isDriveLetter( const QString& path )
{
	auto _drivePrefix = [ & ]( const QString& path ){

		if( path.size() == 2 ){

			auto a = path.at( 0 ) ;
			auto b = path.at( 1 ) ;

			return a >= 'A' && a <= 'Z' && b == ':' ;
		}else{
			return false ;
		}
	} ;

	if( utility::platformIsWindows() ){

		if( path.size() == 2 ){

			return _drivePrefix( path ) ;

		}else if( path.size() == 3 ){

			return _drivePrefix( path ) && path.at( 2 ) == '\\' ;
		}
	}

	return false ;
}

template< typename E >
static void _setWindowsMountMountOptions( QWidget * obj,E e,QPushButton * s )
{
	auto menu = new QMenu( obj ) ;

	QList< QAction* > actions ;

	char m[ 3 ] = { 'G',':','\0' } ;

	for( ; *m <= 'Z' ; *m += 1 ){

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

void utility::setWindowsMountPointOptions( QWidget * obj,QTextEdit * e,QPushButton * s )
{
	_setWindowsMountMountOptions( obj,e,s ) ;
}

void utility::setWindowsMountPointOptions( QWidget * obj,QLineEdit * e,QPushButton * s )
{
	_setWindowsMountMountOptions( obj,e,s ) ;
}

static utility::result< int > _convert_string_to_version( const QString& e )
{
	auto _convert = []( const QString& e )->utility::result< int >{

		bool ok ;

		auto s = e.toInt( &ok ) ;

		if( ok ){

			return s  ;
		}else{
			return {} ;
		}
	} ;

	auto s = utility::split( e,'.' ) ;

	auto components = s.size() ;

	int major = 1000000 ;
	int minor = 1000 ;
	int patch = 1 ;

	if( components == 1 ){

		auto a = _convert( s.first() ) ;

		if( a ){

			return major * a.value() ;
		}

	}else if( components == 2 ){

		auto a = _convert( s.at( 0 ) ) ;
		auto b = _convert( s.at( 1 ) ) ;

		if( a && b ){

			return major * a.value() + minor * b.value() ;
		}

	}else if( components == 3 ){

		auto a = _convert( s.at( 0 ) ) ;
		auto b = _convert( s.at( 1 ) ) ;
		auto c = _convert( s.at( 2 ) ) ;

		if( a && b && c ){

			return major * a.value() + minor * b.value() + patch * c.value() ;
		}
	}

	return {} ;
}

::Task::future< utility::result< QString > >& utility::backEndInstalledVersion( const QString& backend )
{
	return ::Task::run( [ = ]()->utility::result< QString >{

		return engines::instance().getByName( backend ).installedVersionString() ;
	} ) ;
}

static utility::result< int > _installedVersion( const QString& backend )
{
	auto s = utility::backEndInstalledVersion( backend ).get() ;

	if( s && !s.value().isEmpty() ){

		return _convert_string_to_version( s.value() ) ;
	}else{
		return {} ;
	}
}

template< typename Function >
::Task::future< utility::result< bool > >& _compare_versions( const QString& backend,
							     const QString& version,
							     Function compare )
{
	return ::Task::run( [ = ]()->utility::result< bool >{

		auto installed = _installedVersion( backend ) ;
		auto guard_version = _convert_string_to_version( version ) ;

		if( installed && guard_version ){

			return compare( installed.value(),guard_version.value() ) ;
		}else{
			return {} ;
		}
	} ) ;
}

::Task::future< utility::result< bool > >& utility::backendIsGreaterOrEqualTo( const QString& backend,
									       const QString& version )
{
	return _compare_versions( backend,version,std::greater_equal<int>() ) ;
}

::Task::future< utility::result< bool > >& utility::backendIsLessThan( const QString& backend,
								       const QString& version )
{
	return _compare_versions( backend,version,std::less<int>() ) ;
}

QString utility::wrap_su( const QString& s )
{
	auto su = utility::executableFullPath( "su" ) ;

	if( su.isEmpty() ){

		return s ;
	}else{
		return QString( "%1 - -c \"%2\"" ).arg( su,QString( s ).replace( "\"","'" ) ) ;
	}
}

utility::globalEnvironment& utility::globalEnvironment::instance()
{
	static utility::globalEnvironment m ;
	return m ;
}

const QProcessEnvironment& utility::globalEnvironment::get() const
{
	return m_environment ;
}

void utility::globalEnvironment::remove( const QString& e )
{
	m_environment.remove( e ) ;
}

void utility::globalEnvironment::insert( const QString& key,const QString& value )
{
	m_environment.insert( key,value ) ;
}

utility::globalEnvironment::globalEnvironment() :
	m_environment( QProcessEnvironment::systemEnvironment() )
{
	auto m = utility::executableSearchPaths( m_environment.value( "PATH" ) ) ;

	m_environment.insert( "PATH",m ) ;

	m_environment.insert( "LANG","C" ) ;
}

void utility::setGUIThread()
{
	_main_gui_thread = QThread::currentThread() ;
}

bool utility::runningOnGUIThread()
{
	return QThread::currentThread() == _main_gui_thread ;
}

bool utility::runningOnBackGroundThread()
{
	return QThread::currentThread() != _main_gui_thread ;
}

void utility::runInUiThread( std::function< void() > function )
{
	runInThread::instance( _main_gui_thread,std::move( function ) ) ;
}

void utility::waitForOneSecond()
{
	utility::wait( 1 ) ;
}

void utility::wait( int time )
{
	if( utility::runningOnBackGroundThread() ){

		utility::Task::wait( time ) ;
	}else{
		utility::Task::suspend( time ) ;
	}
}

void utility::waitForFinished( QProcess& e )
{
	if( utility::runningOnGUIThread() ){

		while( e.state() == QProcess::Running ){

			utility::waitForOneSecond() ;
		}
	}

	e.waitForFinished() ;
}

static QString _ykchalresp_path()
{
	static QString m = utility::executableFullPath( "ykchalresp" ) ;
	return m ;
}

utility::result< QByteArray > utility::yubiKey( const QString& challenge )
{
	QString exe = _ykchalresp_path() ;

	if( !exe.isEmpty() ){

		exe = exe + " " + settings::instance().ykchalrespArguments() ;

		auto s = utility::unwrap( ::Task::process::run( exe,challenge.toLatin1() ) ) ;

		utility::logCommandOutPut( s,exe ) ;

		if( s.success() ){

			auto m = s.std_out() ;

			m.replace( "\n","" ) ;

			return m ;
		}else{
			utility::debug::cout() << "Failed to get a responce from ykchalresp" ;
			utility::debug::cout() << "StdOUt:" << s.std_out() ;
			utility::debug::cout() << "StdError:" << s.std_error() ;
		}
	}

	return {} ;
}
