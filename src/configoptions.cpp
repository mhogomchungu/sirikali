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

static bool _actionNotSet = true ;

configOptions::configOptions( QWidget * parent,
			      secrets& a,
			      QMenu * m,
			      std::function< void() > function,
			      std::function< void( QAction * ) > u ) :
	QDialog( parent ),
	m_ui( new Ui::configOptions ),
	m_function( std::move( function ) ),
	m_updateLanguage( std::move( u ) ),
	m_secrets( a )
{
	m_ui->setupUi( this ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->lineEditFileManager->setText( utility::fileManager() ) ;

	m_ui->lineEditExecutableKeySource->setText( utility::externalPluginExecutable() ) ;

	m_ui->lineEditAfterMountCommand->setText( utility::runCommandOnMount() ) ;

	m_ui->lineEditBeforesUnMount->setText( utility::preUnMountCommand() ) ;

	connect( m_ui->pushButton,&QPushButton::clicked,[ this ](){ this->HideUI() ; } ) ;

	m_ui->cbAutoOpenMountPoint->setChecked( utility::autoOpenFolderOnMount() ) ;

	connect( m_ui->cbAutoOpenMountPoint,&QCheckBox::toggled,[]( bool e ){

		utility::autoOpenFolderOnMount( e ) ;
	} ) ;

	m_ui->cbReUseMountPoint->setChecked( utility::reUseMountPoint() ) ;

	connect( m_ui->cbReUseMountPoint,&QCheckBox::toggled,[]( bool e ){

		utility::reUseMountPoint( e ) ;
	} ) ;

	m_ui->cbAutoCheckForUpdates->setChecked( utility::autoCheck() ) ;

	connect( m_ui->cbAutoCheckForUpdates,&QCheckBox::toggled,[]( bool e ){

		utility::autoCheck( e ) ;
	} ) ;

	m_ui->cbStartMinimized->setChecked( utility::startMinimized() ) ;

	connect( m_ui->cbStartMinimized,&QCheckBox::toggled,[]( bool e ){

		utility::setStartMinimized( e ) ;
	} ) ;

	m_ui->lineEditMountPointPrefix->setText( utility::mountPath() ) ;

	connect( m_ui->pbMountPointPrefix,&QPushButton::clicked,[ this ](){

		auto e = utility::getExistingDirectory( this,QString(),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditMountPointPrefix->setText( e ) ;
		}
	} ) ;

	m_ui->cbAutoMountAtStartUp->setChecked( utility::autoMountFavoritesOnStartUp() ) ;

	connect( m_ui->cbAutoMountAtStartUp,&QCheckBox::toggled,[]( bool e ){

		utility::autoMountFavoritesOnStartUp( e ) ;
	} ) ;

	m_ui->cbAutoMountWhenAvailable->setChecked( utility::autoMountFavoritesOnAvailable() ) ;

	connect( m_ui->cbAutoMountWhenAvailable,&QCheckBox::toggled,[]( bool e ){

		utility::autoMountFavoritesOnAvailable( e ) ;
	} ) ;

	m_ui->cbShowMountDialogWhenAutoMounting->setChecked( utility::showMountDialogWhenAutoMounting() ) ;

	connect( m_ui->cbShowMountDialogWhenAutoMounting,&QCheckBox::toggled,[]( bool e ){

		utility::showMountDialogWhenAutoMounting( e ) ;
	} ) ;

	m_ui->pbChangeWalletPassword->setEnabled( [](){

		auto a = utility::walletName() ;
		auto b = utility::applicationName() ;

		return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
	}() ) ;

	connect( m_ui->pbChangeWalletPassword,&QPushButton::clicked,[ this ](){

		auto a = utility::walletName() ;
		auto b = utility::applicationName() ;

		this->hide() ;

		m_secrets.changeInternalWalletPassword( a,b,[ this ](){ this->show() ; } ) ;
	} ) ;

	m_ui->pbSelectLanguage->setMenu( m ) ;

	if( _actionNotSet ){

		_actionNotSet = false ;

		connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

			m_updateLanguage( ac ) ;

			m_ui->retranslateUi( this ) ;

			for( auto& it : m_actionPair ){

				it.first->setText( tr( it.second ) ) ;
			}
		} ) ;
	}

	m_ui->pbKeyStorage->setMenu( [ this ](){

		using bk = LXQt::Wallet::BackEnd ;

		auto m = new QMenu( this ) ;

		auto ac = m->addAction( tr( "Internal Wallet" ),[ this ](){

			this->hide() ;

			walletconfig::instance( this,
						m_secrets.walletBk( bk::internal ),
						[ this ](){ this->show() ; } ) ;
		} ) ;

		m_actionPair.emplace_back( ac,"Internal Wallet" ) ;

		ac = m->addAction( tr( "Libsecret" ),[ this ](){

			walletconfig::instance( this,m_secrets.walletBk( bk::libsecret ) ) ;
		} ) ;

		m_actionPair.emplace_back( ac,"Libsecret" ) ;

		ac = m->addAction( tr( "KWallet" ),[ this ](){

			walletconfig::instance( this,m_secrets.walletBk( bk::kwallet ) ) ;
		} ) ;

		m_actionPair.emplace_back( ac,"KWallet" ) ;

		ac = m->addAction( tr( "MACOS Keychain" ),[ this ](){

			walletconfig::instance( this,m_secrets.walletBk( bk::osxkeychain ) ) ;
		} ) ;

		m_actionPair.emplace_back( ac,"MACOS Keychain" ) ;

		ac->setEnabled( utility::platformIsOSX() ) ;

		return m ;
	}() ) ;

	using bk = LXQt::Wallet::BackEnd ;

	auto walletBk = utility::autoMountBackEnd() ;

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

	connect( m_ui->rbInternalWallet,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			utility::autoMountBackEnd( bk::internal ) ;
		}
	} ) ;

	connect( m_ui->rbKWallet,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			utility::autoMountBackEnd( bk::kwallet ) ;
		}
	} ) ;

	connect( m_ui->rbLibSecret,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			utility::autoMountBackEnd( bk::libsecret ) ;
		}
	} ) ;

	connect( m_ui->rbMacOSKeyChain,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			utility::autoMountBackEnd( bk::osxkeychain ) ;
		}
	} ) ;

	connect( m_ui->rbNone,&QRadioButton::toggled,[](){

		utility::autoMountBackEnd( utility::walletBackEnd() ) ;
	} ) ;

	this->show() ;
}

configOptions::~configOptions()
{
	delete m_ui ;
}

void configOptions::HideUI()
{
	m_function() ;

	utility::setFileManager( m_ui->lineEditFileManager->text() ) ;
	utility::setExternalPluginExecutable( m_ui->lineEditExecutableKeySource->text() ) ;
	utility::preUnMountCommand( m_ui->lineEditBeforesUnMount->text() ) ;
	utility::runCommandOnMount( m_ui->lineEditAfterMountCommand->text() ) ;
	utility::setDefaultMountPointPrefix( m_ui->lineEditMountPointPrefix->text() ) ;

	this->hide() ;
	this->deleteLater() ;
}

void configOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}