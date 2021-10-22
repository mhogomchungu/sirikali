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
#include <QApplication>
#include <QCoreApplication>

#include <utility>
#include <initializer_list>
#include <memory>

#include <unistd.h>

#include "createbackendwindow.h"
#include "filemanager.h"
#include "dialogmsg.h"
#include "tablewidget.h"
#include "oneinstance.h"
#include "utility.h"
#include "siritask.h"
#include "checkforupdates.h"
#include "favorites.h"
#include "plugins.h"
#include "crypto.h"
#include "help.h"
#include "configoptions.h"
#include "win.h"
#include "settings.h"
#include "engines.h"
#include "processManager.h"

#include "favorites2.h"
#include "favorites.h"

static std::function< void( const QString& ) > _debug()
{
	return []( const QString& e ){ utility::debug() << e ; } ;
}

static int _print( int s,const QString& m )
{
	std::cout << m.toStdString() << std::endl ;

	return s ;
}

static int _print_err( int s,const QString& m )
{
	std::cerr << m.toStdString() << std::endl ;

	return s ;
}

static int _run( const QString& mm,const QStringList& args,secrets& ss )
{
	if( utility::equalsAtleastOne( mm,"stdin","keyfile" ) ){

		return sirikali::unlockVolume( args,ss ) ;
	}

	if( args.contains( "-b" ) && mm.isEmpty() ){

		return _print_err( 1,QObject::tr( "Unknown Wallet Option" ) ) ;
	}

	if( utility::platformIsWindows() ){

		if( args.size() > 2 && args.at( 1 ) == "-T" ){

			return SiriKali::Windows::terminateProcess( args.at( 2 ).toULong() ) ;
		}
	}

	if( args.contains( "-s" ) ){

		auto e = utility::cmdArgumentValue( args,"-f" ) ;

		if( e.isEmpty() ){

			auto s = crypto::hmac_key( utility::readPassword() ) ;
			return _print( 0,s ) ;
		}else{
			auto s = crypto::hmac_key( e,utility::readPassword() ) ;

			if( s.isEmpty() ){

				return 1 ;
			}else{
				return _print( 0,s ) ;
			}
		}
	}

	if( args.contains( "-u" ) ){

		auto volume = utility::cmdArgumentValue( args,"-d" ) ;

		for( const auto& it : mountinfo::unlockedVolumes().await() ){

			const auto& m = it.vInfo ;

			const auto& a = m.cipherPath() ;
			const auto& b = m.mountPoint();
			const auto& c = m.fileSystem() ;

			if( a == volume || b == volume ){

				const auto& engine = engines::instance().getByName( c ) ;

				if( siritask::encryptedFolderUnMount( { a,b,engine,5 } ).success() ){

					siritask::deleteMountFolder( b ) ;

					return 0 ;
				}else{
					break ;
				}
			}
		}

		return 1 ;
	}

	if( args.contains( "-p" ) ){

		for( const auto& it : mountinfo::unlockedVolumes().await() ){

			std::cout << it.vInfo.toString().toStdString() << std::endl ;
		}

		return 0 ;
	}

	return _print_err( 1,QObject::tr( "Unknown Option" ) ) ;
}

template< typename App,typename Function >
static int _run( App& app,Function function )
{
	app.setApplicationName( "SiriKali" ) ;

	miniSiriKali srk( std::move( function ) ) ;

	QMetaObject::invokeMethod( &srk,"start",Qt::QueuedConnection ) ;

	return app.exec() ;
}

int sirikali::run( const QStringList& args,int argc,char * argv[] )
{
	if( utility::containsAtleastOne( args,"-s","-u","-p","-T","-b" ) ){

		auto mm = utility::cmdArgumentValue( args,"-b" ) ;

		if( utility::equalsAtleastOne( mm,"internal","windows_dpapi","gnomewallet",
					       "libsecret","kwallet","osxkeychain" ) ){

			QApplication srk( argc,argv ) ;

			return _run( srk,[ & ]( secrets& ss ){

				return sirikali::unlockVolume( args,ss ) ;
			} ) ;
		}else{
			QCoreApplication srk( argc,argv ) ;

			return _run( srk,[ & ]( secrets& ss ){

				return _run( mm,args,ss ) ;
			} ) ;
		}
	}else{
		utility::initGlobals() ;

		QApplication srk( argc,argv ) ;

		srk.setApplicationName( "SiriKali" ) ;

		sirikali app( args ) ;

		QMetaObject::invokeMethod( &app,"start",Qt::QueuedConnection ) ;

		return srk.exec() ;
	}
}

void miniSiriKali::start()
{
	QCoreApplication::exit( m_function( m_secrets ) ) ;
}

void miniSiriKali::silenceWarning()
{
}

sirikali::sirikali( const QStringList& args ) :
	m_secrets( this ),
	m_mountInfo( this,true,[ & ](){ QCoreApplication::exit( m_exitStatus ) ; },_debug() ),
	m_checkUpdates( this,{ [ this ](){ this->disableAll() ; },[ this ](){ this->enableAll() ; } } ),
	m_configOptions( this,m_secrets,&m_language_menu,this->configOption() ),
	m_signalHandler( this ),
	m_argumentList( args )
{
	utility::miscOptions::instance().setMainQtWidget( this ) ;
}

configOptions::functions sirikali::configOption()
{
	auto a = [ this ](){

		this->enableAll() ;
		m_folderOpener = settings::instance().fileManager() ;
	} ;

	auto b = [ this ]( QAction * ac ){

		settings::instance().languageMenu( &m_language_menu,ac,m_translator ) ;

		m_ui->retranslateUi( this ) ;

		for( auto& it : m_actionPair ){

			it.first->setText( tr( it.second ) ) ;
		}

		for( auto& it : m_menuPair ){

			it.first->setTitle( tr( it.second ) ) ;
		}

		for( auto& it : m_language_menu.actions() ){

			it->setText( m_translator.translate( it->objectName() ) ) ;
		}
	} ;

	return { std::move( a ),std::move( b ) } ;
}

