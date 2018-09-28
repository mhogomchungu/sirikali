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
#include "crypto.h"
#include "help.h"
#include "configoptions.h"
#include "winfsp.h"
#include "json.h"

static utility::volumeList _readFavorites()
{
	utility::volumeList e ;

	for( auto&& it : utility::readFavorites() ){

		e.emplace_back( std::move( it ),QByteArray() ) ;
	}

	return e ;
}

sirikali::sirikali() :
	m_secrets( this ),
	m_mountInfo( this,true,[ & ](){ QCoreApplication::exit( m_exitStatus ) ; } ),
	m_checkUpdates( this ),
	m_configOptions( this,m_secrets,&m_language_menu,this->configOption() ),
	m_debugWindow()
{
	utility::setDebugWindow( &m_debugWindow ) ;
}

configOptions::functions sirikali::configOption()
{
	auto a = [ this ](){

		this->enableAll() ;
		m_folderOpener = utility::fileManager() ;
	} ;

	auto b = [ this ]( QAction * ac ){

		utility::languageMenu( &m_language_menu,ac,m_translator ) ;

		m_ui->retranslateUi( this ) ;

		for( auto& it : m_actionPair ){

			it.first->setText( tr( it.second ) ) ;
		}

		for( auto& it : m_menuPair ){

			it.first->setTitle( tr( it.second ) ) ;
		}
	} ;

	return { std::move( a ),std::move( b ) } ;
}

