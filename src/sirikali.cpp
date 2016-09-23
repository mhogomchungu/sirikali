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

#include <unistd.h>
#include <sys/vfs.h>

#include "keydialog.h"
#include "dialogmsg.h"
#include "tablewidget.h"
#include "oneinstance.h"
#include "mountinfo.h"
#include "utility.h"
#include "siritask.h"
#include "checkforupdates.h"
#include "favorites.h"
#include "walletconfig.h"
#include <memory>

#include "3rdParty/json.hpp"

sirikali::sirikali( QWidget * parent ) :
	QWidget( parent ),
	m_settings( "SiriKali","SiriKali" ),
	m_mountInfo( mountinfo::instance( this,true,[](){ QCoreApplication::quit() ; } ) )
{
	utility::setSettingsObject( &m_settings ) ;
	m_secrets.setParent( this ) ;
}

/*
 * This should be the only function that closes the application
 */
void sirikali::closeApplication()
{
	m_mountInfo.stop()() ;
}

void sirikali::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::sirikali ;
	m_ui->setupUi( this ) ;

	m_ui->pbcreate->setMinimumHeight( 31 ) ;
	m_ui->pbunlockvolume->setMinimumHeight( 31 ) ;
	m_ui->pbmenu->setMinimumHeight( 31 ) ;
	m_ui->pbupdate->setMinimumHeight( 31 ) ;

	auto f = utility::getWindowDimensions() ;

	auto e = f.data() ;

	this->window()->setGeometry( *( e + 0 ),*( e + 1 ),*( e + 2 ),*( e + 3 ) ) ;

	auto table = m_ui->tableWidget ;

	table->setColumnWidth( 0,*( e + 4 ) ) ;
	table->setColumnWidth( 1,*( e + 5 ) ) ;
	table->setColumnWidth( 2,*( e + 6 ) ) ;
	table->setColumnWidth( 3,*( e + 7 ) ) ;

#if QT_VERSION < QT_VERSION_CHECK( 5,0,0 )
	m_ui->tableWidget->verticalHeader()->setResizeMode( QHeaderView::ResizeToContents ) ;
#else
	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
#endif
	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbupdate,SIGNAL( clicked()),
		 this,SLOT( pbUpdate() ) ) ;

	connect( &m_mountInfo,SIGNAL( gotEvent() ),this,
		 SLOT( pbUpdate() ),Qt::QueuedConnection ) ;

	connect( &m_mountInfo,SIGNAL( gotEvent( QString ) ),
		 this,SLOT( autoMountFavoritesOnAvailable( QString ) ),Qt::QueuedConnection ) ;

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
				ac->setText( tr( "%1 Is Not Installed" ).arg( exe ) ) ;
			}
		} ;

		_enable( m->addAction( "Cryfs" ),"Cryfs" ) ;
		_enable( m->addAction( "Gocryptfs" ),"Gocryptfs" ) ;
		_enable( m->addAction( "Securefs" ),"Securefs" ) ;
		_enable( m->addAction( "Encfs" ),"Encfs" ) ;

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

	auto dirPath = utility::homeConfigPath() ;
	QDir dir( dirPath ) ;

	if( !dir.exists() ){

		dir.mkpath( dirPath ) ;
	}

	this->disableAll() ;

	this->startAutoMonitor() ;

	this->updateVolumeList( siritask::updateVolumeList().await() ) ;

	if( volume.isEmpty() ) {

		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}

	this->autoUpdateCheck() ;
}