void sirikali::closeApplication( int s,const QString& e )
{
	m_secrets.close() ;

	if( m_ui && !m_emergencyShuttingDown ){

		auto m = processManager::get().enginesList() ;

		if( utility::platformIsWindows() ){

			if( m.size() > 0 ){

				auto m = tr( "Close All Volumes Before Quitting The Application" ) ;
				return DialogMsg( this ).ShowUIOK( tr( "WARNING" ),m ) ;
			}
		}else{
			if( m.size() > 0 ){

				QString s = m[ 0 ]->name() ;

				for( size_t i = 1 ; i < m.size() ; i++ ){

					s += "," + m[ i ]->name() ;
				}

				auto m = tr( "Close The Following File System(s) Before Quitting The Application\n\"%1\"" ) ;

				return DialogMsg( this ).ShowUIOK( tr( "WARNING" ),m.arg( s ) ) ;
			}
		}
	}

	utility::quitHelper() ;

	m_exitStatus = s ;

	if( !e.isEmpty() ){

		utility::debug::cerr() << e ;
	}

	if( m_ui ){

		m_debugWindow.Hide() ;
		this->hide() ;

		if( !m_emergencyShuttingDown ){

			utility::waitForOneSecond() ;
		}
	}

	m_mountInfo.stop() ;
}

void sirikali::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_signalHandler.setHandle( [ this ]( systemSignalHandler::signal s ){

		Q_UNUSED( s )

		this->emergencyShutDown() ;
	} ) ;

	if( utility::platformIsLinux() ){

		if( settings::instance().unMountVolumesOnLogout() ){

			m_signalHandler.addSignal( systemSignalHandler::signal::HUP ) ;
			m_signalHandler.addSignal( systemSignalHandler::signal::INT ) ;
			m_signalHandler.addSignal( systemSignalHandler::signal::QUIT ) ;
			m_signalHandler.addSignal( systemSignalHandler::signal::TERM ) ;
		}
	}

	m_ui = new Ui::sirikali ;
	m_ui->setupUi( this ) ;

	m_configOptions.translateUI() ;

	m_ui->pbcreate->setMinimumHeight( 31 ) ;
	m_ui->pbunlockvolume->setMinimumHeight( 31 ) ;
	m_ui->pbmenu->setMinimumHeight( 31 ) ;
	m_ui->pbupdate->setMinimumHeight( 31 ) ;
	m_ui->pbFavorites->setMinimumHeight( 31 ) ;

	auto table = m_ui->tableWidget ;

	const auto dimensions = settings::instance().getWindowDimensions() ;

	this->window()->setGeometry( dimensions.geometry() ) ;

	table->setColumnWidth( 0,dimensions.columnWidthAt( 0 ) ) ;
	table->setColumnWidth( 1,dimensions.columnWidthAt( 1 ) ) ;
	table->setColumnWidth( 2,dimensions.columnWidthAt( 2 ) ) ;
	table->setColumnWidth( 3,dimensions.columnWidthAt( 3 ) ) ;

	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidget->currentItem() ;

		if( item ){

			this->showContextMenu( item,true ) ;
		}
	} ) ;

	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT( slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;

	connect( m_ui->pbupdate,SIGNAL( clicked() ),
		 this,SLOT( pbUpdate() ) ) ;

	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	if( engines::instance().atLeastOneDealsWithFiles() ){

		auto m = new QMenu( this ) ;

		auto _addAction = [ & ]( const QString& a,const char * b,bool c ){

			auto ac = m->addAction( a ) ;
			m_actionPair.emplace_back( ac,b ) ;

			connect( ac,&QAction::triggered,[ this,c ](){

				this->unlockVolume( c ) ;
			} ) ;
		} ;

		_addAction( tr( "Mount Folder" ),"Mount Folder",true ) ;

		_addAction( tr( "Mount File" ),"Mount File",false ) ;

		m_ui->pbunlockvolume->setMenu( m ) ;
	}else{
		connect( m_ui->pbunlockvolume,&QPushButton::clicked,[ this ](){

			this->unlockVolume( true ) ;
		} ) ;
	}

	m_ui->pbcreate->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( createVolume( QAction * ) ) ) ;

		const auto& engines = engines::instance() ;

		for( const auto& it : engines.supportedEngines() ){

			const auto& name = it->uiName() ;

			auto ac = m->addAction( name ) ;

			ac->setObjectName( name ) ;

			if( it->isNotInstalled() ){

				ac->setEnabled( false ) ;

				if( name == "Ecryptfs" ){

					ac->setText( tr( "%1 Is Not Installed" ).arg( "Ecryptfs-simple" ) ) ;
				}else{
					ac->setText( tr( "%1 Is Not Installed" ).arg( name ) ) ;
				}
			}else{
				if( it->likeSsh() ){

					ac->setEnabled( true ) ;
				}else{
					ac->setEnabled( !it->createControlStructure().isEmpty() ) ;
				}
			}
		}

		return m ;
	}() ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( utility::getIcon( utility::iconType::general ) ) ;

	this->setUpAppMenu() ;

	this->setLocalizationLanguage( false ) ;

	m_trayIcon.setParent( this ) ;
	m_trayIcon.setIcon( utility::getIcon( utility::iconType::trayIcon ) ) ;

	this->showTrayIcon() ;

	if( utility::platformIsNOTWindows() && checkUpdates::hasNetworkSupport() ){

		QTimer::singleShot( settings::instance().checkForUpdateInterval(),this,SLOT( autoUpdateCheck() ) ) ;
	}

	if( utility::miscOptions::instance().debugEnabled() ){

		this->showDebugWindow() ;
	}

	auto& ss = settings::instance() ;

	auto e = ss.runCommandOnInterval() ;
	auto p = ss.runCommandOnIntervalTime() ;

	if( !e.isEmpty() ){

		auto s = new QTimer( this ) ;

		connect( s,&QTimer::timeout,[ this,e ](){

			this->runIntervalCustomCommand( e ) ;
		} ) ;

		s->start( p * 60 * 1000 ) ;

		this->runIntervalCustomCommand( e ) ;
	}

	this->updateFavoritesInContextMenu() ;

	this->startGUI( volume ) ;
}

void sirikali::showTrayIcon()
{
	if( QSystemTrayIcon::isSystemTrayAvailable() ){

		utility::debug() << "Showing System Tray Icon" ;

		m_trayIcon.show() ;
	}else{
		utility::Timer( 1000,[ this ]( int counter ){

			if( QSystemTrayIcon::isSystemTrayAvailable() ){

				utility::debug() << "Showing System Tray Icon" ;

				m_trayIcon.show() ;

				return true ;

			}else if( counter == 5 ){

				/*
				 * The tray doesn't seem to be ready yet but we cant wait any
				 * longer, just display it and hope for the best.
				 */

				utility::debug() << "TimeOut Waiting For System Tray to Become Available" ;

				m_trayIcon.show() ;

				return true ;
			}else{
				utility::debug() << "Waiting For System Tray To Become Available" ;

				return false ;
			}
		} ) ;
	}
}

