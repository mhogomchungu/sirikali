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

#include "createbackendwindow.h"
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
#include "crypto.h"
#include "help.h"
#include "configoptions.h"
#include "win.h"
#include "json.h"
#include "settings.h"
#include "engines.h"

#include "favorites2.h"
#include "favorites.h"

static favorites::volumeList _readFavorites()
{
	favorites::volumeList e ;

	for( auto&& it : favorites::instance().readFavorites() ){

		e.emplace_back( std::move( it ),QByteArray() ) ;
	}

	return e ;
}

sirikali::sirikali( const QStringList& l ) :
	m_secrets( this ),
	m_mountInfo( this,true,[ & ](){ QCoreApplication::exit( m_exitStatus ) ; } ),
	m_checkUpdates( this,{ [ this ](){ this->disableAll() ; },[ this ](){ this->enableAll() ; } } ),
	m_configOptions( this,m_secrets,&m_language_menu,this->configOption() ),
	m_debugWindow(),
	m_signalHandler( this,this->getEmergencyShutDown() ),
	m_argumentList( l )
{
	utility::setMainQWidget( this ) ;
	favorites::instance().updateFavorites() ;
}

std::function< void( systemSignalHandler::signal ) > sirikali::getEmergencyShutDown()
{
	return [ this ]( systemSignalHandler::signal s ){

		Q_UNUSED( s )

		this->emergencyShutDown() ;
	} ;
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
	if( utility::platformIsWindows() && m_ui && !m_emergencyShuttingDown ){

		if( m_ui->tableWidget->rowCount() > 0 ){

			auto m = tr( "Close All Volumes Before Quitting The Application" ) ;
			return DialogMsg( this ).ShowUIOK( tr( "WARNING" ),m ) ;
		}
	}

	utility::quitHelper() ;

	m_exitStatus = s ;

	if( !e.isEmpty() ){

		utility::debug() << e ;
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

	m_signalHandler.listen() ;

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

			connect( ac,&QAction::triggered,[ = ](){

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

			auto name = it->name() ;

			name.replace( 0,1,name.at( 0 ).toUpper() ) ;

			auto ac = m->addAction( name ) ;

			ac->setObjectName( name ) ;

			if( it->isNotInstalled() ){

				ac->setEnabled( false ) ;

				if( name == "Ecryptfs" ){

					ac->setText( tr( "%1 Is Not Installed" ).arg( "Ecryptfs-simple" ) ) ;
				}else{
					ac->setText( tr( "%1 Is Not Installed" ).arg( name ) ) ;
				}
			}
		}

		return m ;
	}() ) ;

	this->setUpShortCuts() ;

	this->setUpFont() ;

	const auto icon = utility::getIcon() ;

	this->setAcceptDrops( true ) ;
	this->setWindowIcon( icon ) ;

	this->setUpAppMenu() ;

	this->setLocalizationLanguage( false ) ;

	m_trayIcon.setParent( this ) ;
	m_trayIcon.setIcon( icon ) ;

	this->showTrayIcon() ;

	this->disableAll() ;

	auto m = mountinfo::unlockedVolumes().await() ;

	this->updateVolumeList( m ) ;

	if( volume.isEmpty() ) {

		this->enableAll() ;
	}else{
		this->showMoungDialog( volume ) ;
	}

	this->startGUI( m ) ;

	QTimer::singleShot( settings::instance().checkForUpdateInterval(),this,SLOT( autoUpdateCheck() ) ) ;

	if( utility::debugEnabled() ){

		this->showDebugWindow() ;
	}

	auto e = settings::instance().runCommandOnInterval() ;
	auto p = settings::instance().runCommandOnIntervalTime() ;

	if( !e.isEmpty() ){

		e = utility::Task::makePath( e ) ;

		auto s = new QTimer( this ) ;

		connect( s,&QTimer::timeout,[ this,e ](){

			this->runIntervalCustomCommand( e ) ;
		} ) ;

		s->start( p * 60 * 1000 ) ;

		this->runIntervalCustomCommand( e ) ;
	}

	this->updateFavoritesInContextMenu() ;
}

void sirikali::showTrayIconWhenReady()
{
	m_trayIcon.show() ;
}

