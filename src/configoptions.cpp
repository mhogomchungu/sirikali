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
#include "settings.h"
#include "walletconfig.h"

#include <QFileDialog>

configOptions::configOptions( QWidget * parent,
			      secrets& a,
			      QMenu * m,
			      configOptions::functions f ) :
	QDialog( parent ),
	m_ui( new Ui::configOptions ),
	m_functions( std::move( f ) ),
	m_secrets( a )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbMountPointPrefix->setIcon( QIcon( ":/folder.png" ) ) ;

	connect( m_ui->pushButton,&QPushButton::clicked,[ this ](){ this->HideUI() ; } ) ;

	connect( m_ui->cbAutoOpenMountPoint,&QCheckBox::toggled,[]( bool e ){

		settings::instance().autoOpenFolderOnMount( e ) ;
	} ) ;

	connect( m_ui->cbReUseMountPoint,&QCheckBox::toggled,[]( bool e ){

		settings::instance().reUseMountPoint( e ) ;
	} ) ;

	connect( m_ui->cbAutoCheckForUpdates,&QCheckBox::toggled,[]( bool e ){

		settings::instance().autoCheck( e ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		m_ui->cbAutoCheckForUpdates->setEnabled( false ) ;
	}

	connect( m_ui->cbStartMinimized,&QCheckBox::toggled,[]( bool e ){

		settings::instance().setStartMinimized( e ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		m_ui->label->setText( tr( "Set Executables Search Path" ) ) ;
		m_ui->lineEditMountPointPrefix->clear() ;
		m_ui->lineEditMountPointPrefix->setEnabled( true ) ;
		m_ui->pbMountPointPrefix->setEnabled( true ) ;
		m_ui->lineEditMountPointPrefix->setText( settings::instance().windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( settings::instance().mountPath() ) ;
	}

	connect( m_ui->pbMountPointPrefix,&QPushButton::clicked,[ this ](){

		auto e = utility::getExistingDirectory( this,QString(),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditMountPointPrefix->setText( e ) ;

			if( utility::platformIsWindows() ){

				settings::instance().setWindowsExecutableSearchPath( e ) ;
			}else{
				settings::instance().setDefaultMountPointPrefix( e ) ;
			}
		}
	} ) ;

	connect( m_ui->cbAutoMountAtStartUp,&QCheckBox::toggled,[]( bool e ){

		settings::instance().autoMountFavoritesOnStartUp( e ) ;
	} ) ;

	m_ui->cbAutoMountWhenAvailable->setChecked( settings::instance().autoMountFavoritesOnAvailable() ) ;

	connect( m_ui->cbAutoMountWhenAvailable,&QCheckBox::toggled,[]( bool e ){

		settings::instance().autoMountFavoritesOnAvailable( e ) ;
	} ) ;

	connect( m_ui->cbShowMountDialogWhenAutoMounting,&QCheckBox::toggled,[]( bool e ){

		settings::instance().showMountDialogWhenAutoMounting( e ) ;
	} ) ;

	m_ui->pbChangeWalletPassword->setEnabled( [](){

		auto a = settings::instance().walletName() ;
		auto b = settings::instance().applicationName() ;

		return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
	}() ) ;

	connect( m_ui->pbChangeWalletPassword,&QPushButton::clicked,[ this ](){

		auto a = settings::instance().walletName() ;
		auto b = settings::instance().applicationName() ;

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

	auto walletBk = settings::instance().autoMountBackEnd() ;

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

			settings::instance().autoMountBackEnd( bk::internal ) ;
		}
	} ) ;

	connect( m_ui->rbKWallet,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			settings::instance().autoMountBackEnd( bk::kwallet ) ;
		}
	} ) ;

	connect( m_ui->rbLibSecret,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			settings::instance().autoMountBackEnd( bk::libsecret ) ;
		}
	} ) ;

	connect( m_ui->rbMacOSKeyChain,&QRadioButton::toggled,[]( bool e ){

		if( e ){

			settings::instance().autoMountBackEnd( bk::osxkeychain ) ;
		}
	} ) ;

	connect( m_ui->rbNone,&QRadioButton::toggled,[](){

		settings::instance().autoMountBackEnd( settings::walletBackEnd() ) ;
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
	settings& s = settings::instance() ;

	m_ui->cbAutoOpenMountPoint->setChecked( s.autoOpenFolderOnMount() ) ;

	m_ui->cbReUseMountPoint->setChecked( s.reUseMountPoint() ) ;

	m_ui->cbAutoCheckForUpdates->setChecked( s.autoCheck() ) ;

	m_ui->cbStartMinimized->setChecked( s.startMinimized() ) ;

	m_ui->cbAutoMountAtStartUp->setChecked( s.autoMountFavoritesOnStartUp() ) ;

	m_ui->cbShowMountDialogWhenAutoMounting->setChecked( s.showMountDialogWhenAutoMounting() ) ;

	m_ui->lineEditFileManager->setText( s.fileManager() ) ;

	m_ui->lineEditExecutableKeySource->setText( s.externalPluginExecutable() ) ;

	m_ui->lineEditAfterMountCommand->setText( s.runCommandOnMount() ) ;

	m_ui->lineEditBeforesUnMount->setText( s.preUnMountCommand() ) ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPointPrefix->setText( s.windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( s.mountPath() ) ;
	}

	this->show() ;
}

void configOptions::HideUI()
{
	m_functions.function_1() ;

	settings& s = settings::instance() ;

	s.setFileManager( m_ui->lineEditFileManager->text() ) ;
	s.setExternalPluginExecutable( m_ui->lineEditExecutableKeySource->text() ) ;
	s.preUnMountCommand( m_ui->lineEditBeforesUnMount->text() ) ;
	s.runCommandOnMount( m_ui->lineEditAfterMountCommand->text() ) ;

	if( utility::platformIsWindows() ){

		s.setWindowsExecutableSearchPath( m_ui->lineEditMountPointPrefix->text() ) ;
	}else{
		s.setDefaultMountPointPrefix( m_ui->lineEditMountPointPrefix->text() ) ;
	}

	this->hide() ;
}

void configOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
