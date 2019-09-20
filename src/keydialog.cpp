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

#include "keydialog.h"
#include "ui_keydialog.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QTableWidget>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include "win.h"
#include "options.h"
#include "dialogmsg.h"
#include "task.hpp"
#include "utility.h"
#include "lxqt_wallet.h"
#include "utility2.h"
#include "plugin.h"
#include "crypto.h"
#include "configfileoption.h"
#include "walletconfig.h"

static QString _kwallet()
{
	return QObject::tr( "Kde Wallet" ) ;
}

static QString _internalWallet()
{
	return QObject::tr( "Internal Wallet" ) ;
}

static QString _gnomeWallet()
{
	return QObject::tr( "Gnome Wallet" ) ;
}

static QString _OSXKeyChain()
{
	return QObject::tr( "OSX KeyChain" ) ;
}

keyDialog::keyDialog( QWidget * parent,
		      secrets& s,
		      bool o,
		      const QString& q,
		      favorites::volumeList z,
		      std::function< void() > f ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_fileManagerOpen( q ),
	m_autoOpenMountPoint( o ),
	m_create( false ),
	m_secrets( s ),
	m_settings( settings::instance() ),
	m_done( std::move( f ) ),
	m_volumes( std::move( z ) ),
	m_walletKey( s )
{
	m_ui->setupUi( this ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;
	utility::setWindowOptions( this ) ;

	this->setFont( parent->font() ) ;

	this->setUpInitUI() ;

	if( this->mountedAll() ){

		this->HideUI() ;
	}else{
		this->setVolumeToUnlock() ;

		this->ShowUI() ;
	}
}

void keyDialog::unlockVolume()
{
	if( this->mountedAll() ){

		this->HideUI() ;
	}else{
		this->setVolumeToUnlock() ;
	}
}

keyDialog::keyDialog( QWidget * parent,
		      secrets& s,
		      const volumeInfo& e,
		      std::function< void() > p,
		      bool o,
		      const QString& q,
		      const QString& exe,
		      const QByteArray& key ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_exe( exe ),
	m_fileManagerOpen( q ),
	m_autoOpenMountPoint( o ),
	m_create( e.isNotValid() ),
	m_secrets( s ),
	m_settings( settings::instance() ),
	m_cancel( std::move( p ) ),
	m_walletKey( s )
{
	m_ui->setupUi( this ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;
	utility::setWindowOptions( this ) ;

	this->setFont( parent->font() ) ;

	this->setUpInitUI() ;

	this->setUpVolumeProperties( e,key ) ;

	if( m_create ){

		m_ui->lineEditMountPoint->setText( QString() ) ;
	}

	this->ShowUI() ;
}

void keyDialog::setUpInitUI()
{
	this->setUIVisible( true ) ;

	if( utility::platformIsWindows() ){

		/*
		 * We are diabling this functionality on windows for now
		 * its currently not possible to open a volume in read only mode.
		 */
		m_ui->checkBoxOpenReadOnly->setChecked( false ) ;
		m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	}

	m_reUseMountPoint = m_settings.reUseMountPoint() ;

	m_checkBoxOriginalText = m_ui->checkBoxOpenReadOnly->text() ;

	connect( m_ui->pbCancel,SIGNAL( clicked() ),
		 this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpen,SIGNAL( clicked() ),
		 this,SLOT( pbOpen() ) ) ;
	connect( m_ui->pbkeyOption,SIGNAL( clicked() ),
		 this,SLOT( pbkeyOption() ) ) ;
	connect( m_ui->pbOpenFolderPath,SIGNAL( clicked() ),
		 this,SLOT( pbFolderPath() ) ) ;
	connect( m_ui->pbMountPoint,SIGNAL( clicked() ),
		 this,SLOT( pbMountPointPath() ) ) ;
	connect( m_ui->checkBoxOpenReadOnly,SIGNAL( stateChanged( int ) ),
		 this,SLOT( cbMountReadOnlyStateChanged( int ) ) ) ;
	connect( m_ui->cbKeyType,SIGNAL( currentIndexChanged( QString ) ),
		 this,SLOT( cbActicated( QString ) ) ) ;
	connect( m_ui->pbOK,SIGNAL( clicked( bool ) ),
		 this,SLOT( pbOK() ) ) ;
	connect( m_ui->checkBoxVisibleKey,SIGNAL( stateChanged( int ) ),
		 this,SLOT( cbVisibleKeyStateChanged( int ) ) ) ;
	connect( m_ui->pbOptions,SIGNAL( clicked() ),
		 this,SLOT( pbOptions() ) ) ;
	connect( m_ui->pbSetKeyKeyFile,SIGNAL( clicked() ),
		 this,SLOT( pbSetKeyKeyFile() ) ) ;
	connect( m_ui->pbSetKey,SIGNAL( clicked() ),
		 this,SLOT( pbSetKey() ) ) ;
	connect( m_ui->pbSetKeyCancel,SIGNAL( clicked() ),
		 this,SLOT( pbSetKeyCancel() ) ) ;

	connect( m_ui->pbMountPoint_1,&QPushButton::clicked,[ this ](){

		auto msg = tr( "Select A Folder To Create A Mount Point In." ) ;
		auto e = utility::getExistingDirectory( this,msg,m_settings.homePath() ) ;

		if( !e.isEmpty() ){

			while( e.endsWith( "/" ) ){

				e.truncate( e.length() - 1 ) ;
			}

			m_ui->lineEditMountPoint->setText( e ) ;
		}
	} ) ;

	if( m_create ){

		m_ui->pbMountPoint_1->setVisible( false ) ;

		connect( m_ui->lineEditMountPoint,SIGNAL( textChanged( QString ) ),
			 this,SLOT( textChanged( QString ) ) ) ;

		m_ui->pbOpen->setText( tr( "&Create" ) ) ;

		m_ui->label_2->setText( tr( "Volume Name" ) ) ;

		if( utility::platformIsWindows() ){

			//m_ui->lineEditFolderPath->setText( "Z:" ) ;
			//utility::setWindowsMountPointOptions( this,m_ui->lineEditFolderPath,m_ui->pbOpenFolderPath ) ;
			m_ui->lineEditFolderPath->setText( m_settings.homePath() + "/Desktop/" ) ;
			//m_ui->pbOpenFolderPath->setIcon( QIcon( ":/folder.png" ) ) ;
		}else{
			m_ui->lineEditFolderPath->setText( m_settings.homePath() + "/" ) ;
		}

		m_ui->lineEditMountPoint->setFocus() ;

		m_ui->lineEditMountPoint->setText( m_path ) ;

		this->windowSetTitle() ;
	}else{
		this->windowSetTitle( tr( "Unlocking \"%1\"" ).arg( m_path ) ) ;

		m_ui->pbMountPoint_1->setVisible( utility::platformIsWindows() ) ;

		m_ui->label_2->setText( tr( "Mount Path" ) ) ;

		m_ui->pbMountPoint->setIcon( QIcon( ":/folder.png" ) ) ;

		m_ui->pbMountPoint_1->setIcon( QIcon( ":/folder.png" ) ) ;

		m_ui->lineEditKey->setFocus() ;
	}

	QIcon folderIcon( ":/folder.png" ) ;

	m_ui->pbOpenFolderPath->setIcon( folderIcon ) ;

	m_ui->pbSetKeyKeyFile->setIcon( folderIcon ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->checkBoxOpenReadOnly->setChecked( m_settings.getOpenVolumeReadOnlyOption() ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	m_ui->cbKeyType->addItem( tr( "Key" ) ) ;
	m_ui->cbKeyType->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "HMAC+KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "ExternalExecutable" ) ) ;
	m_ui->cbKeyType->addItem( tr( "YubiKey Challenge/Response" ) ) ;

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::internal ) ){

		m_ui->cbKeyType->addItem( _internalWallet() ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

		m_ui->cbKeyType->addItem( _gnomeWallet() ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

		m_ui->cbKeyType->addItem( _kwallet() ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::osxkeychain ) ){

		m_ui->cbKeyType->addItem( _OSXKeyChain() ) ;
	}

	if( m_create ){

		if( m_keyStrength ){

			connect( m_ui->lineEditKey,SIGNAL( textChanged( QString ) ),
				 this,SLOT( passWordTextChanged( QString ) ) ) ;
		}

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
		m_ui->lineEditKey->setFocus() ;
	}

	this->installEventFilter( this ) ;

	if( !m_key.isEmpty() ){

		m_ui->lineEditKey->setText( m_key ) ;
		m_ui->pbOpen->setFocus() ;
	}

	m_ui->checkBoxVisibleKey->setToolTip( tr( "Check This Box To Make Password Visible" ) ) ;

	m_ui->checkBoxVisibleKey->setEnabled( m_settings.enableRevealingPasswords() ) ;
}

void keyDialog::setVolumeToUnlock()
{
	const auto& m = m_volumes[ m_counter ] ;

	m_counter++ ;

	this->setUpVolumeProperties( m.first,m.second ) ;

	auto a = QString::number( m_counter ) ;
	auto b = QString::number( m_volumes.size() ) ;

	if( m_volumes.size() > 1 ){

		this->windowSetTitle( tr( "(%1/%2) Unlocking \"%3\"" ).arg( a,b,m_path ) ) ;
	}
}

void keyDialog::setUpVolumeProperties( const volumeInfo& e,const QByteArray& key )
{
	m_path         = e.volumePath() ;
	m_reverseMode  = e.reverseMode() ;
	m_configFile   = e.configFilePath() ;
	m_idleTimeOut  = e.idleTimeOut() ;
	m_mountOptions = e.mountOptions() ;
	m_working      = false ;

	m_favoriteReadOnly = e.mountReadOnly() ;

	if( m_favoriteReadOnly.defined() ){

		m_ui->checkBoxOpenReadOnly->setChecked( m_favoriteReadOnly.True() ) ;
	}else{
		if( utility::platformIsWindows() ){

			m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
		}else{
			m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;
			m_ui->checkBoxOpenReadOnly->setChecked( m_settings.getOpenVolumeReadOnlyOption() ) ;
		}
	}

	m_ui->lineEditKey->setText( key ) ;

	this->setUIVisible( true ) ;

	this->SetUISetKey( false ) ;

	m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;

	if( m_create ){

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
		const auto& engine = siritask::mountEngine( m_path,m_configFile ).engine ;

		m_engineName = engine.name() ;

		auto m = m_ui->pbOptions->menu() ;

		if( m ){

			m->deleteLater() ;
		}

		m = new QMenu( this ) ;

		auto _addAction = [ & ]( QString e,const QString& s,bool a ){

			auto ac = m->addAction( e.replace( 0,1,e.at( 0 ).toUpper() ) ) ;
			ac->setObjectName( s ) ;
			ac->setEnabled( a ) ;
			m->addAction( ac ) ;
		} ;

		_addAction( tr( "Select Volume Type" ),"Select Volume Type",false ) ;

		m->addSeparator() ;

		if( engine.known() ){

			_addAction( m_engineName,m_engineName,false ) ;
		}else{
			auto s = engines::instance().enginesWithNoConfigFile() ;

			if( s.isEmpty() ){

				// ????
			}else {
				for( const auto& it : s ){

					_addAction( it,it,true ) ;
				}
			}
		}

		m->addSeparator() ;

		_addAction( tr( "Options" ),"Options",true ) ;

		connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

			if( ac->objectName() == "Options" ){

				this->pbOptions() ;
			}else{
				m_engineName = ac->objectName() ;
			}
		} ) ;

		m_ui->pbOptions->setMenu( m ) ;

		if( key.isEmpty() ){

			m_ui->lineEditKey->setFocus() ;
		}else{
			m_ui->pbOpen->setFocus() ;
		}
	}

	m_ui->lineEditMountPoint->setText( [ & ]()->QString{

		auto m = e.mountPoint() ;

		if( utility::platformIsWindows() ){

			utility::setWindowsMountPointOptions( this,
							      m_ui->lineEditMountPoint,
							      m_ui->pbMountPoint ) ;

			if( m.isEmpty() ){

				const auto& e = siritask::mountEngine( m_path,m_configFile ).engine ;

				if( m_settings.windowsUseMountPointPath( e.name() ) ){

					auto mm = m_settings.windowsMountPointPath() ;

					utility::createFolder( mm ) ;

					return mm + utility::split( m_path,'/' ).last() ;
				}else{
					return utility::freeWindowsDriveLetter() ;
				}
			}else{
				return m ;
			}
		}

		if( m.startsWith( "/" ) ){

			if( m_reUseMountPoint ){

				return m ;
			}else{
				auto y = m ;
				auto r = y.lastIndexOf( '/' ) ;

				if( r != -1 ){

					y.truncate( r ) ;
				}

				return y + "/" + utility::mountPathPostFix( m,m.split( '/' ).last() ) ;
			}
		}else{
			if( m_reUseMountPoint ){

				if( m.isEmpty() ){

					return m_settings.mountPath( m_path.split( "/" ).last() ) ;
				}else{
					return m_settings.mountPath( m.split( "/" ).last() ) ;
				}
			}else{
				return m_settings.mountPath( [ &m,this ](){

					if( m.isEmpty() ){

						return utility::mountPathPostFix( m_path.split( "/" ).last() ) ;
					}else{
						return utility::mountPathPostFix( m ) ;
					}
				}() ) ;
			}
		}
	}() ) ;
}

void keyDialog::setDefaultUI()
{
	if( m_create ){

		auto e = engines::instance().getByName( m_exe ).hasGUICreateOptions() ;

		m_ui->pbOptions->setEnabled( e ) ;

		m_ui->label_3->setVisible( true ) ;

		m_ui->checkBoxOpenReadOnly->setVisible( false ) ;

		m_ui->lineEditFolderPath->setVisible( true ) ;

		m_ui->pbOpenFolderPath->setVisible( true ) ;

		m_ui->pbMountPoint->setVisible( false ) ;

		m_ui->pbMountPoint_1->setVisible( false ) ;

		m_ui->lineEditFolderPath->setEnabled( false ) ;

		m_ui->pbkeyOption->setVisible( false ) ;
	}else{
		this->windowSetTitle( tr( "Unlocking \"%1\"" ).arg( m_path ) ) ;

		m_ui->label_3->setVisible( false ) ;

		m_ui->lineEditMountPoint->setEnabled( true ) ;

		m_ui->lineEditFolderPath->setVisible( false ) ;

		m_ui->checkBoxOpenReadOnly->setVisible( true ) ;

		m_ui->pbOpenFolderPath->setVisible( false ) ;
	}

	m_ui->pbOK->setVisible( false ) ;
	m_ui->pbOpen->setVisible( true ) ;
	m_ui->checkBoxVisibleKey->setVisible( true ) ;
	m_ui->pbkeyOption->setVisible( false ) ;
	m_ui->textEdit->setVisible( false ) ;
}

void keyDialog::windowSetTitle( const QString& s )
{
	if( s.isEmpty() ){

		auto f = tr( "Create A New \"%1\" Volume" ).arg( m_exe ) ;

		if( this->windowTitle() != f ){

			this->setWindowTitle( f ) ;
		}
	}else{
		this->setWindowTitle( s ) ;
	}
}

void keyDialog::pbOptions()
{
	if( m_create ){

		auto& e = engines::instance().getByName( m_exe ) ;

		this->hide() ;

		e.GUICreateOptionsinstance( m_parentWidget,[ this ]( const engines::engine::Options& e ){

			if( e.success ){

				m_reverseMode = e.reverseMode ;

				utility2::stringListToStrings( e.options,m_createOptions,m_configFile ) ;
			}

			this->ShowUI() ;
		} ) ;
	}else{
		if( !m_checked ){

			m_checked = true ;

			auto f = favorites::instance().readFavorite( m_path ) ;

			if( f.has_value() ){

				const auto& e = f.value() ;
				m_idleTimeOut  = e.idleTimeOut ;
				m_configFile   = e.configFilePath ;
				m_mountOptions = e.mountOptions ;
				m_reverseMode  = e.reverseMode ;
			}
		}

		options::Options e{ { m_idleTimeOut,m_configFile,m_mountOptions,m_exe },m_reverseMode } ;

		this->hide() ;

		options::instance( m_parentWidget,m_create,e,[ this ]( const options::Options& e ){

			if( e.success ){

				m_reverseMode = e.reverseMode ;

				utility2::stringListToStrings( e.options,m_idleTimeOut,m_configFile,m_mountOptions ) ;

				if( m_ui->lineEditKey->text().isEmpty() ){

					m_ui->lineEditKey->setFocus() ;
				}else{
					m_ui->pbOpen->setFocus() ;
				}
			}

			this->ShowUI() ;
		} ) ;
	}
}

void keyDialog::passWordTextChanged( QString e )
{
	if( m_keyType == keyDialog::Key ){

		int r = m_keyStrength.quality( e ) ;

		if( r < 0 ){

			this->setWindowTitle( tr( "Passphrase Quality: 0%" ) ) ;
		}else{
			this->setWindowTitle( tr( "Passphrase Quality: %1%" ).arg( QString::number( r ) ) ) ;
		}

	}else if( m_keyType == keyDialog::keyKeyFile ||
		  m_keyType == keyDialog::hmacKeyFile ||
		  m_keyType == keyDialog::yubikey ){

		this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
	}else{
		this->windowSetTitle() ;
	}
}

bool keyDialog::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbCancel() ; } ) ;
}

bool keyDialog::keySelected( int e )
{
	return e == keyDialog::keyType::Key || e == keyDialog::keyType::yubikey ;
}

void keyDialog::cbMountReadOnlyStateChanged( int state )
{
	if( this->upgradingFileSystem() ){

		return ;
	}

	if( m_favoriteReadOnly.defined() ){

		m_ui->checkBoxOpenReadOnly->setChecked( state == Qt::Checked ) ;
		return ;
	}

	auto e = m_settings.setOpenVolumeReadOnly( this,state == Qt::Checked ) ;

	m_ui->checkBoxOpenReadOnly->setChecked( e ) ;

	if( m_ui->lineEditKey->text().isEmpty() ){

		m_ui->lineEditKey->setFocus() ;

	}else if( m_ui->lineEditMountPoint->text().isEmpty() ){

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
		m_ui->pbOpen->setFocus() ;
	}
}

void keyDialog::textChanged( QString e )
{
	e.remove( '/' ) ;

	m_ui->lineEditMountPoint->setText( e ) ;

	auto r =  m_ui->lineEditFolderPath->text() ;

	auto l = r.lastIndexOf( '/' ) ;

	if( l != -1 ){

		r.truncate( l + 1 ) ;

		m_ui->lineEditFolderPath->setText( r + e ) ;
	}
}

void keyDialog::pbMountPointPath()
{
	auto msg = tr( "Select A Folder To Create A Mount Point In." ) ;
	auto e = utility::getExistingDirectory( this,msg,m_settings.homePath() ) ;

	if( !e.isEmpty() ){

		e = e + "/" + m_ui->lineEditMountPoint->text().split( '/' ).last() ;

		m_ui->lineEditMountPoint->setText( e ) ;
	}
}

void keyDialog::pbFolderPath()
{
	auto msg = tr( "Select A Folder To Create A Mount Point In." ) ;
	auto e = utility::getExistingDirectory( this,msg,m_settings.homePath() ) ;

	if( !e.isEmpty() ){

		e = e + "/" + m_ui->lineEditFolderPath->text().split( '/' ).last() ;

		m_ui->lineEditFolderPath->setText( e ) ;
	}
}

void keyDialog::enableAll()
{
	m_ui->pbMountPoint->setEnabled( true ) ;
	m_ui->pbMountPoint_1->setEnabled( true ) ;
        m_ui->pbOptions->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->pbOpenFolderPath->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbOpen->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->cbKeyType->setEnabled( true ) ;

	auto index = m_ui->cbKeyType->currentIndex() ;

	m_ui->lineEditKey->setEnabled( this->keySelected( index ) ) ;

	auto enable = index == keyDialog::keyfile || index == keyDialog::keyKeyFile ;

	m_ui->pbkeyOption->setEnabled( enable ) ;

	if( m_settings.enableRevealingPasswords() ){

		m_ui->checkBoxVisibleKey->setEnabled( this->keySelected( index ) ) ;
	}

	if( utility::platformIsNOTWindows() ){

		m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;
	}

	m_ui->lineEditFolderPath->setEnabled( false ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->lineEditMountPoint->setEnabled( true ) ;
}

void keyDialog::disableAll()
{
	m_ui->checkBoxVisibleKey->setEnabled( false ) ;
	m_ui->pbMountPoint->setEnabled( false ) ;
	m_ui->pbMountPoint_1->setEnabled( false ) ;
	m_ui->cbKeyType->setEnabled( false ) ;
	m_ui->pbOptions->setEnabled( false ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->lineEditMountPoint->setEnabled( false ) ;
	m_ui->pbOpenFolderPath->setEnabled( false ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbOpen->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	m_ui->lineEditFolderPath->setEnabled( false ) ;
}

void keyDialog::setUIVisible( bool e )
{
	m_ui->pbOK->setVisible( !e ) ;
	m_ui->labelMsg->setVisible( !e ) ;
	m_ui->cbKeyType->setVisible( e ) ;
	m_ui->pbOptions->setVisible( e ) ;
	m_ui->pbkeyOption->setVisible( e ) ;
	m_ui->label_2->setVisible( e ) ;
	m_ui->lineEditMountPoint->setVisible( e ) ;
	m_ui->lineEditKey->setVisible( e ) ;
	m_ui->pbCancel->setVisible( e ) ;
	m_ui->pbOpen->setVisible( e ) ;
	m_ui->label->setVisible( e ) ;
	m_ui->checkBoxVisibleKey->setVisible( e ) ;
	m_ui->pbkeyOption->setVisible( e ) ;

	if( e ){

		m_ui->pbMountPoint->setVisible( !m_create ) ;

		if( utility::platformIsWindows() ){

			m_ui->pbMountPoint_1->setVisible( !m_create ) ;
		}

		m_ui->label_3->setVisible( m_create ) ;
		m_ui->checkBoxOpenReadOnly->setVisible( !m_create ) ;
		m_ui->lineEditFolderPath->setVisible( m_create ) ;
		m_ui->pbOpenFolderPath->setVisible( m_create ) ;
	}else{
		m_ui->pbMountPoint->setVisible( e ) ;

		if( utility::platformIsWindows() ){

			m_ui->pbMountPoint_1->setVisible( e ) ;
		}

		m_ui->label_3->setVisible( e ) ;
		m_ui->checkBoxOpenReadOnly->setVisible( e ) ;
		m_ui->lineEditFolderPath->setVisible( e ) ;
		m_ui->pbOpenFolderPath->setVisible( e ) ;
	}
}

void keyDialog::KeyFile()
{
	if( m_ui->cbKeyType->currentIndex() == keyDialog::keyfile ){

		auto msg = tr( "Select A File To Be Used As A Keyfile." ) ;
		auto e = QFileDialog::getOpenFileName( this,msg,m_settings.homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditKey->setText( e ) ;
		}else{
			m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;
		}
	}
}

void keyDialog::pbkeyOption()
{
	auto msg = tr( "Select A File To Be Used As A Keyfile." ) ;
	auto e = QFileDialog::getOpenFileName( this,msg,m_settings.homePath() ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditKey->setText( e ) ;
	}
}

void keyDialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

bool keyDialog::mountedAll()
{
	return m_volumes.size() == m_counter ;
}

void keyDialog::pbOpen()
{
	if( m_create ){

		if( m_ui->lineEditMountPoint->text().isEmpty() ){

			this->showErrorMessage( tr( "Volume Name Field Is Empty." ) ) ;

			m_ui->lineEditMountPoint->setFocus() ;

			return ;
		}else{
			m_path = m_ui->lineEditFolderPath->text() ;
		}
	}

	this->disableAll() ;

	if( m_ui->cbKeyType->currentIndex() > keyDialog::keyKeyFile ){

		utility::wallet w ;

		auto wallet = m_ui->lineEditKey->text() ;

		auto kde      = wallet == _kwallet() ;
		auto gnome    = wallet == _gnomeWallet() ;
		auto internal = wallet == _internalWallet() ;
		auto osx      = wallet == _OSXKeyChain() ;

		/* Figure out which wallet is used. Defaults to 'internal' */
		using bk = LXQt::Wallet::BackEnd ;
		bk bkwallet = LXQt::Wallet::BackEnd::internal ;
		if( wallet == _kwallet() ){
			bkwallet = LXQt::Wallet::BackEnd::kwallet ;
		}else if( wallet == _gnomeWallet() ){
			bkwallet = LXQt::Wallet::BackEnd::libsecret ;
		}else if( wallet == _OSXKeyChain() ){
			bkwallet = LXQt::Wallet::BackEnd::osxkeychain ;
		}

		if( kde || gnome || osx ){

			w = utility::getKey( m_path,m_secrets.walletBk( bkwallet ).bk() ) ;

		}else if( internal ){

			using bk = LXQt::Wallet::BackEnd ;

			w = utility::getKey( m_path,m_secrets.walletBk( bk::internal ).bk(),this ) ;

			if( w.notConfigured ){

				this->showErrorMessage( tr( "Internal Wallet Is Not Configured." ) ) ;
				return this->enableAll() ;
			}
		}else{
			return this->openVolume() ;
		}

		if( w.opened ){

			if( w.key.isEmpty() ){

				m_walletType = wallet ;

				auto s = tr( "Volume Not Found in \"%1\".\n\nSet The Volume Key To Add It To The Wallet Before Mounting." ).arg( wallet ) ;

				this->setKeyInWallet( wallet,s ) ;
			}else{
				m_key = w.key.toLatin1() ;
				this->openVolume() ;
			}
		}else{
			this->enableAll() ;
		}
	}else{
		this->openVolume() ;
	}
}

void keyDialog::openMountPoint( const QString& m )
{
	if( m_settings.autoOpenFolderOnMount() ){

		utility::Task::exec( m_fileManagerOpen + " " + utility::Task::makePath( m ) ) ;
	}
}

void keyDialog::reportErrorMessage( const engines::engine::cmdStatus& s )
{
	if( s == engines::engine::status::cryfsMigrateFileSystem ){

		m_ui->checkBoxOpenReadOnly->setText( tr( "Upgrade File System" ) ) ;
	}else{
		m_ui->checkBoxOpenReadOnly->setText( m_checkBoxOriginalText ) ;
	}	

	this->showErrorMessage( s ) ;
}

void keyDialog::showErrorMessage( const QString& e )
{
	this->setUIVisible( false ) ;

	m_ui->labelMsg->setText( e ) ;

	m_ui->pbOK->setFocus() ;
}

void keyDialog::showErrorMessage( const engines::engine::cmdStatus& e )
{
	if( e == engines::engine::status::backendFail ){

		this->setUIVisible( false ) ;

		m_ui->textEdit->setVisible( true ) ;
		m_ui->labelMsg->setVisible( false ) ;

		m_ui->textEdit->setText( e.toString() ) ;
	}else{
		this->showErrorMessage( e.toString() ) ;
	}
}

void keyDialog::pbOK()
{
	if( m_closeGUI ){

		m_cancel() ;
		return this->HideUI() ;
	}

	m_ui->checkBoxVisibleKey->setChecked( false ) ;

	this->setUIVisible( true ) ;

	m_ui->textEdit->setVisible( false ) ;

	if( this->keySelected( m_ui->cbKeyType->currentIndex() ) ) {

		m_ui->checkBoxVisibleKey->setVisible( true ) ;
		m_ui->pbkeyOption->setVisible( false ) ;
	}else{
		m_ui->checkBoxVisibleKey->setVisible( false ) ;
		m_ui->pbkeyOption->setVisible( true ) ;
	}
}

void keyDialog::encryptedFolderCreate()
{
	utility::raii deleteKey( [ & ](){ m_walletKey.deleteKey() ; } ) ;

	auto path = m_ui->lineEditFolderPath->text() ;

	auto m = utility::split( path,'/' ).last() ;

	if( utility::pathExists( path ) ){

		this->showErrorMessage( tr( "Encrypted Folder Path Is Already Taken." ) ) ;

		return this->enableAll() ;
	}

	if( utility::platformIsWindows() ){

		if( m_settings.windowsUseMountPointPath( m_exe.toLower() ) ){

			m = m_settings.windowsMountPointPath() + m ;

			if( SiriKali::Windows::mountPointTaken( m ) ){

				this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

				return this->enableAll() ;
			}

			if( utility::pathExists( m ) && utility::folderNotEmpty( m ) ){

				this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

				return this->enableAll() ;
			}
		}else{
			m = utility::freeWindowsDriveLetter() ;

			if( utility::pathExists( m ) ){

				this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

				return this->enableAll() ;
			}
		}

	}else{
		m = m_settings.mountPath( utility::mountPathPostFix( m ) ) ;

		if( utility::pathExists( m ) && !m_reUseMountPoint ){

			this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

			return this->enableAll() ;
		}
	}

	m_working = true ;

	engines::engine::options s( path,
				    m,
				    m_key,
				    m_idleTimeOut,
				    m_configFile,
				    m_exe.toLower(),
				    false,
				    m_reverseMode,
				    m_mountOptions,
				    m_createOptions ) ;

	m_cryfsWarning.showCreate( m_exe.toLower() ) ;

	auto e = siritask::encryptedFolderCreate( s ) ;

	m_cryfsWarning.hide() ;

	m_working = false ;

	if( e == engines::engine::status::success ){

		deleteKey.cancel() ;

		this->openMountPoint( m ) ;
		this->HideUI() ;
	}else{
		this->reportErrorMessage( e ) ;

		if( e == engines::engine::status::volumeCreatedSuccessfully ){

			m_closeGUI = true ;
		}else{
			if( this->keySelected( m_ui->cbKeyType->currentIndex() ) ){

				m_ui->lineEditKey->clear() ;
			}

			this->enableAll() ;

			m_ui->lineEditKey->setFocus() ;
		}
	}
}

void keyDialog::pbSetKeyKeyFile()
{
	auto m = m_settings.homePath() ;
	auto a = QFileDialog::getOpenFileName( this,tr( "KeyFile" ),m,nullptr ) ;
	m_ui->lineEditSetKeyKeyFile->setText( a ) ;
}

void keyDialog::setKeyInWallet()
{
	auto _enable_all = [ & ](){

		m_ui->labelSetKeyKeyFile->setEnabled( false ) ;
		m_ui->labelSetKeyPassword->setEnabled( true ) ;
		m_ui->lineEditSetKeyKeyFile->setEnabled( false ) ;
		m_ui->lineEditSetKeyPassword->setEnabled( true ) ;
		m_ui->pbSetKey->setEnabled( true ) ;
		m_ui->pbSetKeyCancel->setEnabled( true ) ;
		m_ui->pbSetKeyKeyFile->setEnabled( false ) ;
		m_ui->labelSetKey->setEnabled( true ) ;
	} ;

	auto passphrase = m_ui->lineEditSetKeyPassword->text() ;

	if( passphrase.isEmpty() ){

		m_ui->labelSetKey->setText( tr( "Volume Key Can Not Be Empty." ) ) ;
		return _enable_all() ;
	}

	auto w = [ & ](){

		if( m_walletType == _kwallet() ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::kwallet ) ;

		}else if( m_walletType == _gnomeWallet() ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::libsecret ) ;

		}else if ( m_walletType == _OSXKeyChain() ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::osxkeychain ) ;
		}else{
			return m_secrets.walletBk( LXQt::Wallet::BackEnd::internal ) ;
		}
	}() ;

	auto _add_key = [ & ]{

		QString id ;

		if( m_create ){

			id = m_ui->lineEditFolderPath->text() ;
		}else{
			id = m_path ;
		}

		if( w->readValue( id ).isEmpty() ){

			if( walletconfig::addKey( w,id,passphrase,"Nil" ) ){

				m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;
				m_ui->lineEditKey->setText( passphrase ) ;
				this->SetUISetKey( false ) ;
				this->setUIVisible( true ) ;
				m_ui->pbkeyOption->setVisible( false ) ;
				this->disableAll() ;

				m_walletKey.set = true ;
				m_walletKey.id = id ;
				m_walletKey.bk = w->backEnd() ;
				m_walletKey.walletName = m_settings.walletName( m_walletKey.bk ) ;
				m_walletKey.appName = m_settings.applicationName() ;

				this->openVolume() ;

				return true ;
			}else{
				m_ui->labelSetKey->setText( tr( "Failed To Add A Volume To The A Wallet." ) ) ;
			}
		}else{
			m_ui->labelSetKey->setText( tr( "Volume Already Exists In The Wallet." ) ) ;
		}

		return false ;
	} ;

	if( w->opened() ){

		if( _add_key() ){

			return ;
		}
	}else{
		w->setImage( QIcon( ":/sirikali" ) ) ;

		auto bk = w->backEnd() ;
		bool s ;

		if( bk == LXQt::Wallet::BackEnd::internal ){

			this->hide() ;

			s = w->open( m_settings.walletName( bk ),m_settings.applicationName() ) ;

			this->show() ;
		}else{
			s = w->open( m_settings.walletName( bk ),m_settings.applicationName() ) ;
		}

		if( s ){

			if( _add_key() ){

				return ;
			}
		}else{
			m_ui->labelSetKey->setText( tr( "Failed To Open Wallet." ) ) ;
		}
	}

	_enable_all() ;
}

void keyDialog::setKeyInWallet( const QString& volumeType,const QString& title )
{
	m_walletType = volumeType ;

	m_ui->labelSetKey->setText( title ) ;

	m_ui->lineEditSetKeyPassword->clear() ;
	m_ui->lineEditSetKeyKeyFile->clear() ;

	m_ui->labelMsg->clear() ;

	this->setUIVisible( false ) ;
	this->SetUISetKey( true ) ;

	m_ui->lineEditSetKeyPassword->setFocus() ;
}

void keyDialog::pbSetKey()
{
	this->setKeyEnabled( false ) ;

	if( !m_walletType.isEmpty() ){

		return this->setKeyInWallet() ;
	}

	auto keyFile    = m_ui->lineEditSetKeyKeyFile->text() ;
	auto passphrase = m_ui->lineEditSetKeyPassword->text() ;

	Task::run( [ = ](){

		if( m_hmac ){

			return crypto::hmac_key( keyFile,passphrase ) ;
		}else{
			auto exe = m_settings.externalPluginExecutable() ;

			if( exe.isEmpty() ){

				return QByteArray() ;
			}else{
				exe = exe + " " + utility::Task::makePath( keyFile ) ;

				const auto& env = utility::systemEnvironment() ;

				return utility::Task( exe,20000,env,passphrase.toLatin1() ).stdOut() ;
			}
		}

	} ).then( [ this ]( const QByteArray& e ){

		this->setKeyEnabled( true ) ;

		if( e.isEmpty() ){

			m_ui->labelSetKey->setText( tr( "Failed To Generate Key." ) ) ;
		}else{
			m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;
			m_ui->lineEditKey->setText( e ) ;
			this->SetUISetKey( false ) ;
			this->setUIVisible( true ) ;
			m_ui->pbkeyOption->setVisible( false ) ;
		}
	} ) ;
}

void keyDialog::pbSetKeyCancel()
{
	this->SetUISetKey( false ) ;
	this->setUIVisible( true ) ;
	this->enableAll() ;
	m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;
}

void keyDialog::setKeyEnabled( bool e )
{
	m_ui->labelSetKeyKeyFile->setEnabled( e ) ;
	m_ui->labelSetKeyPassword->setEnabled( e ) ;
	m_ui->lineEditSetKeyKeyFile->setEnabled( e ) ;
	m_ui->lineEditSetKeyPassword->setEnabled( e ) ;
	m_ui->pbSetKey->setEnabled( e ) ;
	m_ui->pbSetKeyCancel->setEnabled( e ) ;
	m_ui->pbSetKeyKeyFile->setEnabled( e ) ;
	m_ui->labelSetKey->setEnabled( e ) ;
}

void keyDialog::SetUISetKey( bool e )
{
	m_ui->labelSetKeyKeyFile->setVisible( e ) ;
	m_ui->labelSetKeyPassword->setVisible( e ) ;
	m_ui->lineEditSetKeyKeyFile->setVisible( e ) ;
	m_ui->lineEditSetKeyPassword->setVisible( e ) ;
	m_ui->pbSetKey->setVisible( e ) ;
	m_ui->pbSetKeyCancel->setVisible( e ) ;
	m_ui->pbSetKeyKeyFile->setVisible( e ) ;
	m_ui->labelSetKey->setVisible( e ) ;
	m_ui->pbOK->setVisible( e ) ;

	if( e && !m_walletType.isEmpty() ){

		m_ui->labelSetKeyKeyFile->setEnabled( false ) ;
		m_ui->lineEditSetKeyKeyFile->setEnabled( false ) ;
		m_ui->pbSetKeyKeyFile->setEnabled( false ) ;
	}

	if( e ){

		m_ui->pbOK->setVisible( false ) ;
		m_ui->pbOpen->setVisible( false ) ;
	}else{
		this->setDefaultUI() ;
	}
}

void keyDialog::encryptedFolderMount()
{
	auto ro = m_ui->checkBoxOpenReadOnly->isChecked() ;

	auto m = m_ui->lineEditMountPoint->text() ;

	if( m.isEmpty() ){

		this->showErrorMessage( tr( "Atleast One Required Field Is Empty." ) ) ;

		this->enableAll() ;

		m_ui->lineEditMountPoint->setFocus() ;

		return ;
	}

	if( utility::pathExists( m ) && !m_reUseMountPoint ){

		if( utility::platformIsWindows() ){

			m_settings.reUseMountPoint( true ) ;
			m_reUseMountPoint = true ;
		}else{
			this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

			return this->enableAll() ;
		}
	}

	if( utility::platformIsWindows() ){

		if( SiriKali::Windows::mountPointTaken( m ) ){

			this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

			return this->enableAll() ;
		}

		if( !utility::isDriveLetter( m ) && utility::folderNotEmpty( m ) ){

			this->showErrorMessage( tr( "Mount Point Path Is Not Empty." ) ) ;

			return this->enableAll() ;
		}
	}

	if( this->upgradingFileSystem() ){

		if( m_ui->checkBoxOpenReadOnly->isChecked() ){

			if( m_mountOptions.isEmpty() ){

				m_mountOptions = "--allow-filesystem-upgrade" ;
			}else{
				m_mountOptions += ",--allow-filesystem-upgrade" ;
			}
		}
	}

	m_working = true ;

	engines::engine::options s{ m_path,
				    m,
				    m_key,
				    m_idleTimeOut,
				    m_configFile,
				    m_exe,
				    ro,
				    m_reverseMode,
				    m_mountOptions,
				    QString() } ;

	m_cryfsWarning.showUnlock( m_engineName ) ;

	auto e = siritask::encryptedFolderMount( s,false,m_engineName ) ;

	m_cryfsWarning.hide() ;

	m_working = false ;

	if( e == engines::engine::status::success ){

		this->openMountPoint( m ) ;

		this->enableAll() ;
		this->unlockVolume() ;
	}else{
		this->reportErrorMessage( e ) ;

		m_ui->lineEditKey->clear() ;

		this->enableAll() ;

		m_ui->lineEditKey->setFocus() ;
	}
}

bool keyDialog::upgradingFileSystem()
{
	return m_ui->checkBoxOpenReadOnly->text().remove( "&" ) == tr( "Upgrade File System" ).remove( "&" ) ;
}

void keyDialog::openVolume()
{
	auto keyType = m_ui->cbKeyType->currentIndex() ;

	auto _run = [ this ](){

		if( m_create ){

			this->encryptedFolderCreate() ;
		}else{
			this->encryptedFolderMount() ;
		}
	} ;

	if( keyType == keyDialog::yubikey ){

		auto s = m_ui->lineEditKey->text().toLatin1() ;

		if( s.isEmpty() ){

			s = "\n" ;
		}

		auto m = utility::yubiKey( s ) ;

		if( m.has_value() ){

			m_key = m.value() ;

			_run() ;
		}else{
			this->showErrorMessage( tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			return this->enableAll() ;
		}

	}else if( keyType == keyDialog::Key ){

		m_key = m_ui->lineEditKey->text().toLatin1() ;

		_run() ;

	}else if( keyType == keyDialog::keyKeyFile ){

		/*
		 * Should not get here.
		 */

	}else if( keyType == keyDialog::hmacKeyFile ){

		/*
		 * Should not get here.
		 */

	}else if( keyType == keyDialog::keyfile ){

		Task::run( [ this ](){

			return utility::fileContents( m_ui->lineEditKey->text() ) ;

		} ).then( [ this,_run = std::move( _run ) ]( QByteArray key ){

			m_key = std::move( key ) ;

			if( utility::containsAtleastOne( m_key,'\n','\0','\r' ) ){

				this->showErrorMessage( keyDialog::keyFileError() ) ;
				this->enableAll() ;
			}else{
				_run() ;
			}
		} ) ;

	}else if( keyType == keyDialog::Plugin ){

		/*
		 * m_key is already set
		 */
		_run() ;
	}else{
		_run() ;
	}
}

QString keyDialog::keyFileError()
{
	return QObject::tr( "Not Supported KeyFile Encountered Since It Contains AtLeast One Illegal Character('\\n','\\0','\\r').\n\nPlease Use a Hash Of The KeyFile Through \"HMAC+KeyFile\" Option." ) ;
}

void keyDialog::cbVisibleKeyStateChanged( int s )
{
	if( this->keySelected( m_ui->cbKeyType->currentIndex() ) ){

		if( s == Qt::Checked ){

			m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
		}else{
			m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
		}

		m_ui->lineEditKey->setFocus() ;
	}
}

void keyDialog::cbActicated( QString e )
{
	m_walletType.clear() ;

	auto _t = []( QString&& e ){

		e.remove( '&' ) ;

		return std::move( e ) ;
	} ;

	e = _t( std::move( e ) ) ;

	auto _showVisibleKeyOption = [ this ]( bool e ){

		bool s = m_settings.enableRevealingPasswords() ;
		m_ui->checkBoxVisibleKey->setEnabled( e && s ) ;
		m_ui->checkBoxVisibleKey->setChecked( false ) ;
		m_ui->checkBoxVisibleKey->setVisible( e ) ;
		m_ui->pbkeyOption->setVisible( !e ) ;
	} ;

	if( e == _t( tr( "Key" ) ) || e == _t( tr( "YubiKey Challenge/Response" ) ) ){

		this->key() ;

		_showVisibleKeyOption( true ) ;

	}else if( e == _t( tr( "KeyFile" ) ) ){

		_showVisibleKeyOption( false ) ;

		this->keyFile() ;

		this->KeyFile() ;

	}else if( e == _t( tr( "Key+KeyFile" ) ) || e == _t ( tr( "ExternalExecutable" ) ) ){

		QString s ;

		if( e == _t( tr( "Key+KeyFile" ) ) ){

			s = QObject::tr( "Effective Key Is Generated With Below Formula:\n\nkey = hmac_sha256(password,keyfile contents)" ) ;

			m_hmac = true ;
		}else{
			s = QObject::tr( "This plugin delegates key generation to an external application" ) ;

			m_hmac = false ;
		}

		m_ui->labelSetKey->setText( s ) ;

		m_ui->lineEditSetKeyPassword->clear() ;
		m_ui->lineEditSetKeyKeyFile->clear() ;

		m_ui->lineEditSetKeyPassword->setFocus() ;

		m_ui->labelMsg->clear() ;

		this->setUIVisible( false ) ;
		this->SetUISetKey( true ) ;

	}else if( e == _t( tr( "HMAC+KeyFile" ) ) ){

		_showVisibleKeyOption( false ) ;

		auto q = QFileDialog::getOpenFileName( this,tr( "Select A KeyFile" ),QDir::homePath() ) ;

		if( q.isEmpty() ){

			m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;
			m_ui->lineEditKey->setText( QString() ) ;
		}else{
			this->disableAll() ;

			Task::run( [ q = std::move( q ) ](){

				return crypto::hmac_key( q,QString() ) ;

			} ).then( [ this ]( QByteArray key ){

				m_key = std::move( key ) ;

				m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;

				m_ui->lineEditKey->setText( m_key ) ;

				if( m_keyStrength && m_create ){

					this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
				}

				this->enableAll() ;
			} ) ;
		}

	}else if( m_create && utility::equalsAtleastOne( e,
							 _t( _kwallet() ),
							 _t( _gnomeWallet() ),
							 _t( _internalWallet() ),
							 _t( _OSXKeyChain() ) ) ){

		if( m_ui->lineEditMountPoint->text().isEmpty() ){

			this->showErrorMessage( tr( "Volume Name Field Is Empty." ) ) ;
			m_ui->cbKeyType->setCurrentIndex( 0 ) ;
			m_ui->lineEditMountPoint->setFocus() ;
			m_ui->checkBoxVisibleKey->setVisible( false ) ;
			return ;
		}

		auto s = tr( "Create A Volume With Specified Key And Then Add The Key In \n\"%1\"." ).arg( e ) ;

		this->setKeyInWallet( e,s ) ;
	}else{
		this->plugIn() ;

		if( e == _kwallet() ){

			m_ui->lineEditKey->setText( _kwallet() ) ;

		}else if( e == _gnomeWallet() ){

			m_ui->lineEditKey->setText( _gnomeWallet() ) ;

		}else if( e == _internalWallet() ){

			m_ui->lineEditKey->setText( _internalWallet() ) ;

		}else if( e == _OSXKeyChain() ){

			m_ui->lineEditKey->setText( _OSXKeyChain() ) ;
		}

		_showVisibleKeyOption( false ) ;
	}
}

void keyDialog::HMACKeyFile()
{
	m_keyType = keyDialog::hmacKeyFile ;

	m_ui->pbkeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "KeyFile" ) ) ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKey->clear() ;
}

void keyDialog::keyAndKeyFile()
{
	m_keyType = keyDialog::keyKeyFile ;

	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( tr( "Key+KeyFile" ) ) ;
}

void keyDialog::plugIn()
{
	m_keyType = keyDialog::Plugin ;

	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( _internalWallet() ) ;
}

void keyDialog::key()
{
	m_keyType = keyDialog::Key ;

	m_ui->pbkeyOption->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label->setText( tr( "Key" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->checkBoxVisibleKey->setChecked( false ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKey->setFocus() ;
}

void keyDialog::keyFile()
{
	m_keyType = keyDialog::keyfile ;

	m_ui->pbkeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Keyfile path" ) ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
}

void keyDialog::pbCancel()
{
	if( this->mountedAll() ){

		m_cancel() ;

		this->HideUI() ;
	}else{
		this->unlockVolume() ;
	}
}

void keyDialog::ShowUI()
{
	m_cryfsWarning.setWarningLabel( m_ui->cryfsWarning ) ;
	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

void keyDialog::HideUI()
{
	if( !m_working ){

		m_done() ;

		this->hide() ;
		this->deleteLater() ;
	}
}

keyDialog::~keyDialog()
{
	delete m_ui ;
}