void sirikali::closeApplication( int s,const QString& e )
{
	if( utility::platformIsWindows() && m_ui ){

		if( SiriKali::Winfsp::babySittingBackends() && m_ui->tableWidget->rowCount() > 0 ){

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
		utility::Task::suspendForOneSecond() ;
	}

	m_mountInfo.stop() ;
}

void sirikali::setUpApp( const QString& volume )
{
	this->setLocalizationLanguage( true ) ;

	m_ui = new Ui::sirikali ;
	m_ui->setupUi( this ) ;

	m_configOptions.translateUI() ;

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

	m_ui->tableWidget->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui->tableWidget->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui->tableWidget->setMouseTracking( true ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	connect( m_ui->tableWidget,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s ) ;

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

	connect( m_ui->pbunlockvolume,SIGNAL( clicked() ),
		 this,SLOT( unlockVolume() ) ) ;

	m_ui->pbcreate->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		connect( m,SIGNAL( triggered( QAction * ) ),
			 this,SLOT( createVolume( QAction * ) ) ) ;

		auto _enable = []( QAction * ac,const QString& exe ){

			ac->setObjectName( exe ) ;

			if( utility::executableFullPath( exe.toLower() ).isEmpty() ){

				ac->setEnabled( false ) ;

				if( exe == "Ecryptfs" ){

					ac->setText( tr( "%1 Is Not Installed" ).arg( "Ecryptfs-simple" ) ) ;
				}else{
					ac->setText( tr( "%1 Is Not Installed" ).arg( exe ) ) ;
				}
			}
		} ;

		if( utility::platformIsWindows() ){

			_enable( m->addAction( "Encfs" ),"Encfs" ) ;
			_enable( m->addAction( "Securefs" ),"Securefs" ) ;
			_enable( m->addAction( "Sshfs" ),"Sshfs" ) ;

		}else if( utility::platformIsOSX() ){

			_enable( m->addAction( "Cryfs" ),"Cryfs" ) ;
			_enable( m->addAction( "Gocryptfs" ),"Gocryptfs" ) ;
			_enable( m->addAction( "Securefs" ),"Securefs" ) ;
			_enable( m->addAction( "Encfs" ),"Encfs" ) ;
		}else{
			_enable( m->addAction( "Cryfs" ),"Cryfs" ) ;
			_enable( m->addAction( "Gocryptfs" ),"Gocryptfs" ) ;
			_enable( m->addAction( "Securefs" ),"Securefs" ) ;
			_enable( m->addAction( "Encfs" ),"Encfs" ) ;
			_enable( m->addAction( "Ecryptfs" ),"Ecryptfs" ) ;
			_enable( m->addAction( "Sshfs" ),"Sshfs" ) ;
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

	QTimer::singleShot( utility::checkForUpdateInterval(),this,SLOT( autoUpdateCheck() ) ) ;

	if( utility::debugEnabled() || utility::debugFullEnabled() ){

		this->showDebugWindow() ;
	}
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
			utility::Task::suspendForOneSecond() ;
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

#ifndef Q_OS_WIN
	m->addAction( _addAction( false,false,tr( "Check For Updates" ),"Check For Updates",
				  SLOT( updateCheck() ) ) ) ;
#endif
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

	m_ui->pbmenu->setMenu( m ) ;

	m_trayIcon.setContextMenu( m ) ;

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

void sirikali::favoriteClicked( QAction * ac )
{
	auto e = ac->objectName() ;

	if( e == "Manage Favorites" ){

		favorites::instance( this ) ;
	}else{
		if( e == "Mount All" ){

			this->mountMultipleVolumes( _readFavorites() ) ;
		}else{
			for( auto&& it : _readFavorites() ){

				if( it.first.volumePath == e ){

					this->mountMultipleVolumes( this->autoUnlockVolumes( { std::move( it ) },true ) ) ;

					break ;
				}
			}
		}
	}
}

void sirikali::showFavorites()
{
	utility::readFavorites( m_ui->pbFavorites->menu() ) ;
}

void sirikali::setLocalizationLanguage( bool translate )
{
	utility::setLocalizationLanguage( translate,&m_language_menu,m_translator ) ;
}

void sirikali::startGUI( const std::vector< volumeInfo >& m )
{
	if( !m_startHidden ){

		this->raiseWindow() ;
	}

	if( utility::autoMountFavoritesOnStartUp() ){

		this->autoUnlockVolumes( m ) ;
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

	for( const QString& it : l ){

		if( it.startsWith( "terminateProcess-" ) ){

			auto s = it ;

			s.replace( "terminateProcess-","" ) ;

			return SiriKali::Winfsp::terminateProcess( s.toULong() ) ;
		}
	}

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

void sirikali::polkitFailedWarning()
{
	DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "SiriKali Failed To Connect To siriPolkit.\nPlease Report This Serious Bug." ) ) ;
}

void sirikali::start( const QStringList& l )
{
	utility::enableDebug( l.contains( "--debug" ) ) ;
	utility::enableFullDebug( l.contains( "--debug-full" ) ) ;

	m_startHidden  = l.contains( "-e" ) ;

	if( !m_startHidden ){

		m_startHidden = utility::startMinimized() ;
	}

	m_folderOpener = utility::cmdArgumentValue( l,"-m",utility::fileManager() ) ;

	auto _cliCommand = [ & ](){

		return l.contains( "-s" ) ||
		       l.contains( "-u" ) ||
		       l.contains( "-p" ) ||
		       !utility::cmdArgumentValue( l,"-b" ).isEmpty() ;
	}() ;

	if( _cliCommand ){

		this->cliCommand( l ) ;
	}else{
		utility::polkitFailedWarning( [ this ](){

			QMetaObject::invokeMethod( this,"polkitFailedWarning",Qt::QueuedConnection ) ;
		} ) ;

		auto s = utility::socketPath() ;

		if( !utility::platformIsWindows() ){

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

		auto x = utility::cmdArgumentValue( l,"-d" ) ;

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
				_unlockVolume( w.key ) ;
			}
		}else{
			this->closeApplication( 1,tr( "ERROR: Failed To Unlock Requested Backend." ) ) ;
		}
	}
}

void sirikali::mountMultipleVolumes( utility::volumeList e )
{
	if( !e.empty() ){

		m_disableEnableAll = true ;

		this->disableAll() ;

		keyDialog::instance( this,
				     m_secrets,
				     m_autoOpenFolderOnMount,
				     m_folderOpener,
				     std::move( e ),
				     [ this ](){ m_disableEnableAll = false ; this->enableAll() ; } ) ;
	}
}

void sirikali::autoMountFavoritesOnAvailable( QString m )
{
	if( utility::autoMountFavoritesOnAvailable() ){

		utility::volumeList e ;

		for( auto&& it : _readFavorites() ){

			if( it.first.volumePath.startsWith( m ) && it.first.autoMount() ){

				e.emplace_back( std::move( it ) ) ;
			}
		}

		this->mountMultipleVolumes( this->autoUnlockVolumes( std::move( e ) ) ) ;
	}
}

void sirikali::autoUnlockVolumes( const std::vector< volumeInfo >& s )
{
	utility::volumeList e ;

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

		if( m.autoMount() && !_mounted( m.volumePath )){

			e.emplace_back( std::move( it ) ) ;
		}
	}

	this->mountMultipleVolumes( this->autoUnlockVolumes( std::move( e ) ) ) ;
}

utility::volumeList sirikali::autoUnlockVolumes( utility::volumeList l,bool autoOpenFolderOnMount )
{
	if( l.empty() ){

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

		utility::volumeList e ;

		auto _mount = [ & ]( utility::volumeList& q,
				     const std::pair< favorites::entry,QByteArray >& e,
				     const QByteArray& key,
				     bool s ){
			if( s ){

				q.emplace_back( e.first,key ) ;
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

		return QString( [](){

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

static utility::result< QByteArray > _volume_properties( const QString& cmd,
							 const std::pair< QString,QString >& args,
							 const QString& path )
{
	auto e = utility::Task::run( cmd + args.first + path ).await() ;

	if( e.success() ){

		return e.stdOut() ;
	}else{
		for( const auto& it : utility::readFavorites() ){

			if( utility::Task::makePath( it.volumePath ) == path ){

				auto s = [ & ]{

					if( cmd.endsWith( "gocryptfs\"" ) ){

						auto a = utility::Task::makePath( it.configFilePath ) ;
						auto b = utility::Task::makePath( it.volumePath ) ;

						return a + " " + b ;

					}else if( utility::endsWithAtLeastOne( cmd,"encfsctl\"","encfsctl.exe\"" ) ){

						auto s = it.configFilePath ;

						int index = s.lastIndexOf( "/" ) ;
						if( index != -1 ){

							s.remove( index,s.length() - index ) ;
						}

						return utility::Task::makePath( s ) ;
					}else{
						return utility::Task::makePath( it.configFilePath ) ;
					}
				}() ;

				e = utility::Task::run( cmd + args.first + args.second + s ).await() ;

				if( e.success() ){

					return e.stdOut() ;
				}else{
					break ;
				}
			}
		}

		return utility::result< QByteArray >() ;
	}
}

static void _volume_properties( const QString& cmd,const std::pair<QString,QString>& args,
				QTableWidget * table,QWidget * w )
{
	auto exe = utility::executableFullPath( cmd ) ;

	auto path = [ & ](){

		auto row = table->currentRow() ;

		if( row < 0 ){

			return QString() ;
		}else{
			auto m = table->item( row,2 )->text() ;

			if( cmd == "gocryptfs" && m == "gocryptfs-reverse" ){

				return utility::Task::makePath( table->item( row,1 )->text() ) ;
			}else{
				return utility::Task::makePath( table->item( row,0 )->text() ) ;
			}
		}
	}() ;

	if( exe.isEmpty() ){

		DialogMsg( w ).ShowUIOK( QObject::tr( "ERROR" ),
					 QObject::tr( "Failed To Find %1 Executable" ).arg( cmd ) ) ;
	}else{
		auto e = _volume_properties( utility::Task::makePath( exe ),args,path ) ;

		if( e ){

			auto& s = e.value() ;

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

	_volume_properties( "encfsctl",{ " ","" },m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::securefsProperties()
{
	this->disableAll() ;

	_volume_properties( "securefs",{ " info ","" },m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::gocryptfsProperties()
{
	this->disableAll() ;

	_volume_properties( "gocryptfs",{ " -info "," -config " },m_ui->tableWidget,this ) ;

	this->enableAll() ;
}

void sirikali::sshfsProperties()
{
	auto table = m_ui->tableWidget ;

	auto row = table->currentRow() ;

	auto m = SiriKali::Winfsp::volumeProperties( table->item( row,1 )->text() ) ;

	if( m.isEmpty() ){

		DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Read Volume Properties" ) ) ;
	}else{
		DialogMsg( this ).ShowUIInfo( tr( "INFORMATION" ),true,m ) ;
	}
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

				return { SLOT( cryfsProperties() ),!utility::platformIsWindows() } ;

			}else if( e == "ecryptfs" ){

				return { SLOT( ecryptfsProperties() ),true } ;

			}else if( e == "securefs" ){

				return { SLOT( securefsProperties() ),true } ;

			}else if( e == "encfs" ){

				return { SLOT( encfsProperties() ),true } ;

			}else if( utility::containsAtleastOne( e,"gocryptfs","gocryptfs-reverse" ) ){

				return { SLOT( gocryptfsProperties() ),true } ;

			}else if( e == "sshfs" ){

				if( utility::platformIsWindows() ){

					return { SLOT( sshfsProperties() ),true } ;
				}
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
	if( utility::autoOpenFolderOnMount() ){

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
	utility::volumeList s ;

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

			favorites::instance( this,favorites::type::sshfs ) ;
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

void sirikali::unlockVolume()
{
	this->disableAll() ;

	auto e = tr( "Select An Encrypted Volume Directory" ) ;
	auto path = utility::getExistingDirectory( this,e,utility::homePath() ) ;

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