void sirikali::setUpAppMenu()
{
	auto m = new QMenu( this ) ;

	m->setFont( this->font() ) ;

	auto _addAction = [ this ]( bool p,bool q,const QString& e,const char * z,const char * s ){

		auto ac = new QAction( e,this ) ;

		m_actionPair.emplace_back( ac,z ) ;

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

		m_menuPair.emplace_back( e,z ) ;

		e->setFont( this->font() ) ;

		if( t ){

			connect( e,SIGNAL( triggered( QAction * ) ),this,t ) ;
		}

		if( s ){

			connect( e,SIGNAL( aboutToShow() ),this,s ) ;
		}

		return e ;
	} ;

	if( utility::platformIsNOTWindows() ){

		auto ac = _addAction( false,false,tr( "Check For Updates" ),"Check For Updates",
				      SLOT( updateCheck() ) ) ;

		ac->setEnabled( checkUpdates::hasNetworkSupport() ) ;

		m->addAction( ac ) ;
	}
	m->addAction( _addAction( false,false,tr( "Unmount All" ),
				  "Unmount All",SLOT( unMountAll() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Unmount All And Quit" ),
				  "Unmount All And Quit",SLOT( unMountAllAndQuit() ) ) ) ;

	m_ui->pbFavorites->setMenu( _addMenu( new QMenu( this ),tr( "Favorites" ),"Favorites",
				    SLOT( favoriteClicked( QAction * ) ),
				    SLOT( showFavorites() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Settings" ),"Settings",
				  SLOT( configurationOptions() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "About" ),"About",SLOT( licenseInfo() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "FAQ" ),"FAQ",SLOT( FAQ() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Show/Hide" ),
				  "Show/Hide",SLOT( slotTrayClicked() ) ) ) ;

	m->addAction( _addAction( false,false,tr( "Quit" ),"Quit",SLOT( closeApplication() ) ) ) ;

	this->showFavorites() ;

	m_main_menu = m ;

	m_ui->pbmenu->setMenu( m_main_menu ) ;

	if( !settings::instance().showFavoritesInContextMenu() ){

		m_trayIcon.setContextMenu( m_main_menu ) ;
	}

	connect( &m_trayIcon,SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
		 this,SLOT( slotTrayClicked( QSystemTrayIcon::ActivationReason ) ) ) ;
}

void sirikali::configurationOptions()
{
	this->disableAll() ;
	m_configOptions.ShowUI() ;
}

void sirikali::FAQ()
{
	this->disableAll() ;
	help::instance( this,m_folderOpener,[ this ](){ this->enableAll() ; } ) ;
}

void sirikali::licenseInfo()
{
	utility::licenseInfo( this ) ;
}

void sirikali::updateCheck()
{
	m_checkUpdates.run( false ) ;
}

void sirikali::autoUpdateCheck()
{
	m_checkUpdates.run( true ) ;
}

void sirikali::favoriteClicked()
{
	this->disableAll() ;

	favorites2::instance( this,m_secrets,[ this ](){

		this->enableAll() ;

		this->updateFavoritesInContextMenu() ;
	} ) ;
}

void sirikali::mountAll()
{
	mountinfo::unlockedVolumes().then( [ this ]( const mountinfo::List& m ){

		this->updateVolumeList( m,false ) ;

		this->autoUnlockVolumes( m,true ) ;
	} ) ;
}

void sirikali::mountFavorite( const QString& e )
{
	auto _found = [ & ]( const favorites::entry& s,
			     const QString& mountPointPath,
			     const QString& volumePath ){

		if( mountPointPath.isEmpty() ){

			return s.volumePath == volumePath ;
		}else{
			return s.volumePath == volumePath && s.mountPointPath == mountPointPath ;
		}
	} ;

	QString volumePath ;
	QString mountPointPath ;

	utility2::stringListToStrings( utility::split( e ),volumePath,mountPointPath ) ;

	favorites::instance().entries( [ & ]( const favorites::entry& e ){

		if( _found( e,mountPointPath,volumePath ) ){

			this->autoUnlockAutoMount( e,true ) ;

			return true ;
		}else{
			return false ;
		}
	} ) ;
}

void sirikali::favoriteClicked( QAction * ac )
{
	auto e = ac->objectName() ;

	if( e == "Manage Favorites" ){

		this->disableAll() ;

		favorites2::instance( this,m_secrets,[ this ](){

			this->enableAll() ;

			this->updateFavoritesInContextMenu() ;
		} ) ;
	}else{
		if( e == "Mount All" ){

			this->mountAll() ;
		}else{
			this->mountFavorite( e ) ;
		}
	}

	this->updateFavoritesInContextMenu() ;
}

void sirikali::showFavorites()
{
	settings::instance().readFavorites( m_ui->pbFavorites->menu() ) ;
}

void sirikali::setLocalizationLanguage( bool translate )
{
	settings::instance().setLocalizationLanguage( translate,&m_language_menu,m_translator ) ;
}

void sirikali::startGUI( const QString& volume )
{
	auto m = settings::instance().autoMountFavoritesOnStartUp() ;

	if( m_startHidden ){

		this->startGUI( volume,m ) ;
	}else{
		this->disableAll() ;

		this->showMainWindow() ;

		auto s = settings::instance().delayBeforeAutoMountAtStartup() ;

		utility::Timer( s * 1000,[ this,volume,m ](){

			this->startGUI( volume,m ) ;
		} ) ;
	}
}

void sirikali::startGUI( const QString& volume,bool autoMountAtStartUp )
{
	mountinfo::unlockedVolumes().then( [ this,autoMountAtStartUp,volume ]( mountinfo::List m ){

		this->updateVolumeList( m,false ) ;

		if( autoMountAtStartUp ){

			this->autoUnlockVolumes( m ) ;
		}

		this->autoMount( volume ) ;

		utility::applicationStarted() ;
	} ) ;
}

void sirikali::createbackendwindow()
{
	createBackendWIndow::instance( this ) ;
}

void sirikali::showMainWindow()
{
	this->setVisible( true ) ;
	this->raise() ;
	this->show() ;
	this->setWindowState( Qt::WindowActive ) ;
}

void sirikali::raiseWindow( const QString& volume )
{
	this->showMainWindow() ;
	this->autoMount( volume ) ;
}

void sirikali::start()
{
	m_startHidden  = m_argumentList.contains( "-e" ) ;

	if( !m_startHidden ){

		m_startHidden = settings::instance().startMinimized() ;
	}

	m_folderOpener = utility::cmdArgumentValue( m_argumentList,"-m",settings::instance().fileManager() ) ;

	auto s = utility::socketPath() ;

	if( utility::platformIsNOTWindows() ){

		utility::createFolder( s.folderPath ) ;

		if( !utility::pathIsWritable( s.folderPath ) ){

			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "\"%1\" Folder Must Be Writable" ).arg( s.folderPath ) ) ;

			return this->closeApplication() ;
		}
	}

	oneinstance::callbacks cb = {

		[ this ]( const QString& e ){ this->setUpApp( e ) ; },
		[ this ](){ this->closeApplication( 1 ) ; },
		[ this ]( const QString& e ){ this->raiseWindow( e ) ; },
	} ;

	auto x = utility::cmdArgumentValue( m_argumentList,"-d" ) ;

	oneinstance::instance( this,s.socketFullPath,x,std::move( cb ) ) ;
}

struct unlockVolumeStruct{
	QString vol ;
	QString volume ;
	QString mountPath ;
	QString backEnd ;
	QString idleTime ;
	QString cPath ;
	QString keyFile ;
	QString mOpt ;
	bool reverse ;
	bool mode ;
};

static int _unlockVolume( const unlockVolumeStruct& s,const QByteArray& key ){

	auto m = [ & ](){

		if( s.mountPath.isEmpty() ){

			auto e = utility::mountPathPostFix( s.volume.split( "/" ).last() ) ;

			return settings::instance().mountPath( e ) ;
		}else{
			return s.mountPath ;
		}
	}() ;

	engines::engine::booleanOptions mmm ;

	mmm.unlockInReverseMode = s.reverse ;
	mmm.unlockInReadOnly    = s.mode ;

	QString q ;

	engines::engine::mountGUIOptions::mountOptions mm( s.idleTime,s.cPath,s.mOpt,q,q,q,mmm ) ;

	engines::engine::cmdArgsList ss{ s.volume,m,key,mm } ;

	siritask::mount sss{ ss,false,{ ss.cipherFolder,ss.configFilePath } } ;

	if( utility::platformIsWindows() || sss.engine->runsInForeGround() ){

		QString a = "This Action Is Not Supported On Windows Or With" ;
		QString b = " Backends That Do Not Run In Background" ;

		return _print_err( 1,a + b ) ;
	}

	auto e = siritask::encryptedFolderMount( sss ) ;

	if( e == engines::engine::status::success ){

		return 0 ;
	}else{
		return _print_err( 1,e.toString() ) ;
	}
}

static secrets::wallet::walletKey _walletKey( const unlockVolumeStruct& s,secrets& secrets )
{
	using wxt = LXQt::Wallet::BackEnd ;

	auto _supported = [ & ]( wxt e,const char * m ){

		return s.backEnd == m && LXQt::Wallet::backEndIsSupported( e ) ;
	} ;

	if( _supported( wxt::internal,"internal" ) ){

		return secrets.walletBk( wxt::internal ).getKey( s.volume ) ;

	}else if( _supported( wxt::libsecret,"gnomewallet" ) ||
		  _supported( wxt::libsecret,"libsecret" ) ){

		return secrets.walletBk( wxt::libsecret ).getKey( s.volume ) ;

	}else if( _supported( wxt::kwallet,"kwallet" ) ){

		return secrets.walletBk( wxt::kwallet ).getKey( s.volume ) ;

	}else if( _supported( wxt::osxkeychain,"osxkeychain" ) ){

		return secrets.walletBk( wxt::osxkeychain ).getKey( s.volume ) ;

	}else if( _supported( wxt::windows_dpapi,"windows_dpapi" ) ){

		return secrets.walletBk( LXQt::Wallet::BackEnd::windows_dpapi ).getKey( s.volume ) ;
	}else{
		return secrets::wallet::walletKey{ false,true,"" } ;
	}
}

int sirikali::unlockVolume( const QStringList& l,secrets& secrets )
{
	unlockVolumeStruct s ;

	s.vol       = utility::cmdArgumentValue( l,"-d" ) ;
	s.volume    = QDir( s.vol ).canonicalPath() ;
	s.mountPath = utility::cmdArgumentValue( l,"-z" ) ;
	s.backEnd   = utility::cmdArgumentValue( l,"-b" ) ;
	s.mode      = utility::cmdArgumentValue( l,"-k","rw" ) == "ro" ;
	s.idleTime  = utility::cmdArgumentValue( l,"-i" ) ;
	s.cPath     = utility::cmdArgumentValue( l,"-c" ) ;
	s.keyFile   = utility::cmdArgumentValue( l,"-f" ) ;
	s.mOpt      = utility::cmdArgumentValue( l,"-o" ) ;
	s.reverse   = l.contains( "-r" ) ;

	if( s.vol.isEmpty() ){

		return _print_err( 1,tr( "ERROR: Volume Path Not Given." ) ) ;
	}else{
		if( s.backEnd == "stdin" ){

			return _unlockVolume( s,[ & ](){

				auto e = utility::readPassword() ;

				if( s.keyFile.isEmpty() ){

					return e ;
				}else{
					return crypto::hmac_key( s.keyFile,e ) ;
				}
			}() ) ;
		}

		if( s.backEnd == "keyfile" ){

			auto key = utility::fileContents( s.keyFile ) ;

			if( utility::containsAtleastOne( key,'\n','\0','\r' ) ){

				_print_err( 1,keyDialog::keyFileError() ) ;
			}

			return _unlockVolume( s,key ) ;
		}

		auto w = _walletKey( s,secrets ) ;

		if( w.opened ){

			if( w.key.isEmpty() ){

				return _print_err( 1,tr( "ERROR: Key Not Found In The Backend." ) ) ;
			}else{
				return _unlockVolume( s,w.key.toUtf8() ) ;
			}
		}else{
			return _print_err( 1,tr( "ERROR: Failed To Unlock Requested Backend." ) ) ;
		}
	}
}

void sirikali::mountMultipleVolumes( keyDialog::volumeList e )
{
	if( !e.empty() ){

		m_allowEnableAll.setFalse() ;

		this->disableAll() ;

		auto done = [ this ](){

			this->setAcceptDrops( true ) ;
			m_allowEnableAll.setTrue() ;
			this->enableAll() ;
		} ;

		this->setAcceptDrops( false ) ;

		keyDialog::instance( this,
				     m_secrets,
				     m_autoOpenFolderOnMount,
				     m_folderOpener,
				     std::move( e ),
				     std::move( done ),
				     [ this ](){ this->updateList() ; } ) ;
	}else{
		m_allowEnableAll.setTrue() ;
		this->enableAll() ;
	}
}

void sirikali::autoMountFavoritesOnAvailable( QString m )
{
	if( settings::instance().autoMountFavoritesOnAvailable() ){

		favorites::instance().entries( [ & ]( const favorites::entry& s ){

			if( s.volumePath.startsWith( m ) && s.autoMount ){

				this->autoUnlockAutoMount( s,false,true ) ;

				return true ;
			}else{
				return false ;
			}
		} ) ;
	}
}

void sirikali::autoUnlockVolumes( const mountinfo::List& s,bool autoSetAutoMount )
{
	favorites::volumeList e ;

	auto _not_mounted = [ & ]( const favorites::entry& e ){

		for( const auto& it : s ){

			const auto& m = it.vInfo ;

			if( it.engine.setsCipherPath() ){

				if( m.cipherPath() == e.volumePath ){

					return false ;
				}

			}else if( m.mountPoint() == e.mountPointPath ){

				return false ;
			}
		}

		return true ;
	} ;

	favorites::instance().entries( [ & ]( const favorites::entry& m ){

		if( autoSetAutoMount ){

			if( _not_mounted( m ) ){

				e.emplace_back( m ) ;
			}
		}else{
			if( m.autoMount && _not_mounted( m ) ){

				e.emplace_back( m ) ;
			}
		}
	} ) ;

	this->autoUnlockAutoMount( std::move( e ),false,false,autoSetAutoMount ) ;
}

void sirikali::autoMount( keyDialog::volumeList& q,
			  keyDialog::entry&& ee,
			  bool showMountDialog,
			  bool autoOpenFolderOnMount )
{
	if( showMountDialog ){

		q.emplace_back( std::move( ee ) ) ;
	}else{
		this->disableAll() ;

		const favorites::volEntry& e = ee.volEntry ;

		engines::engine::cmdArgsList aa( e.favorite(),e.password() ) ;

		if( aa.mountPoint.isEmpty() ){

			aa.mountPoint = keyDialog::mountPointPath( ee.engine.get(),
								   e.favorite().volumePath,
								   e.favorite().mountPointPath,
								   settings::instance(),
								   false ) ;
		}

		auto s = siritask::encryptedFolderMount( { aa,false,ee.engine } ) ;

		if( s == engines::engine::status::success ){

			if( autoOpenFolderOnMount ){

				this->openMountPointPath( e.favorite().mountPointPath ) ;
			}

			if( !s.engine().autorefreshOnMountUnMount() ){

				this->updateList() ;
			}
		}else{
			q.emplace_back( std::move( ee ) ) ;

			utility::debug() << "Automounting has failed: " + s.toString() ;
		}

		this->enableAll() ;
	}
}

keyDialog::volumeList sirikali::autoMount( keyDialog::volumeList l,bool autoOpenFolderOnMount )
{
	keyDialog::volumeList e ;

	auto s = settings::instance().showMountDialogWhenAutoMounting() ;

	for( auto&& it : l ){

		const auto& m = it.volEntry.favorite() ;

		bool u = it.engine->known() && it.engine->requiresNoPassword() ;

		if( u || m.volumeNeedNoPassword || !m.password.isEmpty() ){

			this->autoMount( e,std::move( it ),s,autoOpenFolderOnMount ) ;
		}else{
			e.emplace_back( std::move( it ) ) ;
		}
	}

	return e ;
}

QByteArray static _get_key( const keyDialog::entry& it,QString& debug,secrets::wallet& m )
{
	const auto& volumePath = it.volEntry.favorite().volumePath ;

	const auto& name = it.engine->uiName() ;

	debug += "\n3. Trying to get password from wallet for path: " + volumePath ;

	if( volumePath.startsWith( name + " ",Qt::CaseInsensitive ) ){

		auto ee = m->readValue( volumePath ) ;

		if( ee.isEmpty() ){

			auto ss = name.toLower() + " " + it.engine.cipherFolder() ;

			debug += "\n4. Retrying to get password from wallet for path: " + ss ;

			ee = m->readValue( ss ) ;
		}

		return ee ;
	}else{
		return m->readValue( volumePath ) ;
	}
}

keyDialog::volumeList sirikali::autoUnlockVolumes( favorites::volumeList ss,
						   bool autoOpenFolderOnMount,
						   bool skipUnknown,
						   bool autoSetAutoMount )
{
	const auto& mm = utility::miscOptions::instance() ;

	auto l = keyDialog::fromFavoritesList( std::move( ss ),mm.starting(),skipUnknown ) ;

	if( l.empty() ){

		return l ;
	}

	auto ee = settings::instance().autoMountBackEnd() ;

	auto m = [ & ](){

		if( ee.isInvalid() ){

			utility::debug() << "Warning, Wallet Not Set" ;

			return secrets::wallet() ;
		}else{
			auto m = m_secrets.walletBk( ee.bk() ) ;

			if( !m ){

				utility::debug() << "Warning, Failed To Set Selected Wallet" ;
				return secrets::wallet() ;
			}

			if( m.open() ){

				return m ;
			}else{
				utility::debug() << "Warning, Failed To Open Selected Wallet" ;
				return secrets::wallet() ;
			}
		}
	}() ;

	keyDialog::volumeList e ;

	auto s = settings::instance().showMountDialogWhenAutoMounting() ;

	m_allowEnableAll.setFalse() ;

	auto disableAllRaii = utility2::make_raii( [ this ](){ m_allowEnableAll.setTrue() ; } ) ;

	QString debug ;

	for( auto&& it : l ){

		debug = "1. Processing favorite entry: " + it.volEntry.favorite().volumePath ;

		if( autoSetAutoMount && it.volEntry.favorite().autoMount.False() ){

			it.volEntry.setAutoMount( true ) ;
		}

		if( !it.volEntry.password().isEmpty() ){

			debug += "\n2. Favorite entry has password already" ;

		}else if( it.engine->requiresNoPassword() || it.volEntry.favorite().volumeNeedNoPassword ){

			debug += "\n2. Engine requires no password or favorite need no password" ;

		}else if( m ){

			debug += "\n2. Trying to get volume password from wallet" ;

			auto uu = _get_key( it,debug,m ) ;

			if( uu.isEmpty() ){

				debug += "\n5. Volume has no entry in the wallet" ;

				utility::debug() << debug ;

				e.emplace_back( std::move( it ) ) ;

				continue ;
			}else{
				it.volEntry.setPassword( std::move( uu ) ) ;

				debug += "\n5. Volume password obtained from wallet" ;
			}
		}else{
			debug += "\n2. Failed to obtain password from wallet" ;

			utility::debug() << debug ;

			e.emplace_back( std::move( it ) ) ;

			continue ;
		}

		if( it.engine->takesTooLongToUnlock() ){

			debug += "\n6. Showing mount dialog window" ;
			debug += " because the backend takes too long to unlock" ;

			utility::debug() << debug ;

			e.emplace_back( std::move( it ) ) ;
		}else{
			utility::debug() << debug ;

			this->autoMount( e,std::move( it ),s,autoOpenFolderOnMount ) ;
		}
	}

	return e ;
}

void sirikali::volumeProperties()
{
	auto table = m_ui->tableWidget ;

	auto row = table->currentRow() ;

	QString cipherPath ;
	QString mountPath ;
	QString volumeType ;

	utility2::stringListToStrings( tablewidget::rowEntries( table,row ),
				       cipherPath,
				       mountPath,
				       volumeType ) ;

	const auto& engine = engines::instance().getByName( volumeType ) ;

	auto s = engine.volumeProperties( cipherPath,mountPath ).await() ;

	if( s.isEmpty() ){

		this->genericVolumeProperties() ;
	}else {
		return DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,s ) ;
	}
}

