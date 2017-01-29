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
#include <QObject>
#include <QDir>
#include <pwd.h>
#include <grp.h>

#include <memory>

#include <QTranslator>
#include <QEventLoop>
#include <QDebug>
#include <QCoreApplication>
#include <QByteArray>
#include <QProcess>
#include <QFile>
#include <QFile>
#include <QDir>
#include <QTableWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QProcessEnvironment>
#include <unistd.h>
#include <pwd.h>
#include <QTimer>
#include <QEventLoop>

#include <QEvent>
#include <QKeyEvent>

#include "install_prefix.h"
#include "locale_path.h"

#include "plugins.h"

#include "readonlywarning.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <gcrypt.h>

#include "lxqt_wallet.h"

#include "dialogmsg.h"
#include "plugin.h"

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

static int _help()
{
	utility::debug() << VERSION_STRING << QObject::tr( "\n\
options:\n\
	-d   Path to where a volume to be auto unlocked/mounted is located.\n\
	-m   Tool to use to open a default file manager(default tool is xdg-open).\n\
	-e   Start the application without showing the GUI.\n\
	-b   A name of a backend to retrieve a password from when a volume is open from CLI.\n\
	     Supported backends are: \"internal\",\"kwallet\" and \"gnomewallet.\n\
	     The first one is always present but the rest are compile time dependencies.\n\
	-k   When opening a volume from CLI,a value of \"rw\" will open the volume in read\\write\n\
	     mode and a value of \"ro\" will open the volume in read only mode.\n\
	-z   Full path of the mount point to be used when the volume is opened from CLI.\n\
	     This option is optional.\n\
	-c   Set Volume Configuration File Path when a volume is opened from CLI.\n\
	-i   Set inactivity timeout(in minutes) to dismount the volume when mounted from CLI.\n\n" ) ;

	return 0 ;
}

static bool _printHelpOrVersionInfo()
{
	QStringList q = QCoreApplication::arguments() ;
	return q.contains( "-h" )        ||
	       q.contains( "-help" )     ||
	       q.contains( "--help" )    ||
	       q.contains( "-v" )        ||
	       q.contains(  "-version" ) ||
	       q.contains( "--version" ) ;
}

int utility::startApplication( const char * appName,std::function<int()> start )
{
	QCoreApplication::setApplicationName( appName ) ;

	if( _printHelpOrVersionInfo() ){

		return _help() ;
	}else{
		return start() ;
	}
}

static bool _execute_process( const QString& m,const QString& exe,const QString& env )
{
	Q_UNUSED( env ) ;

	if( exe.startsWith( "/" ) && utility::pathExists( exe ) ){

		auto e = m ;

		e.replace( "\"","\"\"\"" ) ;

		return utility::Task( exe + " \"" + e + "\"" ).success() ;
	}else{
		return false ;
	}
}

::Task::future<bool>& utility::openPath( const QString& path,const QString& opener,const QString& env )
{
	return ::Task::run<bool>( [ env,path,opener ](){

		return _execute_process( path,opener,env ) == false ;
	} ) ;
}

