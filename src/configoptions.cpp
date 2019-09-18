/*
 *
 *  Copyright (c) 2017
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

#include "configoptions.h"
#include "ui_configoptions.h"

#include "utility.h"
#include "walletconfig.h"

#include <QFileDialog>

configOptions::configOptions( QWidget * parent,
			      secrets& a,
			      QMenu * m,
			      configOptions::functions f ) :
	QDialog( parent ),
	m_ui( new Ui::configOptions ),
	m_functions( std::move( f ) ),
	m_secrets( a ),
	m_settings( settings::instance() )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbMountPointPrefix->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->pbPostMountCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbPreUnMountCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbSetExternalCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbSetFileManager->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbRunPeriodicallyCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	connect( m_ui->pushButton,&QPushButton::clicked,[ this ](){ this->HideUI() ; } ) ;

	connect( m_ui->cbAutoOpenMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoOpenFolderOnMount( e ) ;
	} ) ;

	connect( m_ui->cbShowFavoriteListOnContextMenu,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.showFavoritesInContextMenu( e ) ;
	} ) ;

	connect( m_ui->cbReUseMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.reUseMountPoint( e ) ;
	} ) ;

	connect( m_ui->cbAutoCheckForUpdates,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoCheck( e ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		m_ui->cbAutoCheckForUpdates->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;
		m_ui->pbKeyStorage->setEnabled( false ) ;
	}else{
		m_ui->pbChangeWalletPassword->setEnabled( [ this ](){

			auto a = m_settings.walletName() ;
			auto b = m_settings.applicationName() ;

			return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
		}() ) ;
	}

	connect( m_ui->cbStartMinimized,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.setStartMinimized( e ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		m_ui->label->setText( tr( "Set Executables Search Path" ) ) ;
		m_ui->lineEditMountPointPrefix->clear() ;
		m_ui->lineEditMountPointPrefix->setEnabled( true ) ;
		m_ui->pbMountPointPrefix->setEnabled( true ) ;
		m_ui->lineEditMountPointPrefix->setText( m_settings.windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	}

	connect( m_ui->pbMountPointPrefix,&QPushButton::clicked,[ this ](){

		auto e = utility::getExistingDirectory( this,QString(),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditMountPointPrefix->setText( e ) ;

			if( utility::platformIsWindows() ){

				m_settings.setWindowsExecutableSearchPath( e ) ;
			}else{
				m_settings.setDefaultMountPointPrefix( e ) ;
			}
		}
	} ) ;

	connect( m_ui->cbAutoMountAtStartUp,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoMountFavoritesOnStartUp( e ) ;
	} ) ;

	m_ui->cbAutoMountWhenAvailable->setChecked( m_settings.autoMountFavoritesOnAvailable() ) ;


	connect( m_ui->cbAllowExternalToolsToReadPasswords,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.allowExternalToolsToReadPasswords( e ) ;
	} ) ;

	connect( m_ui->cbAutoMountWhenAvailable,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoMountFavoritesOnAvailable( e ) ;
	} ) ;

	connect( m_ui->cbShowMountDialogWhenAutoMounting,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.showMountDialogWhenAutoMounting( e ) ;
	} ) ;

	connect( m_ui->pbChangeWalletPassword,&QPushButton::clicked,[ this ](){

		auto a = m_settings.walletName() ;
		auto b = m_settings.applicationName() ;

		this->hide() ;

		m_secrets.changeInternalWalletPassword( a,b,[ this ](){ this->show() ; } ) ;
	} ) ;

	m_ui->pbSelectLanguage->setMenu( m ) ;

	connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

		m_functions.function_2( ac ) ;

		this->translateUI() ;
	} ) ;

	m_ui->pbKeyStorage->setMenu( [ this ](){

		using bk = LXQt::Wallet::BackEnd ;

		auto m = new QMenu( this ) ;

		auto _addAction = [ m,this ]( const QString& e,const char * s,bk z ){

			auto ac = m->addAction( e ) ;

			ac->setEnabled( LXQt::Wallet::backEndIsSupported( z ) ) ;

			m_actionPair.emplace_back( ac,s ) ;

			connect( ac,&QAction::triggered,[ this,z ](){

				this->hide() ;

				walletconfig::instance( this,m_secrets.walletBk( z ),[ this ](){ this->show() ; } ) ;
			} ) ;
		} ;

		_addAction( tr( "Internal Wallet" ),"Internal Wallet",bk::internal ) ;
		_addAction( tr( "Libsecret" ),"Libsecret",bk::libsecret ) ;
		_addAction( tr( "KWallet" ),"KWallet",bk::kwallet ) ;
		_addAction( tr( "MACOS Keychain" ),"MACOS Keychain",bk::osxkeychain ) ;

		return m ;
	}() ) ;

	using bk = LXQt::Wallet::BackEnd ;

	auto walletBk = m_settings.autoMountBackEnd() ;

	if( walletBk == bk::internal ){

		m_ui->rbInternalWallet->setChecked( true ) ;

	}else if( walletBk == bk::osxkeychain ){

		m_ui->rbMacOSKeyChain->setChecked( true ) ;

	}else if( walletBk == bk::libsecret ){

		m_ui->rbLibSecret->setChecked( true ) ;

	}else if( walletBk == bk::kwallet ){

		m_ui->rbKWallet->setChecked( true ) ;
	}else{
		m_ui->rbNone->setChecked( true ) ;
	}

	m_ui->rbInternalWallet->setEnabled( LXQt::Wallet::backEndIsSupported( bk::internal ) ) ;
	m_ui->rbKWallet->setEnabled( LXQt::Wallet::backEndIsSupported( bk::kwallet ) ) ;
	m_ui->rbLibSecret->setEnabled( LXQt::Wallet::backEndIsSupported( bk::libsecret ) ) ;
	m_ui->rbMacOSKeyChain->setEnabled( LXQt::Wallet::backEndIsSupported( bk::osxkeychain ) ) ;
	m_ui->rbNone->setEnabled( true ) ;

	connect( m_ui->rbInternalWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_settings.autoMountBackEnd( bk::internal ) ;
		}
	} ) ;

	connect( m_ui->rbKWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_settings.autoMountBackEnd( bk::kwallet ) ;
		}
	} ) ;

	connect( m_ui->rbLibSecret,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_settings.autoMountBackEnd( bk::libsecret ) ;
		}
	} ) ;

	connect( m_ui->rbMacOSKeyChain,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_settings.autoMountBackEnd( bk::osxkeychain ) ;
		}
	} ) ;

	connect( m_ui->rbNone,&QRadioButton::toggled,[ this ](){

		m_settings.autoMountBackEnd( settings::walletBackEnd() ) ;
	} ) ;

	connect( m_ui->pbPostMountCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Post Mount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditAfterMountCommand->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbPreUnMountCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Pre UnMount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditBeforesUnMount->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbRunPeriodicallyCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Pre UnMount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditRunPeriodically->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetFileManager,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Command To Open Mount Points" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditFileManager->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetExternalCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set External Plugin Executable" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditExecutableKeySource->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetFileManagerToDefault,&QPushButton::pressed,[ this ](){

		m_settings.setFileManager( QString() ) ;
		m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;
	} ) ;

	connect( m_ui->pbExternalExecutableDefault,&QPushButton::pressed,[ this ](){

		m_settings.setExternalPluginExecutable( QString() ) ;

		m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;
	} ) ;

	connect( m_ui->pbMountPointPrefixDefault,&QPushButton::pressed,[ this ](){

		m_settings.setDefaultMountPointPrefix( QString() ) ;
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	} ) ;

	connect( m_ui->pbRunPostMountCommandDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditAfterMountCommand->clear() ;
	} ) ;

	connect( m_ui->pbRunPreUnMountCommandDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditBeforesUnMount->clear() ;
	} ) ;

	connect( m_ui->pbRunCommandPeriodicallyDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditRunPeriodically->clear() ;
	} ) ;

	connect( m_ui->cbHiDPI,&QCheckBox::toggled,[ this ]( bool e ){

		m_ui->lineEditHiDPI->setEnabled( e ) ;
	} ) ;
}

configOptions::~configOptions()
{
	delete m_ui ;
}

void configOptions::translateUI()
{
	m_ui->retranslateUi( this ) ;

	for( auto& it : m_actionPair ){

		it.first->setText( tr( it.second ) ) ;
	}

	if( utility::platformIsWindows() ){

		m_ui->label->setText( tr( "Set Executables Search Path" ) ) ;
	}
}

void configOptions::ShowUI()
{
	m_ui->cbAllowExternalToolsToReadPasswords->setChecked( m_settings.allowExternalToolsToReadPasswords() ) ;

	m_ui->cbAutoOpenMountPoint->setChecked( m_settings.autoOpenFolderOnMount() ) ;

	m_ui->cbReUseMountPoint->setChecked( m_settings.reUseMountPoint() ) ;

	m_ui->cbAutoCheckForUpdates->setChecked( m_settings.autoCheck() ) ;

	m_ui->cbStartMinimized->setChecked( m_settings.startMinimized() ) ;

	m_ui->cbAutoMountAtStartUp->setChecked( m_settings.autoMountFavoritesOnStartUp() ) ;

	m_ui->cbShowFavoriteListOnContextMenu->setChecked( m_settings.showFavoritesInContextMenu() ) ;

	m_ui->cbShowMountDialogWhenAutoMounting->setChecked( m_settings.showMountDialogWhenAutoMounting() ) ;

	m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;

	m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;

	m_ui->lineEditAfterMountCommand->setText( m_settings.runCommandOnMount() ) ;

	m_ui->lineEditBeforesUnMount->setText( m_settings.preUnMountCommand() ) ;

	m_ui->lineEditRunPeriodically->setText( m_settings.runCommandOnInterval() ) ;

	m_ui->lineEditRunPeriodicallyInterval->setText( QString::number( m_settings.runCommandOnIntervalTime() ) ) ;

	m_ui->cbHiDPI->setChecked( m_settings.enableHighDpiScaling() ) ;

	m_ui->lineEditHiDPI->setText( m_settings.enabledHighDpiScalingFactor() ) ;

	m_ui->lineEditHiDPI->setEnabled( m_ui->cbHiDPI->isChecked() ) ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPointPrefix->setText( m_settings.windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	}

	this->show() ;
}

void configOptions::HideUI()
{
	m_functions.function_1() ;

	m_settings.setFileManager( m_ui->lineEditFileManager->text() ) ;
	m_settings.setExternalPluginExecutable( m_ui->lineEditExecutableKeySource->text() ) ;
	m_settings.preUnMountCommand( m_ui->lineEditBeforesUnMount->text() ) ;
	m_settings.runCommandOnMount( m_ui->lineEditAfterMountCommand->text() ) ;
	m_settings.runCommandOnInterval( m_ui->lineEditRunPeriodically->text() ) ;
	m_settings.enableHighDpiScaling( m_ui->cbHiDPI->isChecked() ) ;

	m_settings.enabledHighDpiScalingFactor( [ this ]()->QString{

		auto s = m_ui->lineEditHiDPI->text() ;
		bool ok ;

		s.toDouble( &ok ) ;

		if( ok ){

			return s ;
		}else{
			return "1.0" ;
		}
	}() ) ;

	bool ok ;

	auto a = m_ui->lineEditRunPeriodicallyInterval->text() ;

	auto b = a.toInt( &ok ) ;

	if( ok ){

		m_settings.runCommandOnIntervalTime( b ) ;
	}else{
		m_settings.runCommandOnIntervalTime( 10 ) ;
	}

	if( utility::platformIsWindows() ){

		m_settings.setWindowsExecutableSearchPath( m_ui->lineEditMountPointPrefix->text() ) ;
	}else{
		m_settings.setDefaultMountPointPrefix( m_ui->lineEditMountPointPrefix->text() ) ;
	}

	this->hide() ;
}

void configOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