void sirikali::genericVolumeProperties()
{
	this->disableAll() ;

	auto table = m_ui->tableWidget ;

	auto row = table->currentRow() ;

	const auto& engine = engines::instance().getByName( table->item( row,2 )->text() ) ;

	if( utility::platformIsWindows() || engine.runsInForeGround() ){

		auto m = processManager::get().volumeProperties( table->item( row,1 )->text() ) ;

		if( m.isEmpty() ){

			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Read Volume Properties" ) ) ;
		}else{
			DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,m ) ;
		}

		return this->enableAll() ;
	}

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

		if( it.vInfo.mountPoint() == s ){

			DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,[ & ](){

				auto s = it.vInfo.mountOptions() ;

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

void sirikali::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	m.setFont( this->font() ) ;

	auto _addAction = [ & ]( const auto& txt,const char * slot ){

		auto ac = m.addAction( txt ) ;

		connect( ac,SIGNAL( triggered() ),this,slot ) ;
	} ;

	//_addAction( tr( "Open Parent Folder" ),SLOT( slotOpenParentFolder() ) ) ;

	_addAction( tr( "Open Folder" ),SLOT( slotOpenFolder() ) ) ;

	_addAction( tr( "Unmount" ),SLOT( pbUmount() ) ) ;

	_addAction( tr( "Properties" ),SLOT( volumeProperties() ) ) ;

	_addAction( tr( "Add To Favorites" ),SLOT( addToFavorites() ) ) ;

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

void sirikali::addToFavorites()
{
	auto table = m_ui->tableWidget ;

	int s = table->currentRow() ;

	if( s == -1 ){

		return ;
	}

	auto cp = tablewidget::rowEntries( table,s ) ;

	if( cp.size() > 0 ){

		const auto& engine = engines::instance().getByName( cp.at( 2 ) ) ;

		bool m = engine.known() && engine.usesOnlyMountPoint() ;

		favorites2::instance( this,m_secrets,[ this ](){

			this->updateFavoritesInContextMenu() ;

		},engine,m ? cp.at( 1 ) : cp.at( 0 ) ) ;
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

		if( utility::platformIsWindows() ){

			path = path.replace( "/","\\" ) ;
		}

		this->openMountPoint( path ) ;
	}
}

void sirikali::slotOpenParentFolder()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto item = table->currentItem() ;
		auto path = table->item( item->row(),1 )->text() ;

		if( utility::platformIsWindows() ){

			path = path.replace( "/","\\" ) ;
		}

		this->openMountPoint( utility::removeLastPathComponent( path ) ) ;
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
	if( settings::instance().autoOpenFolderOnMount() ){

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

	this->addAction( _addAction( { Qt::Key( Qt::CTRL ) + Qt::Key::Key_F },SLOT( favoriteClicked() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_U },SLOT( pbUmount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_R },SLOT( pbUpdate() ) ) ) ;

	this->addAction( _addAction( { Qt::Key( Qt::CTRL ) + Qt::Key::Key_W },SLOT( hideWindow() ) ) ) ;

	this->addAction( _addAction( { Qt::Key( Qt::CTRL ) + Qt::Key::Key_B },SLOT( createbackendwindow() ) ) ) ;

	this->addAction( [ & ](){

		auto e = _addAction( { Qt::Key( Qt::CTRL ) + Qt::Key::Key_Q },SLOT( closeApplication() ) ) ;

		e->setMenuRole( QAction::QuitRole ) ;

		e->setText( tr( "&Quit" ) ) ;

		return e ;
	}() ) ;

	this->addAction( _addAction( { Qt::Key( Qt::CTRL ) + Qt::Key::Key_D },SLOT( showDebugWindow() ) ) ) ;
}

void sirikali::showDebugWindow()
{
	if( !m_debugWindow.isVisible() ){

		m_debugWindow.Show() ;
	}
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
			this->raise() ;
			this->setWindowState( Qt::WindowActive ) ;
		}
	}
}

