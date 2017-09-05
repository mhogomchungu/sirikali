/*
 *
 *  Copyright (c) 2012-2015
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

#include "sirikali.h"
#include "ui_sirikali.h"
#include <QDebug>

#include <QMainWindow>

#include <QTableWidgetItem>
#include <QDir>
#include <QIcon>
#include <QAction>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QFont>
#include <QCursor>
#include <QAction>
#include <QFileDialog>
#include <QUrl>
#include <QTranslator>
#include <QMimeData>
#include <QFile>

#include <utility>
#include <initializer_list>
#include <memory>

#include <unistd.h>

#include "filemanager.h"
#include "dialogmsg.h"
#include "tablewidget.h"
#include "oneinstance.h"
#include "utility.h"
#include "siritask.h"
#include "checkforupdates.h"
#include "favorites.h"
#include "walletconfig.h"
#include "plugins.h"

#include "json.h"

sirikali::sirikali() :
	m_secrets( this ),
	m_mountInfo( this,true,[ & ](){ QCoreApplication::exit( m_exitStatus ) ; } )
{
}

void sirikali::closeApplication( int s,const QString& e )
{
	utility::quitHelper() ;

	m_exitStatus = s ;

	if( !e.isEmpty() ){

		utility::debug() << e ;
	}

	if( m_ui ){

		this->hide() ;
		utility::Task::suspendForOneSecond() ;
	}

	m_mountInfo.stop() ;
}

void sirikali::setUpApp( bool start,const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::sirikali ;
	m_ui->setupUi( this ) ;

	m_ui->pbcreate->setMinimumHeight( 31 ) ;
	m_ui->pbunlockvolume->setMinimumHeight( 31 ) ;
	m_ui->pbmenu->setMinimumHeight( 31 ) ;
	m_ui->pbupdate->setMinimumHeight( 31 ) ;
	m_ui->pbFavorites->setMinimumHeight( 31 ) ;

	auto table = m_ui->tableWidget ;

	const auto dimensions = utility::getWindowDimensions() ;

	this->window()->setGeometry( dimensions.geometry() ) ;

	table->setColumnWidth( 0,dimensions.columnWidthAt( 0 ) ) ;
	table->setColumnWidth( 1,dimensions.columnWidthAt( 1 ) ) ;
	table->setColumnWidth( 2,dimensions.columnWidthAt( 2 ) ) ;
	table->setColumnWidth( 3,dimensions.columnWidthAt( 3 ) ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbupdate,SIGNAL( clicked() ),
		 this,SLOT( pbUpdate() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbunlockvolume,SIGNAL( clicked() ),
		 this,SLOT( unlockVolume() ) ) ;

	m_ui->pbcreate->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( createVolume( QAction * ) ) ) ;

		auto _enable = []( QAction * ac,const QString& exe ){

			if( utility::executableFullPath( exe.toLower() ).isEmpty() ){

				ac->setEnabled( false ) ;

				if( exe == "Ecryptfs" ){

					ac->setText( tr( "%1 Is Not Installed" ).arg( "Ecryptfs-simple" ) ) ;
				}else{
					ac->setText( tr( "%1 Is Not Installed" ).arg( exe ) ) ;
				}
			}
		} ;

		_enable( m->addAction( "Cryfs" ),"Cryfs" ) ;
		_enable( m->addAction( "Gocryptfs" ),"Gocryptfs" ) ;
		_enable( m->addAction( "Securefs" ),"Securefs" ) ;
		_enable( m->addAction( "Encfs" ),"Encfs" ) ;
		_enable( m->addAction( "Ecryptfs" ),"Ecryptfs" ) ;

		return m ;
	}() ) ;

	m_autoOpenFolderOnMount = this->autoOpenFolderOnMount() ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	const auto icon = utility::getIcon() ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( icon ) ;

	this->setUpAppMenu() ;

	this->setLocalizationLanguage( false ) ;

	m_trayIcon.setParent( this ) ;
	m_trayIcon.setIcon( icon ) ;

	m_trayIcon.show() ;

	this->disableAll() ;

	this->updateVolumeList( mountinfo::unlockedVolumes().await() ) ;

	if( volume.isEmpty() ) {

		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}

	this->startGUI() ;

	QTimer::singleShot( utility::checkForUpdateInterval(),this,SLOT( autoUpdateCheck() ) ) ;

	if( !start ){

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Enable Polkit Support. \n\"Ecryptfs-simple\" Support Is Broken." ) ) ;
	}
}

void sirikali::setUpAppMenu()
{
	auto m = new QMenu( this ) ;

	m->setFont( this->font() ) ;

	auto _addAction = [ this ]( bool p,bool q,const QString& e,const char * z,const char * s ){

		auto ac = new QAction( e,this ) ;

		m_actionPair.append( { ac,z } ) ;

		ac->setFont( this->font() ) ;

		if( p ){

			ac->setCheckable( p ) ;
			ac->setChecked( q ) ;

			if( s ){

				connect( ac,SIGNAL( toggled( bool ) ),this,s ) ;
			}

		}else if( s ){

			connect( ac,SIGNAL( triggered() ),this,s ) ;
		}

		return ac ;
	} ;

	auto _addMenu = [ this ]( QMenu * m,const QString& r,
			const char * z,const char * t,const char * s ){

		auto e = m->addMenu( r ) ;

		m_menuPair.append( { e,z } ) ;

		e->setFont( this->font() ) ;

		if( t ){

			connect( e,SIGNAL( triggered( QAction * ) ),this,t ) ;
		}

		if( s ){

			connect( e,SIGNAL( aboutToShow() ),this,s ) ;
		}

		return e ;
	} ;

	auto _addWalletEntries = [ this,&_addMenu,&_addAction ]( QMenu * menu,bool checkable,
			const QString& translatedText,const char * untranslatedText,
			const char * slot_a,const char * slot_b ){

		auto q = _addMenu( menu,translatedText,untranslatedText,slot_a,slot_b ) ;

		auto d = utility::autoMountBackEnd() ;

		using bk = LXQt::Wallet::BackEnd ;

		auto _addOption = [ & ]( const QString& translatedTxt,const char * untranslatedTxt,
				utility::walletBackEnd s ){

			auto ac = _addAction( checkable,false,translatedTxt,untranslatedTxt,nullptr ) ;

			if( checkable ){

				ac->setChecked( d == s ) ;
			}

			if( s.isValid() ){

				ac->setEnabled( LXQt::Wallet::backEndIsSupported( s.bk() ) ) ;
			}else{
				ac->setEnabled( true ) ;
			}

			q->addAction( ac ) ;
		} ;

		_addOption( tr( "Internal Wallet" ),"Internal Wallet",bk::internal ) ;
		_addOption( tr( "KDE Wallet" ),"KDE Wallet",bk::kwallet ) ;
		_addOption( tr( "Gnome Wallet" ),"Gnome Wallet",bk::libsecret ) ;
		_addOption( tr( "OSX KeyChain" ),"OSX KeyChain",bk::osxkeychain ) ;
		_addOption( tr( "None" ),"None",utility::walletBackEnd() ) ;

		return q ;
	} ;

	m->addAction( _addAction( true,m_autoOpenFolderOnMount,tr( "Auto Open Mount Point" ),
				  "Auto Open Mount Point",SLOT( autoOpenFolderOnMount( bool ) ) ) ) ;

	m->addAction( _addAction( true,utility::reUseMountPoint(),tr( "Reuse Mount Point" ),
				  "Reuse Mount Point",SLOT( reuseMountPoint( bool ) ) ) ) ;

	m->addAction( _addAction( true,checkForUpdates::autoCheck(),tr( "Autocheck For Updates" ),
				  "Autocheck For Updates",SLOT( autoCheckUpdates( bool ) ) ) ) ;

	if( utility::platformIsLinux() ){

		m->addAction( _addAction( true,utility::enablePolkitSupport(),
					  tr( "Enable Polkit Support" ),
					  "Enable Polkit Support",
					  SLOT( enablePolkitSupport( bool ) ) ) ) ;
	}

	m->addAction( _addAction( false,false,tr( "Set Mount Point Prefix" ),
				  "Set Mount Point Prefix",SLOT( setDefaultMountPointPrefix() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Set File Manager" ),
				  "Set File Manager",SLOT( setFileManager() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Set External Plugin Executable" ),
				  "Set External Plugin Executable",SLOT( setPluginExecutable() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Unmount All" ),
				  "Unmount All",SLOT( unMountAll() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Unmount All And Quit" ),
				  "Unmount All And Quit",SLOT( unMountAllAndQuit() ) ) ) ;

	m->addMenu( [ this,m,&_addMenu,&_addAction,&_addWalletEntries ](){

		auto e = _addMenu( m,tr( "AutoMount Favorites" ),"AutoMount Favorites",nullptr,nullptr ) ;

		m_autoMountKeyStorage = _addWalletEntries( e,true,tr( "AutoMount Key Source" ),
							   "AutoMount Key Source",
							   SLOT( autoMountKeySource( QAction * ) ),
							   SLOT( autoMountKeyStorage() ) ) ;

		e->addMenu( m_autoMountKeyStorage ) ;

		e->addSeparator() ;

		e->addAction( _addAction( true,utility::autoMountFavoritesOnStartUp(),
					  tr( "AutoMount Favorite Volumes At Start Up" ),
					  "AutoMount Favorite Volumes At Start Up",
					   SLOT( autoMountFavoritesOnStartUp( bool ) ) ) ) ;

		e->addAction(  _addAction( true,utility::autoMountFavoritesOnAvailable(),
					     tr( "AutoMount Favorite Volumes When Available" ),
					     "AutoMount Favorite Volumes When Available",
					     SLOT( autoMountWhenAvailable( bool ) ) ) ) ;

		e->addAction( _addAction( true,utility::showMountDialogWhenAutoMounting(),
					  tr( "Show Mount Dialog When AutoMounting" ),
					  "Show Mount Dialog When AutoMounting",
					  SLOT( showMountDialogWhenAutoMounting( bool ) ) ) ) ;
		return e ;
	}() ) ;

	m_change_password_action = [ m,&_addMenu,&_addAction ](){

		auto e = _addMenu( m,tr( "Internal Wallet" ),"Internal Wallet",nullptr,nullptr ) ;

		auto ac = _addAction( false,false,tr( "Change Password" ),"Change Password",
				       SLOT( changeInternalWalletPassWord() ) ) ;

		e->addAction( ac ) ;

		return ac ;
	}() ;

	m_key_manager_menu = _addWalletEntries( m,false,tr( "Key Storage" ),
						"Key Storage",
						SLOT( keyManagerClicked( QAction * ) ),
						SLOT( aboutToShowMenu() ) ) ;

	m_ui->pbFavorites->setMenu( _addMenu( new QMenu( this ),tr( "Favorites" ),"Favorites",
				    SLOT( favoriteClicked( QAction * ) ),
				    SLOT( showFavorites() ) ) ) ;

	m_language_menu = _addMenu( m,tr( "Select Language" ),"Select Language",
				    SLOT( languageMenu( QAction * ) ),nullptr ) ;

	m->addAction( _addAction( false,false,tr( "Check For Updates" ),"Check For Updates",
				  SLOT( updateCheck() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "About" ),"About",SLOT( licenseInfo() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Show/Hide" ),
				  "Show/Hide",SLOT( slotTrayClicked() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Quit" ),"Quit",SLOT( closeApplication() ) ) ) ;

	this->showFavorites() ;

	m_ui->pbmenu->setMenu( m ) ;

	m_trayIcon.setContextMenu( m ) ;

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
}

void sirikali::setFileManager()
{
	fileManager::instance( this,m_folderOpener,true ) ;
}

void sirikali::setPluginExecutable()
{
	fileManager::instance( this,m_folderOpener,false ) ;
}

void sirikali::autoMountKeyStorage()
{
	auto s = utility::autoMountBackEnd() ;

	auto a = tr( "Internal Wallet" ).remove( '&' ) ;
	auto b = tr( "KDE Wallet" ).remove( '&' ) ;
	auto c = tr( "Gnome Wallet" ).remove( '&' ) ;
	auto d = tr( "None" ).remove( '&' ) ;
	auto f = tr( "OSX KeyChain" ).remove( '&' ) ;


	for( const auto& it: m_autoMountKeyStorage->actions() ){

		it->setChecked( [ & ](){

			auto e = it->text().remove( '&' ) ;

			if( e == a ){

				return s == LXQt::Wallet::BackEnd::internal ;

			}else if( e == b ){

				return s == LXQt::Wallet::BackEnd::kwallet ;

			}else if( e == c ){

				return s == LXQt::Wallet::BackEnd::libsecret ;

			}else if( e == f ){

				return s == LXQt::Wallet::BackEnd::osxkeychain ;
			}else{
				return e == d && s.isInvalid() ;
			}
		}() ) ;
	}
}

void sirikali::autoMountKeySource( QAction * e )
{
	utility::autoMountBackEnd( [ e ]()->utility::walletBackEnd{

		auto a = e->text().remove( '&' ) ;

		auto b = tr( "Internal Wallet" ).remove( '&' ) ;

		auto c = tr( "KDE Wallet" ).remove( '&' ) ;

		auto d = tr( "Gnome Wallet" ).remove( '&' ) ;

		auto e = tr( "OSX KeyChain" ).remove( '&' ) ;

		if( a == b ){

			return LXQt::Wallet::BackEnd::internal ;

		}else if( a == c ){

			return LXQt::Wallet::BackEnd::kwallet ;

		}else if( a == d ){

			return LXQt::Wallet::BackEnd::libsecret ;

		}else if( a == e ){

			return LXQt::Wallet::BackEnd::osxkeychain ;
		}else{
			return utility::walletBackEnd() ;
		}
	}() ) ;
}

void sirikali::showMountDialogWhenAutoMounting( bool e )
{
	utility::showMountDialogWhenAutoMounting( e ) ;
}

void sirikali::autoMountWhenAvailable( bool e )
{
	utility::autoMountFavoritesOnAvailable( e ) ;
}

void sirikali::setDefaultMountPointPrefix()
{
	auto e = QFileDialog::getExistingDirectory( this,QString(),QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	if( !e.isEmpty() ){

		while( true ){

			if( e.endsWith( '/' ) ){

				e.truncate( e.length() - 1 ) ;
			}else{
				break ;
			}
		}

		utility::setDefaultMountPointPrefix( e ) ;
	}
}

void sirikali::aboutToShowMenu()
{
	auto a = utility::walletName() ;
	auto b = utility::applicationName() ;
	auto c = LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
	m_change_password_action->setEnabled( c ) ;
}

void sirikali::changeInternalWalletPassWord()
{
	auto a = utility::walletName() ;
	auto b = utility::applicationName() ;

	m_secrets.changeInternalWalletPassword( a,b ) ;
}

void sirikali::keyManagerClicked( QAction * ac )
{
	auto e = ac->text().remove( '&' ) ;

	if( e != tr( "None" ).remove( '&' ) ){

		walletconfig::instance( this,m_secrets.walletBk( [ & ](){

			if( e == tr( "KDE Wallet" ).remove( '&' ) ){

				return LXQt::Wallet::BackEnd::kwallet ;

			}else if( e == tr( "Gnome Wallet" ).remove( '&' ) ){

				return LXQt::Wallet::BackEnd::libsecret ;

			}else if( e == tr( "OSX KeyChain" ).remove( '&' ) ){

				return LXQt::Wallet::BackEnd::osxkeychain ;
			}else{
				return LXQt::Wallet::BackEnd::internal ;
			}
		}() ) ) ;
	}
}

void sirikali::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void sirikali::updateCheck()
{
	checkForUpdates::check( this,false ) ;
}

void sirikali::autoUpdateCheck()
{
	checkForUpdates::check( this,true ) ;
}

void sirikali::autoCheckUpdates( bool e )
{
	checkForUpdates::autoCheck( e ) ;
}

void sirikali::favoriteClicked( QAction * ac )
{
	auto e = ac->text().remove( '&' ) ;

	if( e == tr( "Manage Favorites" ).remove( '&' ) ){

		favorites::instance( this ) ;
	}else{
		if( e == tr( "Mount All" ).remove( '&' ) ){

			QVector< std::pair< favorites::entry,QByteArray > > s ;

			for( auto&& it : utility::readFavorites() ){

				s.append( { std::move( it ),QByteArray() } ) ;
			}

			this->mountMultipleVolumes( std::move( s ) ) ;
		}else{
			for( const auto& it : utility::readFavorites() ){

				if( it.volumePath == e ){

					auto s = this->autoUnlockVolumes( { { it,QByteArray() } },true ) ;

					if( !s.isEmpty() ){

						this->mountMultipleVolumes( std::move( s ) ) ;
					}

					break ;
				}
			}
		}
	}
}

void sirikali::showFavorites()
{
	utility::readFavorites( m_ui->pbFavorites->menu(),true,
				tr( "Manage Favorites" ),tr( "Mount All" ) ) ;
}

void sirikali::setLocalizationLanguage( bool translate )
{
	utility::setLocalizationLanguage( translate,m_language_menu,m_translator ) ;
}

void sirikali::languageMenu( QAction * ac )
{
	utility::languageMenu( m_language_menu,ac,m_translator ) ;

	m_ui->retranslateUi( this ) ;

	for( auto& it : m_actionPair ){

		it.first->setText( tr( it.second ) ) ;
	}

	for( auto& it : m_menuPair ){

		it.first->setTitle( tr( it.second ) ) ;
	}
}

void sirikali::autoOpenFolderOnMount( bool e )
{
	m_autoOpenFolderOnMount = e ;
	utility::autoOpenFolderOnMount( e ) ;
}

void sirikali::reuseMountPoint( bool e )
{
	utility::reUseMountPoint( e ) ;
}

void sirikali::enablePolkitSupport( bool e )
{
	utility::enablePolkitSupport( e ) ;

	DialogMsg( this ).ShowUIOK( tr( "Warning" ),tr( "A Restart Is Required For The Change To Take Effect." ) );
}

void sirikali::autoMountFavoritesOnStartUp( bool e )
{
	utility::autoMountFavoritesOnStartUp( e ) ;
}

bool sirikali::autoOpenFolderOnMount( void )
{
	return utility::autoOpenFolderOnMount() ;
}

void sirikali::startGUI()
{
	if( !m_startHidden ){

		this->raiseWindow() ;
	}

	if( utility::autoMountFavoritesOnStartUp() ){

		this->autoUnlockVolumes() ;
	}
}

void sirikali::raiseWindow( const QString& volume )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
	this->showMoungDialog( volume ) ;
}

int sirikali::start( QApplication& e )
{
	QCoreApplication::setApplicationName( "SiriKali" ) ;

	const auto l = QCoreApplication::arguments() ;

	if( utility::printVersionOrHelpInfo( l ) ){

		return 0 ;
	}else{
		QMetaObject::invokeMethod( this,
					   "start",
					   Qt::QueuedConnection,
					   Q_ARG( QStringList,l ) ) ;
		return e.exec() ;
	}
}

void sirikali::start( const QStringList& l )
{
	m_startHidden  = l.contains( "-e" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m",utility::fileManager() ) ;

	utility::createFolder( utility::homeConfigPath() ) ;

	auto _cliCommand = [ & ](){

		return l.contains( "-s" ) ||
		       l.contains( "-u" ) ||
		       l.contains( "-p" ) ||
		       !utility::cmdArgumentValue( l,"-b" ).isEmpty() ;
	}() ;

	if( _cliCommand ){

		this->cliCommand( l ) ;
	}else{
		auto s = utility::homeConfigPath( ".tmp" ) ;

		utility::createFolder( s ) ;

		if( utility::pathIsWritable( s ) ){

			oneinstance::callbacks cb = {

				[ this ]( const QString& e ){ utility::startHelper( this,e,"setUpApp" ) ; },
				[ this ](){ this->closeApplication( 1 ) ; },
				[ this ]( const QString& e ){ this->raiseWindow( e ) ; },
			} ;

			auto x = utility::cmdArgumentValue( l,"-d" ) ;

			oneinstance::instance( this,s + "/SiriKali.socket",x,std::move( cb ) ) ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "\"%1\" Folder Must Be Writable" ).arg( s ) ) ;

			this->closeApplication() ;
		}
	}
}

void sirikali::cliCommand( const QStringList& l )
{
	if( l.contains( "-s" ) ){

		auto e = utility::cmdArgumentValue( l,"-f" ) ;

		auto s = plugins::hmac_key( e,utility::readPassword() ) ;

		if( s.isEmpty() ){

			return this->closeApplication( 1 ) ;
		}else{
			return this->closeApplication( 0,s ) ;
		}
	}

	if( l.contains( "-u" ) ){

		m_mountInfo.announceEvents( false ) ;

		auto volume = utility::cmdArgumentValue( l,"-d" ) ;

		for( const auto& it : mountinfo::unlockedVolumes().await() ){

			const auto& a = it.volumePath() ;
			const auto& b = it.mountPoint() ;
			const auto& c = it.fileSystem() ;

			if( a == volume || b == volume ){

				if( siritask::encryptedFolderUnMount( a,b,c ).await() ){

					siritask::deleteMountFolder( b ) ;

					return this->closeApplication( 0 ) ;
				}else{
					break ;
				}
			}
		}

		return this->closeApplication( 1 ) ;
	}

	if( l.contains( "-p" ) ){

		for( const auto& it : mountinfo::unlockedVolumes().await() ){

			it.printVolumeInfo() ;
		}

		return this->closeApplication( 0 ) ;
	}

	if( !utility::cmdArgumentValue( l,"-b" ).isEmpty() ){

		this->unlockVolume( l ) ;
	}
}

void sirikali::unlockVolume( const QStringList& l )
{
	auto vol       = utility::cmdArgumentValue( l,"-d" ) ;
	auto volume    = QDir( vol ).canonicalPath() ;
	auto mountPath = utility::cmdArgumentValue( l,"-z" ) ;
	auto backEnd   = utility::cmdArgumentValue( l,"-b" ) ;
	auto mode      = utility::cmdArgumentValue( l,"-k","rw" ) == "ro" ;
	auto idleTime  = utility::cmdArgumentValue( l,"-i" ) ;
	auto cPath     = utility::cmdArgumentValue( l,"-c" ) ;
	auto keyFile   = utility::cmdArgumentValue( l,"-f" ) ;
	auto mOpt      = utility::cmdArgumentValue( l,"-o" ) ;

	if( vol.isEmpty() ){

		this->closeApplication( 1,tr( "ERROR: Volume Path Not Given." ) ) ;
	}else{
		auto _unlockVolume = [ & ]( const QString& key ){

			auto m = [ & ]()->QString{

				if( mountPath.isEmpty() ){

					auto e = utility::mountPathPostFix( volume.split( "/" ).last() ) ;

					return utility::mountPath( e ) ;
				}else{
					return mountPath ;
				}
			}() ;

			m_mountInfo.announceEvents( false ) ;

			siritask::options s = { volume,m,key,idleTime,cPath,QString(),mode,mOpt,QString() } ;

			auto& e = siritask::encryptedFolderMount( s ) ;

			if( e.await() == siritask::status::success ){

				//this->openMountPointPath( m ) ;

				this->closeApplication( 0 ) ;
			}else{
				this->closeApplication( 1 ) ;
			}
		} ;

		if( backEnd == "stdin" ){

			return _unlockVolume( [ & ]()->QString{

				auto e = utility::readPassword() ;

				if( keyFile.isEmpty() ){ ;

					return e ;
				}else{
					return plugins::hmac_key( keyFile,e ) ;
				}
			}() ) ;
		}

		if( backEnd == "keyfile" ){

			auto key = utility::fileContents( keyFile ) ;

			if( utility::containsAtleastOne( key,'\n','\0','\r' ) ){

				utility::debug() << keyDialog::keyFileError() ;
			}

			return _unlockVolume( key ) ;
		}

		auto w = [ & ](){

			namespace wxt = LXQt::Wallet ;

			auto _supported = [ & ]( wxt::BackEnd e,const char * s ){

				return backEnd == s && wxt::backEndIsSupported( e ) ;
			} ;

			if( _supported( wxt::BackEnd::internal,"internal" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::internal ) ;

				return utility::getKey( volume,s.bk() ) ;

			}else if( _supported( wxt::BackEnd::libsecret,"gnomewallet" ) ||
				  _supported( wxt::BackEnd::libsecret,"libsecret" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::libsecret ) ;

				return utility::getKey( volume,s.bk() ) ;

			}else if( _supported( wxt::BackEnd::kwallet,"kwallet" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::kwallet ) ;

				return utility::getKey( volume,s.bk() ) ;

			}else if( _supported( wxt::BackEnd::osxkeychain,"osxkeychain" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::osxkeychain ) ;

				return utility::getKey( volume,s.bk() ) ;
			}else{
				return utility::wallet{ false,true,"" } ;
			}
		}() ;

		if( w.opened ){

			if( w.key.isEmpty() ){

				this->closeApplication( 1,tr( "ERROR: Key Not Found In The Backend." ) ) ;
			}else{
				_unlockVolume( w.key ) ;
			}
		}else{
			this->closeApplication( 1,tr( "ERROR: Failed To Unlock Requested Backend." ) ) ;
		}
	}
}

void sirikali::mountMultipleVolumes( QVector< std::pair< favorites::entry,QByteArray > > e )
{
	keyDialog::instance( this,m_secrets,m_autoOpenFolderOnMount,m_folderOpener,std::move( e ) ) ;
}

void sirikali::autoMountFavoritesOnAvailable( QString m )
{
	if( utility::autoMountFavoritesOnAvailable() ){

		QVector< std::pair< favorites::entry,QByteArray > > e ;

		for( const auto& it : utility::readFavorites() ){

			if( it.volumePath.startsWith( m ) && it.autoMount() ){

				e.append( { { it,QByteArray() } } ) ;
			}
		}

		this->mountMultipleVolumes( this->autoUnlockVolumes( e ) ) ;
	}
}

void sirikali::autoUnlockVolumes()
{
	QVector< std::pair< favorites::entry,QByteArray > > e ;

	for( const auto& it : utility::readFavorites() ){

		if( it.autoMount() ){

			e.append( { { it,QByteArray() } } ) ;
		}
	}

	this->mountMultipleVolumes( this->autoUnlockVolumes( e ) ) ;
}

QVector< std::pair< favorites::entry,QByteArray > >
sirikali::autoUnlockVolumes( const QVector< std::pair< favorites::entry,QByteArray > > l,
							 bool autoOpenFolderOnMount )
{
	if( l.isEmpty() ){

		return l ;
	}

	auto e = utility::autoMountBackEnd() ;

	if( e.isInvalid() ){

		return l ;
	}

	auto m = m_secrets.walletBk( e.bk() ) ;

	if( !m ){

		return l ;
	}

	auto _mountVolumes = [ & ](){

		QVector< std::pair< favorites::entry,QByteArray > > e ;

		auto _mount = [ & ]( QVector< std::pair< favorites::entry,QByteArray > >& q,
				     const std::pair< favorites::entry,QByteArray >& e,
				     const QByteArray& key,
				     bool s ){
			if( s ){

				q.append( { e.first,key } ) ;
			}else{
				auto& s = siritask::encryptedFolderMount( { e.first,key } ) ;

				s.then( [ this,autoOpenFolderOnMount,e = e.first.mountPointPath ]( siritask::cmdStatus s ){

					if( s == siritask::status::success && autoOpenFolderOnMount ){

						this->openMountPointPath( e ) ;
					}
				} ) ;
			}
		} ;

		auto s = utility::showMountDialogWhenAutoMounting() ;

		for( const auto& it : l ){

			const auto key = m->readValue( it.first.volumePath ) ;

			if( key.isEmpty() ){

				e.append( it ) ;
			}else{
				_mount( e,it,key,s ) ;
			}
		}

		return e ;
	} ;


	if( m->opened() ){

		return _mountVolumes() ;
	}else{
		m->setImage( QIcon( ":/sirikali" ) ) ;

		if( m->open( utility::walletName( m->backEnd() ),utility::applicationName() ) ){

			return _mountVolumes() ;
		}else{
			return l ;
		}
	}
}

void sirikali::ecryptfsProperties()
{
	this->disableAll() ;

	auto s = [ this ](){

		auto table = m_ui->tableWidget ;

		auto item = table->currentItem() ;

		if( item ){

			return table->item( item->row(),1 )->text() ;
		}else{
			return QString() ;
		}
	}() ;

	for( const auto& it : mountinfo::unlockedVolumes().await() ){

		if( it.mountPoint() == s ){

			DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,[ & ](){

				auto s = it.mountOptions() ;

				s.replace( ",","\n\n" ) ;
				s.replace( "ro\n\n","mode=read only\n\n" ) ;
				s.replace( "rw\n\n","mode=read and write\n\n" ) ;
				s.replace( "="," = " ) ;

				return s ;
			}() ) ;

			break ;
		}
	}

	this->enableAll() ;
}

void sirikali::cryfsProperties()
{
	this->disableAll() ;

	auto m = [ this ](){

		auto table = m_ui->tableWidget ;

		auto row = table->currentRow() ;

		if( row >= 0 ){

			return table->item( row,1 )->text().toLatin1() ;
		}else{
			return QByteArray() ;
		}
	}() ;

	auto vfs = utility::fileSystemInfo( m ).await() ;

	if( !vfs.valid ){

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Read Volume Properties" ) ) ;

		return this->enableAll() ;
	}

	DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,[ & ](){

		auto _prettify = []( quint64 s ){

			auto _convert = [ & ]( const char * p,double q ){

				auto e = QString::number( double( s ) / q,'f',2 ) ;

				e.remove( ".00" ) ;

				return QString( "%1 %2" ).arg( e,p ) ;
			} ;

			switch( QString::number( s ).size() ){

				case 0 :
				case 1 : case 2 : case 3 :

					return QString( "%1 B" ).arg( QString::number( s ) ) ;

				case 4 : case 5 : case 6 :

					return _convert( "KB",1024 ) ;

				case 7 : case 8 : case 9 :

					return _convert( "MB",1048576 ) ;

				case 10: case 11 : case 12 :

					return _convert( "GB",1073741824 ) ;

				default:
					return _convert( "TB",1024.0 * 1073741824 ) ;
			}
		} ;

		return QString( [ this ](){

			auto l = { tr( "Block Size: %1" ),
				   tr( "Used Blocks: %2" ),
				   tr( "Free Blocks: %3" ),
				   tr( "Total Blocks %4" ),
				   tr( "Used Space: %5" ),
				   tr( "Free Space: %6" ),
				   tr( "Total Space: %7" ),
				   tr( "Used %: %8" ) } ;

			QString e ;

			for( const auto& it : l ){

				e += it + "\n\n" ;
			}

			e.truncate( e.size() - 2 ) ;

			return e ;

		}() ).arg( [ & ](){

			return _prettify( vfs.f_bsize ) ;

		}(),[ & ](){

			return QString::number( vfs.f_blocks - vfs.f_bavail ) ;

		}(),[ & ](){

			return QString::number( vfs.f_bfree ) ;

		}(),[ & ](){

			return QString::number( vfs.f_blocks ) ;

		}(),[ & ](){

			return _prettify( vfs.f_bsize * ( vfs.f_blocks - vfs.f_bavail ) ) ;

		}(),[ & ](){

			return _prettify( vfs.f_bsize * vfs.f_bavail ) ;

		}(),[ & ](){

			return _prettify( vfs.f_bsize * vfs.f_blocks ) ;

		}(),[ & ]()->QString{

			if( vfs.f_bfree == 0 ){

				return "100%" ;
			}else{
				quint64 s = vfs.f_blocks - vfs.f_bavail ;

				auto e = double( s ) / double( vfs.f_blocks ) ;

				return QString::number( e * 100,'f',2 ) + "%" ;
			}
		}() ) ;
	}() ) ;

	this->enableAll() ;
}

static void _volume_properties( const QString& cmd,const QString& arg,
				QTableWidget * table,QWidget * w )
{
	auto exe = utility::executableFullPath( cmd ) ;

	auto path = [ table ](){

		auto row = table->currentRow() ;

		if( row < 0 ){

			return QString() ;
		}else{
			return utility::Task::makePath( table->item( row,0 )->text() ) ;
		}
	}() ;

	if( exe.isEmpty() ){

		DialogMsg( w ).ShowUIOK( QObject::tr( "ERROR" ),
					 QObject::tr( "Failed To Find %1 Executable" ).arg( cmd ) ) ;
	}else{
		auto e = utility::Task::run( exe + arg + path ).await() ;

		if( e.success() ){

			auto s = e.stdOut() ;

			if( cmd == "gocryptfs" ){

				s.replace( "Creator:      ","Creator: " ).replace( "\n","\n\n" ) ;
			}

			DialogMsg( w ).ShowUIInfo( QObject::tr( "INFORMATION" ),true,s ) ;
		}else{
			DialogMsg( w ).ShowUIOK( QObject::tr( "ERROR" ),
						 QObject::tr( "Failed To Get Volume Properties" ) ) ;
		}
	}
}

void sirikali::encfsProperties()
{
	this->disableAll() ;

	_volume_properties( "encfsctl"," ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::securefsProperties()
{
	this->disableAll() ;

	_volume_properties( "securefs"," info ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::gocryptfsProperties()
{
	this->disableAll() ;

	_volume_properties( "gocryptfs"," -info ",m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	struct volumeType{ const char * slot ; bool enabled ; } ;

	QMenu m ;

	m.setFont( this->font() ) ;

	auto _addAction = [ & ]( const auto& txt,const volumeType& e ){

		auto ac = m.addAction( txt ) ;

		if( e.enabled ){

			ac->setEnabled( true ) ;

			connect( ac,SIGNAL( triggered() ),this,e.slot ) ;
		}else{
			ac->setEnabled( false ) ;
		}
	} ;

	_addAction( tr( "Open Folder" ),{ SLOT( slotOpenFolder() ),true } ) ;

	_addAction( tr( "Unmount" ),{ SLOT( pbUmount() ),true } ) ;

	_addAction( tr( "Properties" ),[ this ]()->volumeType{

		auto table = m_ui->tableWidget ;

		auto row = table->currentRow() ;

		if( row >= 0 ){

			auto e = table->item( row,2 )->text() ;

			if( e == "cryfs" ){

				return { SLOT( cryfsProperties() ),true } ;

			}else if( e == "ecryptfs" ){

				return { SLOT( ecryptfsProperties() ),true } ;

			}else if( e == "securefs" ){

				return { SLOT( securefsProperties() ),true } ;

			}else if( e == "encfs" ){

				return { SLOT( encfsProperties() ),true } ;

			}else if( e == "gocryptfs" ){

				return { SLOT( gocryptfsProperties() ),true } ;
			}
		}

		return { nullptr,false } ;
	}() ) ;

	m.addSeparator() ;

	m.addAction( tr( "Close Menu" ) ) ;

	if( itemClicked ){

		m.exec( QCursor::pos() ) ;
	}else{
		auto p = this->pos() ;

		auto x = p.x() + 100 + m_ui->tableWidget->columnWidth( 0 ) ;
		auto y = p.y() + 50 + m_ui->tableWidget->rowHeight( 0 ) * item->row() ;

		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

void sirikali::itemClicked( QTableWidgetItem * item )
{
	this->showContextMenu( item,true ) ;
}

void sirikali::defaultButton()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		this->showContextMenu( table->currentItem(),false ) ;
	}
}

void sirikali::slotOpenSharedFolder()
{
	this->openMountPoint( m_sharedFolderPath ) ;
}

void sirikali::slotOpenFolder()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto item = table->currentItem() ;
		auto path = table->item( item->row(),1 )->text() ;

		this->openMountPoint( path ) ;
	}
}

void sirikali::openMountPoint( const QString& m_point )
{
	auto x = tr( "WARNING" ) ;
	auto y = tr( "Could Not Open Mount Point Because \"%1\" Tool Does Not Appear To Be Working Correctly." ).arg( m_folderOpener ) ;

	utility::openPath( m_point,m_folderOpener,this,x,y ) ;
}

void sirikali::openMountPointPath( const QString& m )
{
	if( m_autoOpenFolderOnMount ){

		this->openMountPoint( m ) ;
	}
}

void sirikali::hideWindow()
{
	this->hide() ;
}

void sirikali::setUpShortCuts()
{
	auto _addAction = [ this ]( std::initializer_list< QKeySequence > s,const char * slot ){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( s ) ;

		connect( ac,SIGNAL( triggered() ),this,slot ) ;

		return ac ;
	} ;

	this->addAction( _addAction( { Qt::Key_Enter,Qt::Key_Return },SLOT( defaultButton() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_M },SLOT( createVolume() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_U },SLOT( pbUmount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_R },SLOT( pbUpdate() ) ) ) ;

	this->addAction( _addAction( { Qt::CTRL + Qt::Key::Key_W },SLOT( hideWindow() ) ) ) ;

	this->addAction( [ & ](){

		auto e = _addAction( { Qt::CTRL + Qt::Key::Key_Q },SLOT( closeApplication() ) ) ;

		e->setMenuRole( QAction::QuitRole ) ;

		e->setText( tr( "&Quit" ) ) ;

		return e ;
	}() ) ;


}

void sirikali::setUpFont()
{
}

void sirikali::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

void sirikali::slotTrayClicked( QSystemTrayIcon::ActivationReason e )
{
	if( e == QSystemTrayIcon::Trigger ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	}
}

void sirikali::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

void sirikali::dropEvent( QDropEvent * e )
{
	QVector< std::pair< favorites::entry,QByteArray > > s ;

	for( const auto& it : e->mimeData()->urls() ){

		s.append( { it.path(),QByteArray() } ) ;
	}

	this->mountMultipleVolumes( std::move( s ) ) ;
}

void sirikali::mount( const volumeInfo& entry,const QString& exe,const QByteArray& key )
{
	this->disableAll() ;

	auto s = [ this ](){ this->enableAll() ; } ;

	keyDialog::instance( this,m_secrets,entry,std::move( s ),
			     m_autoOpenFolderOnMount,m_folderOpener,exe,key ) ;
}

void sirikali::createVolume( QAction * ac )
{
	if( ac ){

		this->mount( volumeInfo(),ac->text().remove( '&' ) ) ;
	}else{
		this->mount( volumeInfo(),"Cryfs" ) ;
	}
}

void sirikali::slotMount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		this->mount( tablewidget::rowEntries( table,row ) ) ;
	}
}

void sirikali::showMoungDialog( const volumeInfo& v )
{
	if( v.isNotValid() ){

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),
					    tr( "Permission To Access The Volume Was Denied\nOr\nThe Volume Is Not Supported" ) ) ;

		this->enableAll() ;
	}else{
		this->mount( v ) ;
	}
}

void sirikali::showMoungDialog( const QString& volume,const QString& m_point )
{
	if( !volume.isEmpty() ){

		this->mount( { volume,m_point } ) ;
	}
}

void sirikali::unlockVolume()
{
	this->disableAll() ;

	auto e = tr( "Select An Encrypted Volume Directory" ) ;
	auto path = QFileDialog::getExistingDirectory( this,e,utility::homePath(),QFileDialog::ShowDirsOnly ) ;

	while( true ){

		if( path.endsWith( '/' ) ){

			path.truncate( path.length() - 1 ) ;
		}else{
			break ;
		}
	}

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->showMoungDialog( path ) ;
	}
}

QFont sirikali::getSystemVolumeFont()
{
	auto f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

void sirikali::addEntryToTable( const QStringList& l )
{
	tablewidget::addRow( m_ui->tableWidget,l ) ;
}

void sirikali::addEntryToTable( const volumeInfo& e )
{
	this->addEntryToTable( e.mountInfo().minimalList() ) ;
}

void sirikali::removeEntryFromTable( QString volume )
{
	auto table = m_ui->tableWidget ;

	auto r = tablewidget::columnHasEntry( table,volume ) ;

	if( r != -1 ){

		tablewidget::deleteRow( table,r ) ;
		this->enableAll() ;
	}else{
		this->pbUpdate() ;
	}
}

void sirikali::updateList( const volumeInfo& entry )
{
	if( entry.isValid() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,entry.volumePath() ) ;

		if( row == -1 ){

			row = tablewidget::addRow( table ) ;
		}

		tablewidget::updateRow( table,entry.mountInfo().minimalList(),row,this->font() ) ;

		tablewidget::selectRow( table,row ) ;
	}
}

void sirikali::pbUmount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		this->disableAll() ;

		auto row = table->currentRow() ;

		auto type = table->item( row,2 )->text() ;

		auto a = table->item( row,0 )->text() ;
		auto b = table->item( row,1 )->text() ;
		auto c = table->item( row,2 )->text() ;

		utility::Task::suspendForOneSecond() ;

		if( siritask::encryptedFolderUnMount( a,b,c ).await() ){

			siritask::deleteMountFolder( b ) ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Unmount %1 Volume" ).arg( type ) ) ;

			this->enableAll() ;
		}
	}
}

void sirikali::unMountAll()
{
	m_mountInfo.announceEvents( false ) ;

	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	const auto cipherFolders = tablewidget::columnEntries( table,0 ) ;
	const auto mountPoints   = tablewidget::columnEntries( table,1 ) ;
	const auto fileSystems   = tablewidget::columnEntries( table,2 ) ;

	utility::Task::suspendForOneSecond() ;

	for( auto r = cipherFolders.size() - 1 ; r >= 0 ; r-- ){

		const auto& a = cipherFolders.at( r ) ;
		const auto& b = mountPoints.at( r ) ;
		const auto& c = fileSystems.at( r ) ;

		if( siritask::encryptedFolderUnMount( a,b,c ).await() ){

			tablewidget::deleteRow( table,b,1 ) ;

			siritask::deleteMountFolder( b ) ;

			utility::Task::suspendForOneSecond() ;
		}
	}

	this->enableAll() ;

	m_mountInfo.announceEvents( true ) ;
}

void sirikali::unMountAllAndQuit()
{
	this->unMountAll() ;

	if( m_ui->tableWidget->rowCount() == 0 ){

		this->closeApplication() ;
	}
}

void sirikali::pbUpdate()
{
	this->disableAll() ;

	this->updateVolumeList( mountinfo::unlockedVolumes().await() ) ;
}

void sirikali::updateVolumeList( const QVector< volumeInfo >& r )
{
	tablewidget::clearTable( m_ui->tableWidget ) ;

	for( const auto& it : r ){

		this->updateList( it ) ;
	}

	this->enableAll() ;
}

void sirikali::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void sirikali::disableAll()
{
	m_ui->pbmenu->setEnabled( false ) ;
	m_ui->pbupdate->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbunlockvolume->setEnabled( false ) ;
	m_ui->pbcreate->setEnabled( false ) ;
	m_ui->pbFavorites->setEnabled( false ) ;
}

void sirikali::enableAll()
{
	if( !m_removeAllVolumes ){

		m_ui->pbmenu->setEnabled( true ) ;
		m_ui->pbupdate->setEnabled( true ) ;
		m_ui->tableWidget->setEnabled( true ) ;
		m_ui->tableWidget->setFocus() ;
		m_ui->pbunlockvolume->setEnabled( true ) ;
		m_ui->pbcreate->setEnabled( true ) ;
		m_ui->pbFavorites->setEnabled( true ) ;
	}
}

void sirikali::enableAll_1()
{
	m_removeAllVolumes = false ;
	this->enableAll() ;
}

sirikali::~sirikali()
{
	if( m_ui ){

		auto q = m_ui->tableWidget ;

		const auto& r = this->window()->geometry() ;

		utility::setWindowDimensions( { { { r.x(),
						    r.y(),
						    r.width(),
						    r.height(),
						    q->columnWidth( 0 ),
						    q->columnWidth( 1 ),
						    q->columnWidth( 2 ),
						    q->columnWidth( 3 ) } } } ) ;

		delete m_ui ;
	}
}
