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
#include <pwd.h>
#include <grp.h>
#include <termios.h>

#include <memory>
#include <iostream>

#include <QObject>
#include <QDir>
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
#include <pwd.h>
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

#else

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

#endif

static QSettings * _settings ;

static QByteArray _cookie ;

::Task::future< utility::Task >& utility::Task::run( const QString& exe,bool e )
{
	return utility::Task::run( exe,-1,e ) ;
}

::Task::future< utility::Task >& utility::Task::run( const QString& exe,int s,bool e )
{
	return ::Task::run< utility::Task >( [ = ](){

		auto env = utility::systemEnvironment() ;

		return utility::Task( exe,s,env,QByteArray(),[](){},e ) ;
	} ) ;
}

void utility::Task::execute( const QString& exe,int waitTime,
			     const QProcessEnvironment& env,
			     const QByteArray& password,
			     const std::function< void() >& f,
			     bool polkit )
{
	class Process : public QProcess{
	public:
		Process( const std::function< void() >& f ) : m_function( f )
		{
		}
	protected:
		void setupChildProcess()
		{
			m_function() ;
		}
	private:
		std::function< void() > m_function ;
	} p( f ) ;

	p.setProcessEnvironment( env ) ;

	if( polkit && utility::useZuluPolkit() ){

		auto _report_error = [ this ](){

			m_finished   = true ;
			m_exitCode   = -1 ;
			m_exitStatus = -1 ;
			m_stdError   =  "SiriKali: Failed To Connect To Polkit Backend" ;
			//m_stdOut     =  "" ;
		} ;

		QLocalSocket s ;

		s.connectToServer( utility::helperSocketPath() ) ;

		for( int i = 0 ; ; i++ ){

			if( s.waitForConnected() ){

				break ;

			}else if( i == 3 ){

				_report_error() ;

				utility::debug() << "ERROR: Failed To Start Helper Application" ;
				return ;
			}else{
				utility::debug() << s.errorString() ;

				utility::Task::suspendForOneSecond() ;
			}
		}

		s.write( [ & ]()->QByteArray{

			nlohmann::json json ;

			json[ "cookie" ]     = _cookie.constData() ;
			json[ "password" ]   = password.constData() ;
			json[ "command" ]    = exe.toLatin1().constData() ;

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

			_report_error() ;
		}
	}else{
		p.start( exe ) ;

		if( !password.isEmpty() ){

			p.waitForStarted() ;

			p.write( password + '\n' ) ;

			p.closeWriteChannel() ;
		}

		m_finished   = p.waitForFinished( waitTime ) ;
		m_exitCode   = p.exitCode() ;
		m_exitStatus = p.exitStatus() ;
		m_stdOut     = p.readAllStandardOutput() ;
		m_stdError   = p.readAllStandardError() ;
	}
}

void utility::startHelper( QWidget * obj,const QString& arg,const char * slot )
{
	if( !utility::useZuluPolkit() ){

		QMetaObject::invokeMethod( obj,slot,Q_ARG( bool,true ),Q_ARG( QString,arg ) ) ;

		return ;
	}

	QFile f( "/dev/urandom" ) ;

	f.open( QIODevice::ReadOnly ) ;

	_cookie = f.read( 16 ).toHex() ;

	auto exe = utility::executableFullPath( "pkexec" ) ;

	if( !exe.isEmpty() ){

		exe = QString( "%1 %2 %3 fork" ).arg( exe,siriPolkitPath,utility::helperSocketPath() ) ;

		utility::Task::run( exe ).then( [ = ]( const utility::Task& e ){

			if( e.failed() ){

				utility::debug() << "Failed to start polkit backend" ;
			}

			QMetaObject::invokeMethod( obj,
						   slot,
						   Q_ARG( bool,e.success() ),
						   Q_ARG( QString,arg ) ) ;
		} ) ;
	}else{
		utility::debug() << "Failed to locate pkexec executable" ;

		QMetaObject::invokeMethod( obj,
					   slot,
					   Q_ARG( bool,false ),
					   Q_ARG( QString,QString() ) ) ;
	}
}

QString utility::helperSocketPath()
{
	return utility::homeConfigPath() + ".tmp/SiriKali.polkit.socket" ;
}

bool utility::useZuluPolkit()
{
	if( _settings->contains( "EnablePolkitSupport" ) ){

		return _settings->value( "EnablePolkitSupport" ).toBool() ;
	}else{
		bool e = false ;
		_settings->setValue( "EnablePolkitSupport",e ) ;
		return e ;
	}
}

bool utility::enablePolkitSupport()
{
	return utility::useZuluPolkit() ;
}

void utility::enablePolkitSupport( bool e )
{
	_settings->setValue( "EnablePolkitSupport",e ) ;
}

void utility::quitHelper()
{
	auto e = utility::helperSocketPath() ;

	if( utility::pathExists( e ) ){

		QLocalSocket s ;

		s.connectToServer( e ) ;

		if( s.waitForConnected() ){

			s.write( [ & ]()->QByteArray{

				nlohmann::json json ;

				json[ "cookie" ]     = _cookie.constData() ;
				json[ "password" ]   = "" ;
				json[ "command" ]    = "exit" ;

				return json.dump().c_str() ;
			}() ) ;

			s.waitForBytesWritten() ;
		}
	}
}

QString utility::homePath()
{
	return QDir::homePath() ;
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener )
{
	return ::Task::run<bool>( [ = ](){

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

				DialogMsg m( obj ) ;
				m.ShowUIOK( title,msg ) ;
			}
		} ) ;
	}
}