void sirikali::dragEnterEvent( QDragEnterEvent * e )
{
	e->accept() ;
}

template< typename Function >
static void _folder_entry( const QString& path,
			   favorites::volumeList& volumeList,
			   const favorites& favorites,
			   const Function& function )
{
	favorites.entries( [ & ]( const favorites::entry& e ){

		if( QDir( e.volumePath ).absolutePath() == QDir( path ).absolutePath() ){

			if( e.password.isEmpty() ){

				volumeList.emplace_back( e,function( path ) ) ;
			}else{
				volumeList.emplace_back( e ) ;
			}

			return true ;
		}else{
			return false ;
		}

	},[ & ](){ volumeList.emplace_back( path,function( path ) ) ; } ) ;
}

template< typename Function >
static void _file_entry( const QString& path,
			 favorites::volumeList& volumeList,
			 const favorites& favorites,
			 const Function& function )
{
	if( path.endsWith( ".json" ) ){

		auto ss = favorites.readFavoriteByPath( path ) ;

		if( ss.has_value() ){

			const auto& sss = ss.value() ;

			if( sss.password.isEmpty() ){

				volumeList.emplace_back( sss,function( sss.volumePath ) ) ;
			}else{
				volumeList.emplace_back( sss ) ;
			}
		}else{
			auto bb = favorites.readFavoriteByFileSystemPath( path ) ;

			if( bb.has_value() ){

				volumeList.emplace_back( bb.RValue() ) ;
			}else{
				utility::debug() << "Malformed SiriKali config file: " + path ;
			}
		}
	}else{
		volumeList.emplace_back( path ) ;
	}
}