void sirikali::showTrayIcon()
{
	auto _show_tray = [ & ]{

		QMetaObject::invokeMethod( this,"showTrayIconWhenReady",Qt::QueuedConnection ) ;
	} ;

	for( int i = 0 ; i < 10 ; i++ ){

		if( QSystemTrayIcon::isSystemTrayAvailable() ){

			return _show_tray() ;
		}else{
			utility::waitForOneSecond();
		}
	}

	/*
	 * The tray doesnt seem to be ready yet but we cant wait any longer,just display it and
	 * hope for the best.
	 */
	_show_tray() ;
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

		m->addAction( _addAction( false,false,tr( "Check For Updates" ),"Check For Updates",
					  SLOT( updateCheck() ) ) ) ;
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

static void _connect_to_ssh_server( QWidget * parent,const favorites::entry& entry )
{
	auto s = siritask::encryptedFolderMount( entry ) ;

	if( s == engines::engine::status::sshfsNotFound ){

		auto e = QObject::tr( "Failed To Complete The Request.\nSshfs Executable Could Not Be Found." ) ;

		DialogMsg( parent ).ShowUIOK( QObject::tr( "ERROR" ),e ) ;

	}else if( s != engines::engine::status::success ){

		auto e = QObject::tr( "Failed To Connect To \"%1\" And Below Log was Generated By The Backend.\n" ).arg( entry.volumePath ) ;

		e += "\n----------------------------------------\n" + s.toMiniString() ;

		DialogMsg( parent ).ShowUIOK( QObject::tr( "ERROR" ),e ) ;
	}
}

void sirikali::favoriteClicked( QAction * ac )
{
	auto e = ac->objectName() ;

	if( e == "Manage Favorites" ){

		this->disableAll() ;

		favorites2::instance( this,favorites::type::others,[ this ](){

			this->enableAll() ;

			this->updateFavoritesInContextMenu() ;
		} ) ;
	}else{
		if( e == "Mount All" ){

			this->mountMultipleVolumes( _readFavorites() ) ;
		}else{
			QString volumePath ;
			QString mountPointPath ;

			utility2::stringListToStrings( utility::split( e ),volumePath,mountPointPath ) ;

			auto _found = [ & ]( const std::pair< favorites::entry,QByteArray >& e ){

				const auto& s = e.first ;

				if( mountPointPath.isEmpty() ){

					return s.volumePath == volumePath ;
				}else{
					return s.volumePath == volumePath && s.mountPointPath == mountPointPath ;
				}
			} ;

			for( auto&& it : _readFavorites() ){

				if( _found( it ) ){

					siritask::mount m{ it.first.volumePath,QString(),siritask::Engine() } ;

					auto s = siritask::mountEngine( m )->name() ;

					if( it.first.volumeNeedNoPassword && s == "sshfs" ){

						this->disableAll() ;

						_connect_to_ssh_server( this,it.first ) ;

						this->enableAll() ;
					}else{
						this->mountMultipleVolumes( this->autoUnlockVolumes( { std::move( it ) },true ) ) ;
					}

					break ;
				}
			}
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

void sirikali::startGUI( const std::vector< volumeInfo >& m )
{
	if( !m_startHidden ){

		this->raiseWindow() ;
	}

	if( settings::instance().autoMountFavoritesOnStartUp() ){

		this->autoUnlockVolumes( m ) ;
	}
}

void sirikali::createbackendwindow()
{
	createBackendWIndow::instance( this ) ;
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
	if( utility::platformIsWindows() ){

		if( m_argumentList.size() > 1 && m_argumentList.at( 1 ).startsWith( "-T" ) ){

			auto s = m_argumentList.at( 1 ) ;

			s.replace( "-T","" ) ;

			return SiriKali::Windows::terminateProcess( s.toULong() ) ;
		}
	}

	if( utility::printVersionOrHelpInfo( m_argumentList ) ){

		return 0 ;
	}else{
		QMetaObject::invokeMethod( this,
					   "start",
					   Qt::QueuedConnection ) ;
		return e.exec() ;
	}
}

void sirikali::polkitFailedWarning()
{
	DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "SiriKali Failed To Connect To siriPolkit.\nPlease Report This Serious Bug." ) ) ;
}

void sirikali::start()
{
	m_startHidden  = m_argumentList.contains( "-e" ) ;

	if( !m_startHidden ){

		m_startHidden = settings::instance().startMinimized() ;
	}

	m_folderOpener = utility::cmdArgumentValue( m_argumentList,"-m",settings::instance().fileManager() ) ;

	auto _cliCommand = [ & ](){

		return m_argumentList.contains( "-s" ) ||
		       m_argumentList.contains( "-u" ) ||
		       m_argumentList.contains( "-p" ) ||
		       !utility::cmdArgumentValue( m_argumentList,"-b" ).isEmpty() ;
	}() ;

	if( _cliCommand ){

		this->cliCommand( m_argumentList ) ;
	}else{
		utility::polkitFailedWarning( [ this ](){

			QMetaObject::invokeMethod( this,"polkitFailedWarning",Qt::QueuedConnection ) ;
		} ) ;

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
}

void sirikali::cliCommand( const QStringList& l )
{
	if( l.contains( "-s" ) ){

		auto e = utility::cmdArgumentValue( l,"-f" ) ;

		auto s = crypto::hmac_key( e,utility::readPassword() ) ;

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

				if( siritask::encryptedFolderUnMount( { a,b,c,5 } ).success() ){

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
	auto reverse   = l.contains( "-r" ) ;

	if( vol.isEmpty() ){

		this->closeApplication( 1,tr( "ERROR: Volume Path Not Given." ) ) ;
	}else{
		auto _unlockVolume = [ & ]( const QByteArray& key ){

			auto m = [ & ](){

				if( mountPath.isEmpty() ){

					auto e = utility::mountPathPostFix( volume.split( "/" ).last() ) ;

					return settings::instance().mountPath( e ) ;
				}else{
					return mountPath ;
				}
			}() ;

			m_mountInfo.announceEvents( false ) ;

			engines::engine::options s( volume,
						    m,key,
						    idleTime,
						    cPath,
						    { mode,reverse,false,false },
						    mOpt,
						    QString() ) ;

			auto e = siritask::encryptedFolderMount( s ) ;

			if( e == engines::engine::status::success ){

				//this->openMountPointPath( m ) ;

				this->closeApplication( 0 ) ;
			}else{
				utility::debug::cerr() << e.toString() ;
				this->closeApplication( 1 ) ;
			}
		} ;

		if( backEnd == "stdin" ){

			return _unlockVolume( [ & ](){

				auto e = utility::readPassword() ;

				if( keyFile.isEmpty() ){

					return e ;
				}else{
					return crypto::hmac_key( keyFile,e ) ;
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
				_unlockVolume( w.key.toUtf8() ) ;
			}
		}else{
			this->closeApplication( 1,tr( "ERROR: Failed To Unlock Requested Backend." ) ) ;
		}
	}
}

void sirikali::mountMultipleVolumes( favorites::volumeList e )
{
	if( !e.empty() ){

		m_disableEnableAll = true ;

		this->disableAll() ;

		keyDialog::instance( this,
				     m_secrets,
				     m_autoOpenFolderOnMount,
				     m_folderOpener,
				     std::move( e ),
				     [ this ](){ m_disableEnableAll = false ; this->enableAll() ; },
				     [ this ](){ this->updateList() ; } ) ;
	}
}

void sirikali::autoMountFavoritesOnAvailable( QString m )
{
	if( settings::instance().autoMountFavoritesOnAvailable() ){

		favorites::volumeList e ;

		for( auto&& it : _readFavorites() ){

			if( it.first.volumePath.startsWith( m ) && it.first.autoMount.True() ){

				e.emplace_back( std::move( it ) ) ;
			}
		}

		this->mountMultipleVolumes( this->autoUnlockVolumes( std::move( e ) ) ) ;
	}
}

void sirikali::autoUnlockVolumes( const std::vector< volumeInfo >& s )
{
	favorites::volumeList e ;

	auto _mounted = [ & ]( const QString& e ){

		for( const auto& it : s ){

			if( it.volumePath() == e ){

				return true ;
			}
		}

		return false ;
	} ;

	for( auto&& it : _readFavorites() ){

		const auto& m = it.first ;

		if( m.autoMount.True() && !_mounted( m.volumePath )){

			e.emplace_back( std::move( it ) ) ;
		}
	}

	this->mountMultipleVolumes( this->autoUnlockVolumes( std::move( e ) ) ) ;
}

favorites::volumeList sirikali::autoUnlockVolumes( favorites::volumeList l,bool autoOpenFolderOnMount )
{
	if( l.empty() ){

		return l ;
	}

	auto e = settings::instance().autoMountBackEnd() ;

	if( e.isInvalid() ){

		return l ;
	}

	auto m = m_secrets.walletBk( e.bk() ) ;

	if( !m ){

		return l ;
	}

	auto _mountVolumes = [ & ](){

		favorites::volumeList e ;

		auto _mount = [ & ]( favorites::volumeList& q,
				     const std::pair< favorites::entry,QByteArray >& e,
				     const QByteArray& key,
				     bool s ){
			if( s ){

				q.emplace_back( e.first,key ) ;
			}else{
				auto s = siritask::encryptedFolderMount( { e.first,key } ) ;

				if( s == engines::engine::status::success ){

					if( autoOpenFolderOnMount ){

						this->openMountPointPath( e.first.mountPointPath ) ;
					}

					if( !s.engine().autorefreshOnMountUnMount() ){

						this->updateList() ;
					}
				}
			}
		} ;

		auto s = settings::instance().showMountDialogWhenAutoMounting() ;

		for( const auto& it : l ){

			const auto key = m->readValue( it.first.volumePath ) ;

			if( key.isEmpty() ){

				e.emplace_back( it ) ;
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

		auto& s = settings::instance() ;

		if( m->open( s.walletName( m->backEnd() ),s.applicationName() ) ){

			return _mountVolumes() ;
		}else{
			return l ;
		}
	}
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

	if( utility::platformIsWindows() ){

		auto table = m_ui->tableWidget ;

		auto row = table->currentRow() ;

		auto m = SiriKali::Windows::volumeProperties( table->item( row,1 )->text() ) ;

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

	auto cp = tablewidget::rowEntries( table,table->currentRow() ) ;

	if( cp.size() > 0 ){

		favorites2::instance( this,favorites::type::others,[ this ](){

			this->updateFavoritesInContextMenu() ;

		},cp.first() ) ;
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

	this->addAction( _addAction( { Qt::Key_M },SLOT( createVolume() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_U },SLOT( pbUmount() ) ) ) ;

	this->addAction( _addAction( { Qt::Key_R },SLOT( pbUpdate() ) ) ) ;

	this->addAction( _addAction( { Qt::CTRL + Qt::Key::Key_W },SLOT( hideWindow() ) ) ) ;

	this->addAction( _addAction( { Qt::CTRL + Qt::Key::Key_B },SLOT( createbackendwindow() ) ) ) ;

	this->addAction( [ & ](){

		auto e = _addAction( { Qt::CTRL + Qt::Key::Key_Q },SLOT( closeApplication() ) ) ;

		e->setMenuRole( QAction::QuitRole ) ;

		e->setText( tr( "&Quit" ) ) ;

		return e ;
	}() ) ;

	this->addAction( _addAction( { Qt::CTRL + Qt::Key::Key_D },SLOT( showDebugWindow() ) ) ) ;
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

void sirikali::dropEvent( QDropEvent * e )
{
	favorites::volumeList s ;

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

		s.emplace_back( std::move( m ),QByteArray() ) ;
	}

	this->mountMultipleVolumes( std::move( s ) ) ;
}

void sirikali::mount( const volumeInfo& entry,const QString& exe,const QByteArray& key )
{
	this->disableAll() ;

	keyDialog::instance( this,
			     m_secrets,
			     entry,
			     [ this ](){ this->enableAll() ; },
			     [ this ](){ this->updateList() ; },
			     m_autoOpenFolderOnMount,
			     m_folderOpener,
			     exe,
			     key ) ;
}

void sirikali::createVolume( QAction * ac )
{
	if( ac ){

		auto s = ac->objectName() ;

		if( s == "Sshfs" ){

			favorites2::instance( this,favorites::type::sshfs,[ this ](){

				this->updateFavoritesInContextMenu() ;
			} ) ;
		}else{
			this->mount( volumeInfo(),s ) ;
		}
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

void sirikali::updateFavoritesInContextMenu()
{
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
	this->processMountedVolumes( [ = ]( const sirikali::mountedEntry& s ){

		auto a = utility::Task::makePath( s.cipherPath ) ;
		auto b = utility::Task::makePath( s.mountPoint ) ;
		auto c = s.volumeType ;

		auto key = utility::getKey( s.cipherPath,m_secrets ) ;

		Task::exec( [ = ](){

			const auto& e = utility::systemEnvironment() ;

			auto m = QString( "%1 %2 %3 %4" ).arg( cmd,a,b,c ) ;

			auto r = [ & ](){

				if( key.isEmpty() ){

					return Task::process::run( m,{},-1,{},e ).get() ;
				}else{
					auto s = e ;

					s.insert( settings::instance().environmentalVariableVolumeKey(),key ) ;

					return Task::process::run( m,{},-1,{},s ).get() ;
				}
			}() ;

			utility::logCommandOutPut( r,m ) ;
		} ) ;
	} ) ;
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

		auto row = tablewidget::columnHasEntry( table,entry.mountPoint(),1 ) ;

		if( row == -1 ){

			row = tablewidget::addRow( table ) ;
		}

		tablewidget::updateRow( table,entry.mountInfo().minimalList(),row,this->font() ) ;

		tablewidget::selectRow( table,row ) ;
	}
}

engines::engine::cmdStatus sirikali::unMountVolume( const sirikali::mountedEntry& e )
{
	auto s = siritask::encryptedFolderUnMount( { e.cipherPath,e.mountPoint,e.volumeType,5 } ) ;

	if( s.success() && s.engine().backendRequireMountPath() ){

		siritask::deleteMountFolder( e.mountPoint ) ;
	}

	return s ;
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

		utility::waitForOneSecond() ;

		auto s = this->unMountVolume( { a,b,c } ) ;

		if( s.success() ){

			if( !s.engine().autorefreshOnMountUnMount() ){

				this->updateList() ;
			}
		}else{
			DialogMsg( this ).ShowUIOK( tr( "ERROR" ),s.toString() ) ;

			if( s == engines::engine::status::failedToUnMount ){

				this->enableAll() ;
			}else{
				this->pbUpdate() ;
			}
		}
	}
}

void sirikali::processMountedVolumes( std::function< void( const sirikali::mountedEntry& ) > function )
{
	auto table = m_ui->tableWidget ;

	const auto cipherFolders = tablewidget::columnEntries( table,0 ) ;
	const auto mountPoints   = tablewidget::columnEntries( table,1 ) ;
	const auto fileSystems   = tablewidget::columnEntries( table,2 ) ;

	for( auto r = cipherFolders.size() - 1 ; r >= 0 ; r-- ){

		const auto& a = cipherFolders.at( r ) ;
		const auto& b = mountPoints.at( r ) ;
		const auto& c = fileSystems.at( r ) ;

		function( { a,b,c } ) ;
	}
}

void sirikali::emergencyShutDown()
{
	m_emergencyShuttingDown = true ;

	this->hide() ;

	m_mountInfo.announceEvents( false ) ;

	this->processMountedVolumes( [ this ]( const sirikali::mountedEntry& e ){

		this->unMountVolume( e ) ;
	} ) ;

	this->closeApplication( 0,"Emergency shut down" ) ;
}

void sirikali::unMountAll()
{
	m_mountInfo.announceEvents( false ) ;

	this->disableAll() ;

	utility::waitForOneSecond() ;

	this->processMountedVolumes( [ this ]( const mountedEntry& e ){

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

void sirikali::updateVolumeList( const std::vector< volumeInfo >& r )
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
	if( !utility::platformIsOSX() ){

		m_ui->pbmenu->setEnabled( false ) ;
		m_ui->pbupdate->setEnabled( false ) ;
		m_ui->tableWidget->setEnabled( false ) ;
		m_ui->pbunlockvolume->setEnabled( false ) ;
		m_ui->pbcreate->setEnabled( false ) ;
		m_ui->pbFavorites->setEnabled( false ) ;
	}
}

void sirikali::enableAll()
{
	if( !m_disableEnableAll ){

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