bool utility::runningInMixedMode()
{
	return utility::useZuluPolkit() ;
}

bool utility::notRunningInMixedMode()
{
	return !utility::runningInMixedMode() ;
}

Task::future< utility::fsInfo >& utility::fileSystemInfo( const QString& q )
{
	return ::Task::run< utility::fsInfo >( [ = ](){

		struct statfs e ;
		utility::fsInfo s ;

		s.valid = statfs( q.toLatin1().constData(),&e ) == 0 ;

		s.f_bavail = e.f_bavail ;
		s.f_bfree  = e.f_bfree ;
		s.f_blocks = e.f_blocks ;
		s.f_bsize  = e.f_bsize ;

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

utility::wallet utility::getKey( const QString& keyID,LXQt::Wallet::Wallet& wallet )
{
	auto _getKey = []( LXQt::Wallet::Wallet& wallet,const QString& volumeID ){

		return ::Task::await<QString>( [ & ](){

			return wallet.readValue( volumeID ) ;
		} ) ;
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

				w.opened = wallet.open( walletName,appName ) ;
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
	QString e = f ;

	if( e == "ecryptfs" ){

		e = "ecryptfs-simple" ;
	}

	QString exe ;

	for( const auto& it : utility::executableSearchPaths() ){

		exe = it + e ;

		if( utility::pathExists( exe ) ){

			return exe ;
		}
	}

	return QString() ;
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

			q.append( e ) ;

			QStringList l ;

			for( const auto& it : q ){

				l.append( it.configString() ) ;
			}

			return l ;
		}() ) ;
	}
}

QVector< favorites::entry > utility::readFavorites()
{
	if( _settings->contains( "FavoritesVolumes" ) ){

		QVector< favorites::entry > e ;

		for( const auto& it : _settings->value( "FavoritesVolumes" ).toStringList() ){

			e.append( it ) ;
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

void utility::readFavorites( QMenu * m,bool truncate ,const QString& a, const QString& b )
{
	m->clear() ;

	auto _add_action = [ m,truncate ]( const favorites::entry& e ){

		auto ac = new QAction( m ) ;

		if( truncate ){

			ac->setText( e.volumePath ) ;
		}else{
			ac->setText( e.string() ) ;
		}

		return ac ;
	} ;

	m->addAction( new QAction( a,m ) ) ;
	m->addAction( new QAction( b,m ) ) ;

	m->addSeparator() ;

	for( const auto& it : utility::readFavorites() ){

		m->addAction( _add_action( it ) ) ;
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

static utility::array_t _default_dimensions( const char * defaults )
{
	auto l = QString( defaults ).split( ' ' ) ;

	utility::array_t e ;

	auto f = e.data() ;

	auto j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		*( f + i ) = l.at( i ).toInt() ;
	}

	return e ;
}

static utility::array_t _dimensions( const char * defaults,int size )
{
	if( _settings->contains( "Dimensions" ) ){

		auto l = utility::split( _settings->value( "Dimensions" ).toString(),' ' ) ;

		utility::array_t p ;

		if( l.size() != size || size > int( p.size() ) ){

			utility::debug() << "failed to parse config option" ;
			return _default_dimensions( defaults ) ;
		}

		auto f = p.data() ;

		auto j = l.size() ;

		for( int i = 0 ; i < j ; i++ ){

			bool ok ;

			int e = l.at( i ).toInt( &ok ) ;

			if( ok ){

				*( f + i ) = e ;
			}else{
				utility::debug() << "failed to parse config option" ;
				return _default_dimensions( defaults ) ;
			}
		}

		return p ;
	}else{
		_settings->setValue( "Dimensions",QString( defaults ) ) ;
		return _default_dimensions( defaults ) ;
	}
}

utility::array_t utility::getWindowDimensions()
{
	return _dimensions( "205 149 861 466 326 320 101 76",8 ) ;
}

void utility::setWindowDimensions( const std::initializer_list<int>& e )
{
	_settings->setValue( "Dimensions",[ & ](){

		QString q ;

		for( const auto& it : e ){

			q += QString::number( it ) + " " ;
		}

		return q ;
	}() ) ;
}

int utility::pluginKey( QWidget * w,QDialog * d,QByteArray * key,plugins::plugin plugin )
{
	QString s ;

	if( plugin == plugins::plugin::hmac_key ){

		s = QObject::tr( "hmac plugin.\n\nThis plugin generates a key using below formular:\n\nkey = hmac(sha256,passphrase,keyfile contents)" ) ;

	}else if( plugin == plugins::plugin::externalExecutable ){

		s = QObject::tr( "This plugin delegates key generation to an external application" ) ;
	}else{
		return 1 ;
	}

	QEventLoop l ;

	plugin::instance( w,d,plugin,[ & ]( const QByteArray& e ){

		*key = e ;

		if( e.isEmpty() ){

			l.exit( 1 ) ;
		}else{
			l.exit( 0 ) ;
		}

	},s ) ;

	return l.exec() ;
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
	QIcon icon( INSTALL_PREFIX "/share/icons/hicolor/48x48/apps/sirikali.png" ) ;

	return QIcon::fromTheme( "sirikali",icon ) ;
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

		return "default" ;
	}else{
		return utility::walletName() ;
	}
}

QString utility::applicationName()
{
	return "SiriKali" ;
}

QString utility::mountPath( const QString& path )
{
	QString e ;

	if( _settings->contains( "MountPrefix" ) ){

		e = _settings->value( "MountPrefix" ).toString() ;
	}else{
		auto e = utility::homePath() + "/.SiriKali" ;

		utility::setDefaultMountPointPrefix( e ) ;
	}

	return e + "/" + path ;
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

static bool _use_default_widget_relationship()
{
	if( _settings->contains( "UseDefaultWidgetRelationship" ) ){

		return _settings->value( "UseDefaultWidgetRelationship" ).toBool() ;
	}else{
		bool e = true ;
		_settings->setValue( "UseDefaultWidgetRelationship",e ) ;
		return e ;
	}
}

void utility::setParent( QWidget * parent,QWidget ** localParent,QDialog * dialog )
{
	if( utility::platformIsLinux() ){

		if( _use_default_widget_relationship() ){

			*localParent = parent ;
		}else{
			*localParent = dialog ;
		}
	}else{
		*localParent = dialog ;
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