template< typename Function >
static favorites::volumeList _events( const QStringList& paths,
				      const favorites& favorites,
				      const Function& function )
{
	favorites::volumeList volumeList ;

	QFileInfo fileInfo ;

	for( const auto& it : paths ){

		fileInfo.setFile( it ) ;

		if( fileInfo.isDir() ){

			_folder_entry( it,volumeList,favorites,function ) ;

		}else if( fileInfo.isFile() ){

			_file_entry( it,volumeList,favorites,function ) ;
		}
	}

	return volumeList ;
}

void sirikali::dropEvent( QDropEvent * e )
{
	auto& ff = favorites::instance() ;

	auto m = settings::instance().autoMountBackEnd() ;

	QStringList list ;

	for( const auto& it : e->mimeData()->urls() ){

		auto m = it.path() ;

		if( utility::platformIsWindows() ){

			while( true ){

				if( m.startsWith( "/" ) ){

					m = m.remove( 0,1 ) ;
				}else{
					break ;
				}
			}
		}

		list.append( m ) ;
	}

	if( m.isValid() ){

		auto wallet = m_secrets.walletBk( m.bk() ) ;

		if( wallet.open() ){

			auto l = _events( list,ff,[ & ]( const QString& e ){

				return wallet->readValue( e ) ;
			} ) ;

			return this->autoUnlockAutoMount( std::move( l ),false ) ;
		}
	}

	auto aa = _events( list,ff,[ & ]( const QString& e ){

		Q_UNUSED( e )
		return QByteArray() ;
	} ) ;

	this->autoUnlockAutoMount( std::move( aa ),false ) ;
}