void sirikali::setUpAppMenu()
{
	auto m = new QMenu( this ) ;

	m->setFont( this->font() ) ;

	auto _addAction = [ this ]( bool p,bool q,const QString& e,const char * z,const char * s ){

		auto ac = new QAction( e,this ) ;

		m_actionPair.append( { ac,z } ) ;

		if( p ){

			ac->setCheckable( p ) ;
			ac->setChecked( q ) ;

			if( s ){

				connect( ac,SIGNAL( toggled( bool ) ),this,s ) ;
			}
		}else{
			if( s ){

				connect( ac,SIGNAL( triggered() ),this,s ) ;
			}
		}

		return ac ;
	} ;

	auto _addMenu = [ this ]( QMenu * m,const QString& r,
			const char * z,const char * t,const char * s ){

		auto e = m->addMenu( r ) ;

		m_menuPair.append( { m,z } ) ;

		e->setFont( this->font() ) ;

		if( t ){

			connect( e,SIGNAL( triggered( QAction * ) ),this,t ) ;
		}

		if( s ){

			connect( e,SIGNAL( aboutToShow() ),this,s ) ;
		}

		return e ;
	} ;

	m->addAction( _addAction( true,m_autoOpenFolderOnMount,tr( "Auto Open Mount Point" ),
				  "Auto Open Mount Point",SLOT( autoOpenFolderOnMount( bool ) ) ) ) ;

	m->addAction( _addAction( true,utility::reUseMountPoint(),tr( "Reuse Mount Point" ),
				  "Reuse Mount Point",SLOT( reuseMountPoint( bool ) ) ) ) ;

	m->addAction( _addAction( true,checkForUpdates::autoCheck(),tr( "Autocheck For Updates" ),
				  "Autocheck For Updates",SLOT( autoCheckUpdates( bool ) ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Set Mount Point Prefix" ),
				  "Set Mount Point Prefix",SLOT( setDefaultMountPointPrefix() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Unmount All" ),"Unmount All",SLOT( unMountAll() ) ) ) ;

	m->addMenu( [ this,m,&_addMenu,&_addAction ](){

		auto e = _addMenu( m,tr( "AutoMount Favorites" ),"AutoMount Favorites",nullptr,nullptr ) ;

		e->addMenu( [ this,e,&_addMenu,&_addAction ](){

			m_autoMountKeyStorage = _addMenu( e,tr( "AutoMount Key Source" ),"AutoMount Key Source",
							  SLOT( autoMountKeySource( QAction * ) ),
							  SLOT( autoMountKeyStorage() ) ) ;

			auto _addOption = [ & ]( const QString& e,const char * z,LXQt::Wallet::BackEnd s ){

				auto ac = _addAction( true,false,e,z,nullptr ) ;

				ac->setEnabled( LXQt::Wallet::backEndIsSupported( s ) ) ;

				ac->setChecked( s == utility::autoMountBackEnd() ) ;

				m_autoMountKeyStorage->addAction( ac ) ;
			} ;

			_addOption( tr( "Internal Wallet" ),"Internal Wallet",LXQt::Wallet::BackEnd::internal ) ;
			_addOption( tr( "KDE Wallet" ),"KDE Wallet",LXQt::Wallet::BackEnd::kwallet ) ;
			_addOption( tr( "Gnome Wallet" ),"Gnome Wallet",LXQt::Wallet::BackEnd::libsecret ) ;

			return m_autoMountKeyStorage ;
		}() ) ;

		e->addSeparator() ;

		e->addAction( _addAction( true,utility::autoMountFavoritesOnStartUp(),
					  tr( "AutoMount Favorite Volumes At Start Up" ),
					  "AutoMount Favorite Volumes At Start Up",
					   SLOT( autoMountFavoritesOnStartUp( bool ) ) ) ) ;

		e->addAction( _addAction( true,utility::autoMountFavoritesOnAvailable(),
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

	m_key_manager_menu = [ m,&_addMenu,&_addAction ](){

		auto q = _addMenu( m,tr( "Key Storage" ),"Key Storage",
				   SLOT( keyManagerClicked( QAction * ) ),
				   SLOT( aboutToShowMenu() ) ) ;

		auto _addOption = [ & ]( const QString& e,const char * z,LXQt::Wallet::BackEnd s ){

			auto ac = _addAction( false,false,e,z,nullptr ) ;

			ac->setEnabled( LXQt::Wallet::backEndIsSupported( s ) ) ;

			q->addAction( ac ) ;
		} ;

		_addOption( tr( "Internal Wallet" ),"Internal Wallet",LXQt::Wallet::BackEnd::internal ) ;
		_addOption( tr( "KDE Wallet" ),"KDE Wallet",LXQt::Wallet::BackEnd::kwallet ) ;
		_addOption( tr( "Gnome Wallet" ),"Gnome Wallet",LXQt::Wallet::BackEnd::libsecret ) ;

		return q ;
	}() ;

	m_favorite_menu = _addMenu( m,tr( "Favorites" ),"Favorites",
				    SLOT( favoriteClicked( QAction * ) ),
				    SLOT( showFavorites() ) ) ;

	m_language_menu = _addMenu( m,tr( "Select Language" ),"Select Language",
				    SLOT( languageMenu( QAction * ) ),nullptr ) ;

	m->addAction( _addAction( false,false,tr( "Check For Updates" ),"Check For Updates",
				  SLOT( updateCheck() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "About" ),"About",SLOT( licenseInfo() ) ) ) ;

	auto ac = _addAction( false,false,tr( "Quit" ),"Quit",SLOT( closeApplication() ) ) ;

	m->addAction( ac ) ;

	m_ui->pbmenu->setMenu( m ) ;

	m_trayIcon.setContextMenu( [ this,ac,&_addAction ](){

		auto m = new QMenu( this ) ;

		m->addAction( _addAction( false,false,tr( "Show/Hide" ),
					  "Show/Hide",SLOT( slotTrayClicked() ) ) ) ;

		m->addAction( ac ) ;

		return m ;
	}() ) ;

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
}

void sirikali::autoMountKeyStorage()
{
	auto s = utility::autoMountBackEnd() ;

	auto a = tr( "Internal Wallet" ).remove( '&' ) ;
	auto b = tr( "KDE Wallet" ).remove( '&' ) ;
	auto c = tr( "Gnome Wallet" ).remove( '&' ) ;

	for( const auto& it: m_autoMountKeyStorage->actions() ){

		it->setChecked( [ & ](){

			auto e = it->text().remove( '&' ) ;

			if( e == a ){

				return s == LXQt::Wallet::BackEnd::internal ;

			}else if( e == b ){

				return s == LXQt::Wallet::BackEnd::kwallet ;

			}else if( e == c ){

				return s == LXQt::Wallet::BackEnd::libsecret ;
			}else{
				return false ;
			}
		}() ) ;
	}
}

void sirikali::autoMountKeySource( QAction * e )
{
	utility::autoMountBackEnd( [ e ](){

		auto a = e->text().remove( '&' ) ;

		auto b = tr( "Internal Wallet" ).remove( '&' ) ;

		auto c = tr( "KDE Wallet" ).remove( '&' ) ;

		auto d = tr( "Gnome Wallet" ).remove( '&' ) ;

		if( a == b ){

			return LXQt::Wallet::BackEnd::internal ;

		}else if( a == c ){

			return LXQt::Wallet::BackEnd::kwallet ;

		}else if( a == d ){

			return LXQt::Wallet::BackEnd::libsecret ;
		}else{
			return LXQt::Wallet::BackEnd::internal ;
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

void sirikali::startAutoMonitor()
{
	m_mountInfo.start() ;
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
	walletconfig::instance( this ).ShowUI( m_secrets.walletBk( [ ac ](){

		auto e = ac->text().remove( '&' ) ;

		if( e == tr( "KDE Wallet" ).remove( '&' ) ){

			return LXQt::Wallet::BackEnd::kwallet ;

		}else if( e == tr( "Gnome Wallet" ).remove( '&' ) ){

			return LXQt::Wallet::BackEnd::libsecret ;
		}else{
			return LXQt::Wallet::BackEnd::internal ;
		}
	}() ) ) ;
}

void sirikali::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void sirikali::updateCheck()
{
	checkForUpdates::instance( this,false ) ;
}

void sirikali::autoUpdateCheck()
{
	checkForUpdates::instance( this,true ) ;
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

			for( const auto& it : this->autoUnlockVolumes( utility::readFavorites() ) ){

				this->showMoungDialog( it ) ;
			}
		}else{
			for( const auto& it : utility::readFavorites() ){

				if( it.volumePath == e ){

					if( !this->autoUnlockVolumes( { it } ).isEmpty() ){

						this->showMoungDialog( it ) ;
					}

					break ;
				}
			}
		}
	}
}

void sirikali::showFavorites()
{
	utility::readFavorites( m_favorite_menu,true,tr( "Manage Favorites" ),tr( "Mount All" ) ) ;
}

void sirikali::setLocalizationLanguage( bool translate )
{
	utility::setLocalizationLanguage( translate,m_language_menu ) ;
}

void sirikali::languageMenu( QAction * ac )
{
	utility::languageMenu( this,m_language_menu,ac ) ;

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

void sirikali::raiseWindow()
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void sirikali::raiseWindow( QString volume )
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
	this->showMoungDialog( volume ) ;
}

void sirikali::Show()
{
	auto l = QCoreApplication::arguments() ;

	m_startHidden  = l.contains( "-e" ) ;
	m_folderOpener = utility::cmdArgumentValue( l,"-m","xdg-open" ) ;
	m_env          = utility::cmdArgumentValue( l,"-z","" ) ;

	auto volume = utility::cmdArgumentValue( l,"-d" ) ;

	auto b = utility::cmdArgumentValue( l,"-b","" ) ;
	auto k = utility::cmdArgumentValue( l,"-k","rw" ) ;
	auto z = utility::cmdArgumentValue( l,"-z","" ) ;
	auto i = utility::cmdArgumentValue( l,"-i","" ) ;
        auto c = utility::cmdArgumentValue( l,"-c","" ) ;

	if( b.isEmpty() ){

		oneinstance::instance( this,"SiriKali.socket","startGUI",
				       volume,[ this,volume ]( QObject * instance ){

			connect( instance,SIGNAL( raise() ),this,
				 SLOT( raiseWindow() ) ) ;
			connect( instance,SIGNAL( raiseWithDevice( QString ) ),
				 this,SLOT( raiseWindow( QString ) ) ) ;

			this->setUpApp( volume ) ;
		} ) ;
	}else{
		QMetaObject::invokeMethod( this,"unlockVolume",Qt::QueuedConnection,
					   Q_ARG( QString,volume ),Q_ARG( QString,z ),
					   Q_ARG( QString,b ),Q_ARG( QString,i ),
                                           Q_ARG( QString,c ),Q_ARG( bool,k == "ro" ) ) ;
	}
}

void sirikali::unlockVolume( const QString& volume,const QString& mountPath,
                             const QString& backEnd,const QString& mOpt,const QString& cPath,bool mode )
{
	if( volume.isEmpty() ){

		qDebug() << tr( "ERROR: Volume Path Not Given." ) ;
		QCoreApplication::exit( 1 ) ;
	}else{
		auto w = [ & ](){

			namespace wxt = LXQt::Wallet ;

			auto _supported = [ & ]( wxt::BackEnd e,const char * s ){

				return backEnd == s && wxt::backEndIsSupported( e ) ;
			} ;

			if( _supported( wxt::BackEnd::internal,"internal" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::internal ) ;

				return utility::getKey( volume,s.bk() ) ;

			}else if( _supported( wxt::BackEnd::libsecret,"gnomewallet" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::libsecret ) ;

				return utility::getKey( volume,s.bk() ) ;

			}else if( _supported( wxt::BackEnd::kwallet,"kwallet" ) ){

				auto s = m_secrets.walletBk( wxt::BackEnd::kwallet ) ;

				return utility::getKey( volume,s.bk() ) ;
			}else{
				return utility::wallet{ false,true,"" } ;
			}
		}() ;

		if( w.opened ){

			if( w.key.isEmpty() ){

				qDebug() << tr( "ERROR: Key Not Found In The Backend." ) ;
				QCoreApplication::exit( 1 ) ;
			}else{
				auto m = [ & ]()->QString{

					if( mountPath.isEmpty() ){

						auto e = utility::mountPathPostFix( volume.split( "/" ).last() ) ;

						return utility::mountPath( e ) ;
					}else{
						return mountPath ;
					}
				}() ;

				auto o = []( const QString& e ){ Q_UNUSED( e ) ; } ;

				auto e = siritask::encryptedFolderMount( { volume,m,w.key,mOpt,
									    cPath,"",mode,o } ).await() ;

				if( e == siritask::status::success ){

					QCoreApplication::exit( 0 ) ;
				}else{
					qDebug() << tr( "ERROR: Failed To Unlock Requested Volume." ) ;
					QCoreApplication::exit( 1 ) ;
				}
			}
		}else{
			qDebug() << tr( "ERROR: Failed To Unlock Requested Backend." ) ;
			QCoreApplication::exit( 1 ) ;
		}
	}
}

void sirikali::autoMountFavoritesOnAvailable( QString m )
{
	if( utility::autoMountFavoritesOnAvailable() ){

		QVector< favorites::entry > e ;

		for( const auto& it : utility::readFavorites() ){

			if( it.volumePath.startsWith( m ) && it.autoMount() ){

				e.append( it ) ;
			}
		}

		this->autoUnlockVolumes( e ) ;
	}
}

void sirikali::autoUnlockVolumes()
{
	QVector< favorites::entry > e ;

	for( const auto& it : utility::readFavorites() ){

		if( it.autoMount() ){

			e.append( it ) ;
		}
	}

	this->autoUnlockVolumes( e ) ;
}

QVector< favorites::entry > sirikali::autoUnlockVolumes( const QVector< favorites::entry >& l )
{
	if( l.isEmpty() ){

		return l ;
	}

	auto m = m_secrets.walletBk( utility::autoMountBackEnd() ) ;

	if( !m ){

		return l ;
	}

	auto _mountVolumes = [ & ](){

		QVector< favorites::entry > e ;

		auto _mount = [ this ]( const favorites::entry& e,const QByteArray& key,bool s ){

			if( s ){

				this->mount( e,QString(),key ) ;
			}else{
				siritask::options s = { e.volumePath,
							e.mountPointPath,
							key,
							e.idleTimeOut,
							e.configFilePath,
							"",
							false,
							[]( const QString& e ){ Q_UNUSED( e ) ; } } ;

				siritask::encryptedFolderMount( s ).start() ;
			}
		} ;

		auto s = utility::showMountDialogWhenAutoMounting() ;

		for( const auto& it : l ){

			const auto key = m->readValue( it.volumePath ) ;

			if( key.isEmpty() ){

				e.append( it ) ;
			}else{
				_mount( it,key,s ) ;
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

void sirikali::properties()
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

	DialogMsg msg( this ) ;

	struct statfs vfs ;

	if( Task::await< int >( [ & ](){ return statfs( m.constData(),&vfs ) ; } ) ){

		msg.ShowUIOK( tr( "ERROR" ),tr( "Failed To Read Volume Properties" ) ) ;
		return this->enableAll() ;
	}

	msg.ShowUIInfo( tr( "INFORMATION" ),[ & ](){

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

void sirikali::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	auto _addAction = [ & ]( const QString& txt,const char * slot,bool enable ){

		auto ac = m.addAction( txt ) ;
		ac->setEnabled( enable ) ;

		connect( ac,SIGNAL( triggered() ),this,slot ) ;
	} ;

	_addAction( tr( "Open Folder" ),SLOT( slotOpenFolder() ),true ) ;

	_addAction( tr( "Unmount" ),SLOT( pbUmount() ),true ) ;

	_addAction( tr( "Properties" ),SLOT( properties() ),[ this ](){

		auto table = m_ui->tableWidget ;

		auto row = table->currentRow() ;

		if( row >= 0 ){

			return table->item( row,2 )->text() == "cryfs" ;
		}else{
			return false ;
		}
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

	utility::openPath( m_point,m_folderOpener,m_env,this,x,y ) ;
}

void sirikali::openMountPointPath( QString m )
{
	if( m_autoOpenFolderOnMount ){

		this->openMountPoint( m ) ;
	}
}

void sirikali::setUpShortCuts()
{
	auto _addAction = [ this ]( std::initializer_list<QKeySequence> s,const char * slot ){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( s ) ;

		connect( ac,SIGNAL( triggered() ),this,slot ) ;

		return ac ;
	} ;

	this->addAction( _addAction( { Qt::Key_Enter,Qt::Key_Return },SLOT( defaultButton() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_M },SLOT( createVolume() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_U },SLOT( pbUmount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_R },SLOT( pbUpdate() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_C },SLOT( closeApplication() ) ) ) ;
}

void sirikali::setUpFont()
{
	this->setFont( utility::getFont( this ) ) ;
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
	for( const auto& it : e->mimeData()->urls() ){

		this->showMoungDialog( it.path() ) ;
	}
}

void sirikali::mount( const volumeInfo& entry,const QString& exe,const QByteArray& key )
{
	this->disableAll() ;

	keyDialog::instance( this,m_ui->tableWidget,m_secrets,entry,[ this ](){

		this->enableAll() ;

	},[ this ]( const QString& e ){

		this->openMountPointPath( e ) ;

	},exe,key ).ShowUI() ;
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

		DialogMsg msg( this ) ;

		msg.ShowUIOK( tr( "ERROR" ),
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

void sirikali::showMoungDialog( const favorites::entry& e )
{
	this->mount( e ) ;
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
	this->addEntryToTable( e.entryList() ) ;
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

		tablewidget::updateRow( table,entry.entryList(),row,this->font() ) ;

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

		auto m = table->item( row,1 )->text() ;

		if( !siritask::encryptedFolderUnMount( m ).await() ){

			DialogMsg m( this ) ;
			m.ShowUIOK( tr( "ERROR" ),tr( "Failed To Unmount %1 Volume" ).arg( type ) ) ;

			this->enableAll() ;
		}
	}
}

void sirikali::unMountAll()
{
	m_mountInfo.announceEvents( false ) ;

	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	auto l = tablewidget::columnEntries( table,1 ) ;

	int r = l.size() - 1 ;

	if( r < 0 ){

		utility::Task::suspendForOneSecond() ;
	}else{
		do{
			const auto& e = l.at( r ) ;

			if( siritask::encryptedFolderUnMount( e ).await() ){

				tablewidget::deleteRow( table,e,1 ) ;

				siritask::deleteMountFolder( e ) ;

				utility::Task::suspendForOneSecond() ;
			}

			r -= 1 ;

		}while( r >= 0 ) ;
	}

	this->enableAll() ;

	m_mountInfo.announceEvents( true ) ;
}

void sirikali::pbUpdate()
{
	this->disableAll() ;

	this->updateVolumeList( siritask::updateVolumeList().await() ) ;
}

void sirikali::updateVolumeList( const QVector< volumeInfo >& r )
{
	auto l = tablewidget::columnEntries( m_ui->tableWidget,1 ) ;

	tablewidget::clearTable( m_ui->tableWidget ) ;

	for( const auto& it : r ){

		if( it.isValid() ){

			this->updateList( it ) ;
		}

		l.removeOne( it.mountPoint() ) ;
	}

	for( const auto& it : l ){

		siritask::deleteMountFolder( it ) ;
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

		utility::setWindowDimensions( { r.x(),
						r.y(),
						r.width(),
						r.height(),
						q->columnWidth( 0 ),
						q->columnWidth( 1 ),
						q->columnWidth( 2 ),
						q->columnWidth( 3 ) } ) ;

		delete m_ui ;
	}
}
