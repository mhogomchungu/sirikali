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

bool utility::platformIsNOTWindows()
{
	return !utility::platformIsWindows() ;
}

void windowsDebugWindow( const QString& e,bool s )
{
	if( s ){

		utility::logger().enableDebug().showDebugWindow() ;
	}

	utility::debug() << e ;
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

::Task::future< utility::Task >& utility::Task::run( const QString& exe,
						     const QStringList& list,
						     const utility::Task::moreOptions& e )
{
	return utility::Task::run( exe,list,-1,e ) ;
}

::Task::future< utility::Task >& utility::Task::run( const QString& exe,
						     const QStringList& list,
						     int s,
						     const utility::Task::moreOptions& e )
{
	return ::Task::run( [ = ](){

		const auto& env = utility::systemEnvironment() ;

		return utility::Task( exe,list,s,env,QByteArray(),[](){},e ) ;
	} ) ;
}

void utility::Task::execute( const QString& exe,
			     const QStringList& args,
			     int waitTime,
			     const QProcessEnvironment& env,
			     const QByteArray& password,
			     std::function< void() > function,
			     const utility::Task::moreOptions& moreOpts )
{
	const auto& m = utility::miscOptions::instance() ;

	if( moreOpts.usePolkit() && m.usePolkit() ){

		auto _report_error = [ this ]( const char * msg ){

			m_finished   = true ;
			m_exitCode   = -1 ;
			m_exitStatus = -1 ;
			m_stdError   =  msg ;
			//m_stdOut     =  "" ;
		} ;

		QLocalSocket s ;

		s.connectToServer( m.getSocketPath() ) ;

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

		utility::logger logger ;

		logger.showText( exe,args ) ;

		utility::unlockIntervalReporter rpt( moreOpts.likeSsh() ) ;

		s.write( [ & ]()->QByteArray{

			SirikaliJson json( logger.function() ) ;

			json[ "cookie" ]   = m.getCookie() ;
			json[ "password" ] = password ;
			json[ "command" ]  = exe ;
			json[ "args" ]     = args ;

			return json.structure() ;
		}() ) ;

		s.waitForBytesWritten() ;

		s.waitForReadyRead() ;

		rpt.report() ;

		SirikaliJson json( s.readAll(),logger.function() ) ;

		m_finished   = json.getBool( "finished" ) ;
		m_exitCode   = json.getInterger( "exitCode" ) ;
		m_exitStatus = json.getInterger( "exitStatus" ) ;
		m_stdError   = json.getByteArray( "stdError" ) ;
		m_stdOut     = json.getByteArray( "stdOut" ) ;

		logger.showText( { m_stdOut,m_stdError,m_exitCode,m_exitStatus,m_finished } ) ;
	}else{
		using LG = utility2::LOGLEVEL ;

		auto log = [ & ](){

			if( moreOpts.allowLogging().has_value() ){

				return moreOpts.allowLogging().value() ;
			}else{
				return LG::COMMAND_ONLY ;
			}
		}() ;

		utility::logger logger ;

		if( log == LG::COMMAND_ONLY || log == LG::COMMAND_AND_UNLOCK_DURATION ){

			logger.showText( exe,args ) ;
		}

		utility::unlockIntervalReporter rpt( moreOpts.likeSsh() ) ;

		auto& ss = ::Task::process::run( exe,args,waitTime,password,env,std::move( function ) ) ;

		auto s = utility::unwrap( ss ) ;

		m_finished   = s.finished() ;
		m_exitCode   = s.exit_code() ;
		m_exitStatus = s.exit_status() ;
		m_stdOut     = s.std_out() ;
		m_stdError   = s.std_error() ;

		if( log == LG::COMMAND_ONLY ){

			logger.showText( s ) ;

		}else if( log == LG::COMMAND_ONLY || log == LG::COMMAND_AND_UNLOCK_DURATION ){

			rpt.report() ;
			logger.showText( s ) ;
		}
	}
}

utility::debug utility::debug::operator<<( const QString& e )
{
	utility::logger().showTextWithLines( e ) ;

	return utility::debug() ;
}

void utility::applicationStarted()
{
	utility::miscOptions::instance().doneStarting() ;

	const auto& m = utility::miscOptions::instance() ;

	const auto& e = m.getStartingLogs() ;

	if( !e.isEmpty() ){

		QString a = "***********Start early logs************" ;
		QString b = "\n\n***********End early logs*****************" ;

		utility::logger().enableDebug().showText( a + e + b ).showDebugWindow() ;
	}
}

void utility::debug::showDebugWindow( const QString& e )
{
	utility::logger().enableDebug().showTextWithLines( e ).showDebugWindow() ;
}

void utility::debug::logErrorWhileStarting( const QString& e )
{
	utility::logger().appendLog( "\n\n" + e ) ;
}

utility::debug utility::debug::operator<<( int e )
{
	utility::logger().showTextWithLines( QString::number( e ) ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const char * e )
{
	utility::logger().showTextWithLines( e ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const QByteArray& e )
{
	utility::logger().showTextWithLines( e ) ;

	return utility::debug() ;
}

utility::debug utility::debug::operator<<( const QStringList& e )
{
	utility::logger().showTextWithLines( e.join( "\n" ) ) ;
	return utility::debug() ;
}

static engines::engine::exe_args siriPolkitExe()
{
	auto exe = engines::executableFullPath( "pkexec" ) ;

	if( exe.isEmpty() ){

		return {} ;
	}else{
		return { exe,{ siriPolkitPath,utility::miscOptions::instance().getSocketPath(),"fork" } } ;
	}
}

static ::Task::future< utility::Task >& _start_siripolkit( const engines::engine::exe_args_const& e )
{
	auto m = crypto::getRandomData( 16 ).toHex() ;

	utility::miscOptions::instance().setCookie( m ) ;

	return ::Task::run( [ = ]{

		return utility::Task( e.exe,
				      e.args,
				      -1,
				      utility::systemEnvironment(),
				      m,
				      [](){},
				      false ) ;
	} ) ;
}

bool utility::enablePolkit()
{
	const auto& m = utility::miscOptions::instance() ;

	if( m.usePolkit() ){

		return true ;
	}

	auto exe = siriPolkitExe() ;

	if( !exe.exe.isEmpty() ){

		const auto& socketPath = m.getSocketPath() ;

		if( utility::unwrap( _start_siripolkit( exe ) ).success() ){

			utility::miscOptions::instance().setUsePolkit( true ) ;

			while( utility::pathNotExists( socketPath ) ){

				utility::waitForOneSecond() ;
			}
		}
	}

	return m.usePolkit() ;
}

void utility::initGlobals()
{
	settings::instance().scaleGUI() ;

	auto& m = utility::miscOptions::instance() ;

	m.setCurrentThreadAsMain() ;

	m.setEnableDebug( settings::instance().showDebugWindowOnStartup() ) ;

	if( utility::platformIsLinux() ){

		auto uid = getuid() ;

		QString a = "/tmp/SiriKali-" + QString::number( uid ) ;

		auto b = a + "/siriPolkit.socket" ;

		utility::miscOptions::instance().setPolkitPath( b ) ;

		QDir e ;

		e.remove( a ) ;
		e.rmdir( b ) ;

		e.mkpath( a ) ;

		auto s = a.toLatin1() ;

		if( chown( s.constData(),uid,uid ) ){}
		if( chmod( s.constData(),0700 ) ){}
	}
}

void utility::quitHelper()
{
	if( utility::platformIsLinux() ){

		const auto& w = utility::miscOptions::instance() ;

		auto e = w.getSocketPath() ;

		if( utility::pathExists( e ) ){

			QLocalSocket s ;

			s.connectToServer( e ) ;

			if( s.waitForConnected() ){

				s.write( [ & ]()->QByteArray{

					SirikaliJson json( []( const QString& e ){ utility::debug() << e ; } ) ;

					json[ "cookie" ]   = w.getCookie() ;
					json[ "password" ] = "" ;
					json[ "command" ]  = "exit" ;
					json[ "args" ]     = QStringList() ;

					return json.structure() ;
				}() ) ;

				s.waitForBytesWritten() ;
			}
		}

		auto a = "/tmp/SiriKali-" + QString::number( getuid() ) ;

		QDir m ;
		m.remove( a ) ;
		m.rmdir( w.getSocketPath() ) ;
	}
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener )
{
	return ::Task::run( [ = ](){

		return utility::Task::run( opener,{ path } ).get().failed() ;
	} ) ;
}

void utility::openPath( const QString& path,const QString& opener,
			QWidget * obj,const QString& title,const QString& msg )
{
	if( !path.isEmpty() ){

		openPath( path,opener ).then( [ title,msg,obj ]( bool failed ){

			if( utility::platformIsNOTWindows() ){

				if( failed && obj ){

					DialogMsg( obj ).ShowUIOK( title,msg ) ;
				}
			}
		} ) ;
	}
}

static bool _help()
{
	utility::debug::cout() << VERSION_STRING << QObject::tr( "\n\
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

QString utility::cmdArgumentValue( const QStringList& l,const QString& arg,const QString& defaulT )
{
	int j = l.size() ;

	auto _absolute_exe_path = []( const QString& exe ){

		auto e = engines::executableFullPath( exe ) ;

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
	Q_UNUSED( expand )
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

QIcon utility::getIcon( iconType type )
{
	if( utility::platformIsLinux() ){

		QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/sirikali.png" ) ;

		if( type == utility::iconType::trayIcon ){

			return QIcon::fromTheme( "sirikali-panel",icon ) ;
		}else{
			return QIcon::fromTheme( "sirikali",icon ) ;
		}
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

template< typename T >
static QStringList _split( const QString& e,const T& token )
{
	if( e.isEmpty() ){

		return QStringList( QString() ) ;
	}else{
		#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
			return e.split( token,QString::SkipEmptyParts ) ;
		#else
			return e.split( token,Qt::SkipEmptyParts ) ;
		#endif
	}
}

QStringList utility::split( const QString& e,const QString& token )
{
	return _split( e,token ) ;
}

QStringList utility::split( const QString& e,char token )
{
	return _split( e,token ) ;
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

		n = utility::removeLast( n,1 ) ;
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

static bool _terminalEchoOff( struct termios * old,struct termios * current )
{
	if( tcgetattr( 1,old ) != 0 ){

		return false ;
	}

	*current = *old ;
	current->c_lflag &= static_cast< unsigned int >( ~ECHO ) ;

	if( tcsetattr( 1,TCSAFLUSH,current ) != 0 ){

		return false ;
	}else{
		return true ;
	}
}

QByteArray utility::readPassword( bool addNewLine )
{
	if( utility::platformIsWindows() ){

		std::cout << "Password: " << std::flush ;

		QByteArray s ;
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
	}else{
		std::cout << "Password: " << std::flush ;

		struct termios old ;
		struct termios current ;

		_terminalEchoOff( &old,&current ) ;

		QByteArray s ;
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
}

const QProcessEnvironment& utility::systemEnvironment()
{
	static class sysEnv{
	public:
		sysEnv() : m_QProcessEnvironment( QProcessEnvironment::systemEnvironment() )
		{
			m_QProcessEnvironment.insert( "LANG","C" ) ;
		}
		const QProcessEnvironment& get() const
		{
			return m_QProcessEnvironment ;
		}
	private:
		QProcessEnvironment m_QProcessEnvironment ;
	} environment ;

	return environment.get() ;
}

QString utility::configFilePath( QWidget * s,const QString& e )
{
	QFileDialog dialog( s ) ;

	dialog.setFileMode( QFileDialog::AnyFile ) ;

	dialog.setDirectory( settings::instance().homePath() ) ;

	dialog.setAcceptMode( QFileDialog::AcceptSave ) ;

	dialog.selectFile( e ) ;

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

		if( path.size() == 1 ) {

			auto a = path.at( 0 ) ;

			return a >= 'A' && a <= 'Z' ;

		}else if( path.size() == 2 ){

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

void utility::runInUiThread( std::function< void() > function )
{
	runInThread::instance( utility::miscOptions::instance().getMainGUIThread(),std::move( function ) ) ;
}

void utility::waitForOneSecond()
{
	utility::wait( 1 ) ;
}

void utility::wait( int time )
{
	if( utility::miscOptions::instance().runningOnBackGroundThread() ){

		utility::Task::wait( time ) ;
	}else{
		utility::Task::suspend( time ) ;
	}
}

template <typename Function >
static bool _wait_for_finished( QProcess& e,int timeOut,Function wait )
{
	for( int i = 0 ; i < timeOut ; i++ ){

		e.waitForFinished( 500 ) ;

		if( e.state() == QProcess::Running ){

			utility::debug() << "Waiting For A Process To Finish" ;
			wait() ;
		}else{
			utility::debug() << "Process Finished" ;
			return true ;
		}
	}

	utility::debug() << "Warning, Process Is Still Running Past Timeout" ;

	return false ;
}

bool utility::waitForFinished( QProcess& e,int timeOut )
{
	if( utility::miscOptions::instance().runningOnGUIThread() ){

		return _wait_for_finished( e,timeOut,[](){ utility::Task::suspendForOneSecond() ; } ) ;
	}else{
		return _wait_for_finished( e,timeOut,[](){ utility::Task::waitForOneSecond() ; } ) ;
	}
}

static QString _ykchalresp_path()
{
	static QString m = engines::executableFullPath( "ykchalresp" ) ;
	return m ;
}

static bool _yubikey_remove_newline()
{
	static bool m = settings::instance().yubikeyRemoveNewLine() ;
	return m ;
}

utility::qbytearray_result utility::yubiKey( const QByteArray& challenge )
{
	QString exe = _ykchalresp_path() ;

	if( !exe.isEmpty() ){

		auto args = utility::split( settings::instance().ykchalrespArguments(),' ' );

		utility::logger logger ;

		logger.showText( exe,args ) ;

		auto s = utility::unwrap( ::Task::process::run( exe,args,-1,challenge ) ) ;

		logger.showText( s ) ;

		if( s.success() ){

			auto m = s.std_out() ;

			if( _yubikey_remove_newline() ){

				m.replace( "\n","" ) ;
			}

			return m ;
		}else{
			utility::debug() << "Failed to get a response from ykchalresp" ;
			utility::debug() << "StdOUt:" << s.std_out() ;
			utility::debug() << "StdError:" << s.std_error() ;
		}
	}

	return {} ;
}

QString utility::policyString()
{
	return QObject::tr( "Policy:" ) ;
}

QString utility::commentString()
{
	return QObject::tr( "Comment:" ) ;
}

QByteArray utility::convertPassword( const QString& e )
{
	if( settings::instance().passWordIsUTF8Encoded() ){

		return e.toUtf8() ;
	}else{
		return e.toLatin1() ;
	}
}

QString utility::convertPassword( const QByteArray& e )
{
	if( settings::instance().passWordIsUTF8Encoded() ){

		return QString::fromUtf8( e ) ;
	}else{
		return QString::fromLatin1( e ) ;
	}
}

static void _remove_last( QString& e,int n )
{
	e.remove( e.size() - n,n ) ;
}

QString utility::removeLast( const QString& s,int n )
{
	auto m = s ;

	_remove_last( m,n ) ;

	return m ;
}

QString utility::removeFirstAndLast( const QString& s,int first,int last )
{
	auto m = s.mid( first ) ;

	_remove_last( m,last ) ;

	return m ;
}

QString utility::removeLastPathComponent( const QString& e,char separator )
{
	auto s = utility::split( e,separator ) ;
	s.removeLast() ;

	if( utility::platformIsWindows() && utility::startsWithDriveLetter( e ) ){

		return s.join( separator ) ;
	}

	return separator + s.join( separator ) ;
}


QString utility::likeSshaddPortNumber( const QString& path,const QString& port )
{
	return path + settings::instance().portSeparator() + port ;
}

QString utility::likeSshRemovePortNumber( const QString& path )
{
	return utility::split( path,settings::instance().portSeparator() ).at( 0 ) ;
}

QString utility::logger::starLine()
{
	return "*************************************" ;
}

utility::logger::logger() : m_miscOptions( utility::miscOptions::instance() )
{
}

std::function< void( const QString& ) > utility::logger::function()
{
	return [ this ]( const QString& e ){

		if( m_miscOptions.starting() ){

			if( e.startsWith( "Error" ) ){

				utility::debug::logErrorWhileStarting( e ) ;
			}else{
				utility::debug() << e ;
			}
		}else{
			utility::debug() << e ;
		}
	} ;
}

utility::logger& utility::logger::showText( const QString& cmd,const QStringList& args )
{
	QString exe = "Command: \"" + cmd + "\"" ;

	for( const auto& it : args ){

		exe += " \"" + it + "\"" ;
	}

	this->showText( exe ) ;

	return *this ;
}

utility::logger& utility::logger::showText( const QString& e )
{
	auto s = m_miscOptions.getDebugWindow() ;

	if( s.has_value() ){

		s.value()->UpdateOutPut( e,m_miscOptions.debugEnabled() ) ;
	}

	return *this ;
}

utility::logger& utility::logger::showLine()
{
	this->showText( logger::starLine() ) ;
	return *this ;
}

utility::logger& utility::logger::appendLog( const QString& e )
{
	m_miscOptions.appendLog( e ) ;
	return *this ;
}

utility::logger& utility::logger::showText( const ::Task::process::result& m )
{
	auto _trim = []( QString e,bool s )->QString{

		while( true ){

			if( e.endsWith( '\n' ) ){

				e.truncate( e.size() - 1 ) ;
			}else{
				break ;
			}
		}

		if( e.isEmpty() ){

			if( s ){

				return "\n" ;
			}else{
				return "";
			}
		}else{
			if( s ){

				return "\n" + e + "\n" ;
			}else{
				return "\n" + e ;
			}
		}
	} ;

	QString s = "-------\nExit Code: %1\nExit Status: %2\n-------\nStdOut:%3-------\nStdError:%4" ;

	auto e = s.arg( QString::number( m.exit_code() ),
			QString::number( m.exit_status() ),
			_trim( m.std_out(),true ),
			_trim( m.std_error(),false ) ) ;

	this->showText( e ) ;

	this->showLine() ;

	return *this ;
}

utility::logger& utility::logger::showTextWithLines( const QString& b )
{
	auto m = logger::starLine() ;

	if( m_miscOptions.debugEnabled() ){

		utility::debug::cout() << b ;
	}

	this->showText( m + "\n" + b + "\n" + m ) ;

	return *this ;
}

utility::logger& utility::logger::showDebugWindow()
{
	auto s = m_miscOptions.getDebugWindow() ;

	if( s.has_value() ){

		s.value()->Show() ;
	}

	return *this ;
}

utility::logger& utility::logger::enableDebug()
{
	m_miscOptions.setEnableDebug( true ) ;

	return *this ;
}

QString utility::SiriKaliVersion()
{
	return THIS_VERSION ;
}

#ifdef Q_OS_WIN

QString utility::userName()
{
	return {} ;
}

int utility::userID()
{
	return -1 ;
}

#else

#include <pwd.h>

QString utility::userName()
{
	auto m = getpwuid( getuid() ) ;

	if( m ){

		return m->pw_name ;
	}else{
		return {} ;
	}
}

int utility::userID()
{
	auto m = getpwuid( getuid() ) ;

	if( m ){

		return static_cast< int >( m->pw_uid ) ;
	}else{
		return {} ;
	}
}

#endif

QString utility::userIDAsString()
{
	return QString::number( utility::userID() ) ;
}

void utility::unlockIntervalReporter::report() const
{
	auto m = QDateTime::currentSecsSinceEpoch() - m_origTime ;

	auto s = [ & ](){

		if( m_likeSsh.has_value() && m_likeSsh.value() ){

			return "The attempt to connect took " ;
		}else{
			return "The attempt to unlock the volume took " ;
		}
	}() ;

	utility::debug() << s + QString::number( m ) + " seconds" ;
}