void sirikali::createVolume( QAction * ac )
{
	if( ac ){

		this->disableAll() ;

		auto s = ac->objectName() ;

		const auto& engine = engines::instance().getByName( s ) ;

		if( engine.likeSsh() ){

			favorites2::instance( this,m_secrets,[ this ](){

				this->enableAll() ;

				this->updateFavoritesInContextMenu() ;
			},engine ) ;
		}else{
			keyDialog::instance( this,
					     m_secrets,
					     [ this ](){ this->enableAll() ; },
					     [ this ](){ this->updateList() ; },
					     m_autoOpenFolderOnMount,
					     m_folderOpener,
					     s ) ;
		}
	}
}

void sirikali::autoMount( const QString& vv )
{
	auto volume = vv ;

	while( volume.endsWith( '/' ) ) {

		volume = utility::removeLast( volume,1 ) ;
	}

	if( volume.isEmpty() ){

		return this->enableAll() ;
	}

	auto& favorites = favorites::instance() ;

	auto s = settings::instance().autoOpenFolderOnMount() ;

	if( volume.endsWith( ".json" ) ){

		auto a = favorites.readFavoriteByFileSystemPath( volume ) ;

		if( a.has_value() ){

			return this->autoUnlockAutoMount( a.RValue(),s ) ;
		}
	}

	favorites.entries( [ & ]( const favorites::entry& e ){

		if( e.volumePath == volume ){

			this->autoUnlockAutoMount( e,s ) ;

			return true ;
		}else{
			return false ;
		}

	},[ & ]{ this->autoUnlockAutoMount( volume,s ) ; } ) ;
}

void sirikali::unlockVolume( bool dir )
{
	this->disableAll() ;

	QString path ;

	if( dir ){

		auto e = tr( "Select An Encrypted Volume Directory" ) ;
		path = utility::getExistingDirectory( this,e,settings::instance().homePath() ) ;
	}else{
		auto e = tr( "Select A File To Be Mounted" ) ;
		path = utility::getExistingFile( this,e,settings::instance().homePath() ) ;
	}

	if( path.isEmpty() ){

		this->enableAll() ;
	}else{
		this->autoMount( path ) ;
	}
}

QFont sirikali::getSystemVolumeFont()
{
	auto f = this->font() ;
	f.setItalic( !f.italic() ) ;
	f.setBold( !f.bold() ) ;
	return f ;
}

class mountedVolumes{
public:
	mountedVolumes( QTableWidget * table ) :
		m_cipherPath( tablewidget::columnEntries( table,0 ) ),
		m_mountPath( tablewidget::columnEntries( table,1 ) )
	{
	}
	int row( const QString& e )
	{
		auto a = utility::split( e,'\n' ) ;

		if( a.size() == 1 ){

			const auto& m = a.at( 0 ) ;

			for( int i = 0 ; i < m_cipherPath.size() ; i++ ){

				if( m == m_cipherPath.at( i ) ){

					return i ;
				}
			}
		}else{
			const auto& x = a.at( 0 ) ;
			const auto& y = a.at( 1 ) ;

			for( int i = 0 ; i < m_cipherPath.size() ; i++ ){

				if( x == m_cipherPath.at( i ) && y == m_mountPath.at( i ) ){

					return i ;
				}
			}
		}

		return -1 ;
	}

	bool mounted( const QString& e )
	{
		auto s = this->row( e ) ;

		if( s == -1 ){

			return false ;
		}else{
			m_cipherPath.removeAt( s ) ;
			m_mountPath.removeAt( s ) ;

			return true ;
		}
	}

	void addMounted( QMenu * m,bool e )
	{
		for( int i = 0 ; i < m_cipherPath.size() ; i++ ){

			auto ac = new QAction( m ) ;

			QString n ;

			if( e ){

				n = m_cipherPath.at( i ) + "\n" + m_mountPath.at( i ) ;
			}else{
				n = m_cipherPath.at( i ) ;
			}

			ac->setText( n ) ;
			ac->setObjectName( n ) ;
			ac->setCheckable( true ) ;
			ac->setChecked( true ) ;

			m->addAction( ac ) ;
		}
	}
private:
	QStringList m_cipherPath ;
	QStringList m_mountPath ;
};

void sirikali::updateFavoritesInContextMenu()
{
	if( !settings::instance().showFavoritesInContextMenu() ){

		return ;
	}

	if( !m_context_menu ){

		m_context_menu = new QMenu( this ) ;

		connect( m_context_menu,&QMenu::triggered,[ this ]( QAction * ac ){

			if( ac->isCheckable() ){

				if( ac->isChecked() ){

					this->favoriteClicked( ac ) ;
				}else{
					auto b = mountedVolumes( m_ui->tableWidget ).row( ac->objectName() ) ;

					if( b != -1 ){

						tablewidget::selectRow( m_ui->tableWidget,b ) ;
						this->pbUmount() ;
					}
				}
			}else{
				this->favoriteClicked( ac ) ;
			}
		} ) ;

		m_trayIcon.setContextMenu( m_context_menu ) ;
	}

	auto e = settings::instance().readFavorites( m_context_menu ) ;

	auto s = m_context_menu->actions() ;

	mountedVolumes m( m_ui->tableWidget ) ;

	for( int i = 2 ; i < s.size() ; i++ ){

		auto it = s.at( i ) ;
		it->setCheckable( true ) ;
		it->setChecked( m.mounted( it->objectName() ) ) ;
	}

	m.addMounted( m_context_menu,e ) ;

	m_context_menu->addSeparator() ;

	m_context_menu->addActions( m_main_menu->actions() ) ;
}