void utility::openPath( const QString& path,const QString& opener,const QString& env,QWidget * obj,const QString& title,const QString& msg )
{
	openPath( path,opener,env ).then( [ title,msg,obj ]( bool failed ){

		if( failed && obj ){

			DialogMsg( obj,nullptr ).ShowUIOK( title,msg ) ;
		}
	} ) ;
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

QString utility::executableFullPath( const QString& f )
{
	QString e = f ;

	if( e == "ecryptfs" ){

		e = "ecryptfs-simple" ;
	}

	QString exe ;

	auto l = { "/usr/local/bin/",
		   "/usr/local/sbin/",
		   "/usr/bin/",
		   "/usr/sbin/",
		   "/bin/",
		   "/sbin/",
		   "/opt/local/bin/",
		   "/opt/local/sbin/",
		   "/opt/bin/",
		   "/opt/sbin/" } ;

	for( const auto& it : l ){

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

			qDebug() << "failed to parse config option" ;
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
				qDebug() << "failed to parse config option" ;
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

QFont utility::getFont( QWidget * widget )
{
	if( _settings->contains( "Fonts" ) ){

		auto l = utility::split( _settings->value( "Fonts" ).toString() ) ;

		if( l.size() >= 4 ){

			QFont F ;

			const QString& fontFamily = l.at( 0 ) ;
			const QString& fontSize   = l.at( 1 ) ;
			const QString& fontStyle  = l.at( 2 ) ;
			const QString& fontWeight = l.at( 3 ) ;

			F.setFamily( fontFamily ) ;

			F.setPointSize( fontSize.toInt() ) ;

			if( fontStyle == "normal" ){

				F.setStyle( QFont::StyleNormal ) ;

			}else if( fontStyle == "italic" ){

				F.setStyle( QFont::StyleItalic ) ;
			}else{
				F.setStyle( QFont::StyleOblique ) ;
			}

			if( fontWeight == "normal" ){

				F.setWeight( QFont::Normal ) ;
			}else{
				F.setWeight( QFont::Bold ) ;
			}

			return F ;
		}else{
			return widget->font() ;
		}
	}else{
		return widget->font() ;
	}
}

void utility::saveFont( const QFont& Font )
{
	auto s = QString( "%1\n%2\n" ).arg( Font.family(),QString::number( Font.pointSize() ) ) ;

	if( Font.style() == QFont::StyleNormal ){

		s = s + "normal\n" ;

	}else if( Font.style() == QFont::StyleItalic ){

		s = s + "italic\n" ;
	}else{
		s = s + "oblique\n" ;
	}

	if( Font.weight() == QFont::Normal ){

		s = s + "normal\n" ;
	}else{
		s = s + "bold" ;
	}

	_settings->setValue( "Fonts",s ) ;
}

int utility::pluginKey( QWidget * w,QString * key,const QString& p )
{
	plugins::plugin pluginType ;
	QString pluginString ;
	QVector<QString> exe ;

	if( p == "hmac" ){

		pluginType   = plugins::plugin::hmac_key ;
		pluginString = QObject::tr( "hmac plugin.\n\nThis plugin generates a key using below formular:\n\nkey = hmac(sha256,passphrase,keyfile contents)" ) ;

	}else{
		return 1 ;
	}

	QEventLoop l ;

	plugin::instance( w,pluginType,[ & ]( const QString& e ){

		*key = e ;

		if( e.isEmpty() ){

			l.exit( 1 ) ;
		}else{
			l.exit( 0 ) ;
		}

	},pluginString,exe ) ;

	return l.exec() ;
}

class translator
{
public:
	void set( const QByteArray& e )
	{
		QCoreApplication::installTranslator( [ & ](){

			if( m_translator ){

				QCoreApplication::removeTranslator( m_translator ) ;

				delete m_translator ;
			}

			m_translator = new QTranslator() ;

			m_translator->load( e.constData(),utility::localizationLanguagePath() ) ;

			return m_translator ;
		}() ) ;
	}
	~translator()
	{
		//QCoreApplication::removeTranslator( m_translator ) ;
		delete m_translator ;
	}

private:
	QTranslator * m_translator = nullptr ;
} static _translator ;

static void _selectOption( QMenu * m,const QString& opt )
{
	utility::selectMenuOption s( m,false ) ;
	s.selectOption( opt ) ;
}

void utility::setLocalizationLanguage( bool translate,QMenu * m )
{
	auto r = utility::localizationLanguage().toLatin1() ;

	if( translate ){

		_translator.set( r ) ;
	}else{
		QDir d( utility::localizationLanguagePath() ) ;

		auto t = d.entryList() ;

		t.removeOne( "." ) ;
		t.removeOne( ".." ) ;

		for( auto& it : t ){

			m->addAction( it.remove( ".qm" ) )->setCheckable( true ) ;
		}

		_selectOption( m,r ) ;
	}
}

void utility::languageMenu( QWidget * w,QMenu * m,QAction * ac )
{
	Q_UNUSED( w ) ;

	auto e = ac->text().remove( '&' ) ;

	utility::setLocalizationLanguage( e ) ;

	utility::setLocalizationLanguage( true,m ) ;

	_selectOption( m,e ) ;
}

QString utility::localizationLanguage()
{
	if( _settings->contains( "Language" ) ){

		return _settings->value( "Language" ).toString() ;
	}else{
		_settings->setValue( "Language",QString( "en_US" ) ) ;
		return "en_US" ;
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

	auto l = d.entryList() ;

	l.removeOne( "." ) ;
	l.removeOne( ".." ) ;

	return l ;
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

QString utility::homePath()
{
	return QDir::homePath() ;
}

bool utility::pathIsReadable( const QString& path )
{
	return QDir( path ).isReadable() ;
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
		e = utility::homePath() + "/.SiriKali" ;

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
		utility::autoOpenFolderOnMount( true ) ;
		return true ;
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
		utility::autoCheck( false ) ;
		return false ;
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
		utility::readOnlyWarning( false ) ;
		return false ;
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
		utility::doNotShowReadOnlyWarning( false ) ;
		return false ;
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
		utility::autoMountFavoritesOnStartUp( false ) ;
		return false ;
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
		}else{
			return utility::walletBackEnd() ;
		}
	}else{
		_settings->setValue( "AutoMountPassWordBackEnd",QString( "internal" ) ) ;
		return LXQt::Wallet::BackEnd::internal ;
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
	if( _settings->contains( "checkForUpdateInterval" ) ){

		return _settings->value( "checkForUpdateInterval" ).toInt() * 1000 ;
	}else{
		_settings->setValue( "checkForUpdateInterval",int( 300 ) ) ;
		return 300 * 1000 ;
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
	Q_UNUSED( dialog ) ;

	if( utility::platformIsLinux() ){

		*localParent = parent ;
	}else{
		*localParent = parent ;
		//*localParent = dialog ;
	}
}