void sirikali::runIntervalCustomCommand( const QString& cmd )
{
	this->processMountedVolumes( m_ui->tableWidget,[ & ]( const sirikali::mountedEntry& s ){

		QStringList args{ s.cipherPath,s.mountPoint,s.volumeType } ;

		QString key ;

		auto& settings = settings::instance() ;

		if( settings.allowExternalToolsToReadPasswords() ){

			auto bk = settings.autoMountBackEnd() ;

			if( bk.isValid() ){

				key = m_secrets.walletBk( bk.bk() ).getKey( s.cipherPath ).key ;
			}
		}

		Task::exec( [ = ](){

			const auto& e = utility::systemEnvironment() ;

			utility::logger logger ;

			logger.showText( cmd,args ) ;

			auto r = [ & ](){

				if( key.isEmpty() ){

					return Task::process::run( cmd,args,-1,{},e ).get() ;
				}else{
					auto s = e ;

					s.insert( settings::instance().environmentalVariableVolumeKey(),key ) ;

					return Task::process::run( cmd,args,-1,{},s ).get() ;
				}
			}() ;

			logger.showText( r ) ;
		} ) ;
	} ) ;
}

void sirikali::addEntryToTable( const QStringList& l )
{
	tablewidget::addRow( m_ui->tableWidget,l ) ;
}

void sirikali::addEntryToTable( const volumeInfo& e )
{
	this->addEntryToTable( e.toStringList() ) ;
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

void sirikali::updateList( const volumeInfo& e )
{
	if( e.isValid() ){

		auto table = m_ui->tableWidget ;

		auto row = tablewidget::columnHasEntry( table,e.mountPoint(),1 ) ;

		if( row == -1 ){

			row = tablewidget::addRow( table ) ;
		}

		tablewidget::updateRow( table,e.toStringList(),row,this->font() ) ;

		tablewidget::selectRow( table,row ) ;
	}
}

engines::engine::cmdStatus sirikali::unMountVolume( const sirikali::mountedEntry& e,bool emergencyShutDown )
{
	const auto& engine = engines::instance().getByName( e.volumeType ) ;

	auto s = siritask::encryptedFolderUnMount( { e.cipherPath,e.mountPoint,engine,5 } ) ;

	if( s.success() && engine.backendRequireMountPath() ){

		if( emergencyShutDown ){

			siritask::deleteMountFolder( e.mountPoint ) ;
		}else{
			utility::Timer( 1000,[ s = e.mountPoint ](){

				siritask::deleteMountFolder( s ) ;
			} ) ;
		}
	}

	return s ;
}

void sirikali::pbUmount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		m_mountInfo.announceEvents( false ) ;

		this->disableAll() ;

		auto row = table->currentRow() ;

		auto type = table->item( row,2 )->text() ;

		auto a = table->item( row,0 )->text() ;
		auto b = table->item( row,1 )->text() ;
		auto c = table->item( row,2 )->text() ;

		utility::waitForOneSecond() ;

		auto s = this->unMountVolume( { a,b,c } ) ;

		if( s.success() ){

			this->updateList() ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),s.toString() ) ;

			if( s == engines::engine::status::failedToUnMount ){

				this->enableAll() ;
			}else{
				this->pbUpdate() ;
			}
		}

		m_mountInfo.announceEvents( true ) ;
	}
}

void sirikali::emergencyShutDown()
{
	m_emergencyShuttingDown = true ;

	this->hide() ;

	m_mountInfo.announceEvents( false ) ;

	this->processMountedVolumes( m_ui->tableWidget,[ this ]( const sirikali::mountedEntry& e ){

		this->unMountVolume( e,true ) ;
	} ) ;

	this->closeApplication( 0,"Emergency shut down" ) ;
}

void sirikali::unMountAll()
{
	m_mountInfo.announceEvents( false ) ;
	m_allowEnableAll.setFalse() ;

	this->disableAll() ;

	utility::waitForOneSecond() ;

	this->processMountedVolumes( m_ui->tableWidget,[ this ]( const mountedEntry& e ){

		if( this->unMountVolume( e ).success() ){

			tablewidget::deleteRow( m_ui->tableWidget,e.mountPoint,1 ) ;

			utility::waitForOneSecond() ;
		}
	} ) ;

	auto m = m_ui->tableWidget->rowCount() ;

	if( m > 0 ){

		if( m == 1 ){

			DialogMsg( this ).ShowUIOK( tr( "WARNING" ),tr( "Failed To Unmount 1 Volume." ) ) ;
		}else{
			DialogMsg( this ).ShowUIOK( tr( "WARNING" ),tr( "Failed To Unmount %1 Volumes." ).arg( m ) ) ;
		}
	}

	m_allowEnableAll.setTrue() ;

	this->updateList() ;

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

void sirikali::updateList()
{
	if( utility::platformIsWindows() ){

		this->updateVolumeList( mountinfo::unlockedVolumes().get() ) ;
	}else{
		this->updateVolumeList( mountinfo::unlockedVolumes().await() ) ;
	}

	this->updateFavoritesInContextMenu() ;
}

void sirikali::pbUpdate()
{
	this->disableAll() ;

	this->updateList() ;
}

void sirikali::updateVolumeList( const mountinfo::List& r,bool enableAll )
{
	tablewidget::clearTable( m_ui->tableWidget ) ;

	for( const auto& it : r ){

		this->updateList( it.vInfo ) ;
	}

	if( enableAll ){

		this->enableAll() ;
	}
}

void sirikali::slotCurrentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void sirikali::disableAll()
{
	m_ui->pbmenu->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbunlockvolume->setEnabled( false ) ;
	m_ui->pbcreate->setEnabled( false ) ;
	m_ui->pbFavorites->setEnabled( false ) ;

	if( !utility::miscOptions::instance().starting() ){

		m_ui->pbupdate->setEnabled( false ) ;
	}
}

void sirikali::enableAll()
{
	if( m_allowEnableAll ){

		m_ui->pbmenu->setEnabled( true ) ;
		m_ui->pbupdate->setEnabled( true ) ;
		m_ui->tableWidget->setEnabled( true ) ;
		m_ui->tableWidget->setFocus() ;
		m_ui->pbunlockvolume->setEnabled( true ) ;
		m_ui->pbcreate->setEnabled( true ) ;
		m_ui->pbFavorites->setEnabled( true ) ;
	}
}

sirikali::~sirikali()
{
	if( m_ui ){

		auto q = m_ui->tableWidget ;

		const auto& r = this->window()->geometry() ;

		settings::instance().setWindowDimensions( { { { r.x(),
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
