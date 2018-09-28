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

#include "options.h"
#include "dialogmsg.h"
#include "task.hpp"
#include "utility.h"
#include "lxqt_wallet.h"
#include "utility2.h"
#include "plugin.h"
#include "crypto.h"
#include "configfileoption.h"

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
		      utility::volumeList z,
		      std::function< void() > f ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_fileManagerOpen( q ),
	m_autoOpenMountPoint( o ),
	m_create( false ),
	m_secrets( s ),
	m_done( std::move( f ) ),
	m_volumes( std::move( z ) )
{
	m_ui->setupUi( this ) ;

	utility::setParent( parent,&m_parentWidget,this ) ;
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
	m_cancel( std::move( p ) )
{
	m_ui->setupUi( this ) ;

	utility::setParent( parent,&m_parentWidget,this ) ;
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

	m_reUseMountPoint = utility::reUseMountPoint() ;

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

	if( m_create ){

		connect( m_ui->lineEditMountPoint,SIGNAL( textChanged( QString ) ),
			 this,SLOT( textChanged( QString ) ) ) ;

		m_ui->pbOpen->setText( tr( "&Create" ) ) ;

		m_ui->label_2->setText( tr( "Volume Name" ) ) ;

		if( utility::platformIsWindows() ){

			//m_ui->lineEditFolderPath->setText( "Z:" ) ;
			//utility::setWindowsMountPointOptions( this,m_ui->lineEditFolderPath,m_ui->pbOpenFolderPath ) ;
			m_ui->lineEditFolderPath->setText( utility::homePath() + "/Desktop/" ) ;
			//m_ui->pbOpenFolderPath->setIcon( QIcon( ":/folder.png" ) ) ;
		}else{
			m_ui->lineEditFolderPath->setText( utility::homePath() + "/" ) ;
		}

		m_ui->lineEditMountPoint->setFocus() ;

		m_ui->lineEditMountPoint->setText( m_path ) ;

		this->windowSetTitle() ;
	}else{
		this->windowSetTitle( tr( "Unlocking \"%1\"" ).arg( m_path ) ) ;

		m_ui->label_2->setText( tr( "Mount Path" ) ) ;

		m_ui->pbMountPoint->setIcon( QIcon( ":/folder.png" ) ) ;

		m_ui->lineEditKey->setFocus() ;
	}

	QIcon folderIcon( ":/folder.png" ) ;

	m_ui->pbOpenFolderPath->setIcon( folderIcon ) ;

	m_ui->pbSetKeyKeyFile->setIcon( folderIcon ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->checkBoxOpenReadOnly->setChecked( utility::getOpenVolumeReadOnlyOption() ) ;

	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;

	m_ui->cbKeyType->addItem( tr( "Key" ) ) ;
	m_ui->cbKeyType->addItem( tr( "KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "Key+KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "HMAC+KeyFile" ) ) ;
	m_ui->cbKeyType->addItem( tr( "ExternalExecutable" ) ) ;

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

	m_ui->checkBoxVisibleKey->setEnabled( utility::enableRevealingPasswords() ) ;
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

	m_configFile   = e.configFilePath() ;
	m_idleTimeOut  = e.idleTimeOut() ;
	m_mountOptions = e.mountOptions() ;
	m_working      = false ;

	m_ui->lineEditKey->setText( key ) ;

	this->setUIVisible( true ) ;

	this->SetUISetKey( false ) ;

	m_ui->cbKeyType->setCurrentIndex( keyDialog::Key ) ;

	if( m_create ){

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
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

				return utility::freeWindowsDriveLetter() ;
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

					return utility::mountPath( m_path.split( "/" ).last() ) ;
				}else{
					return utility::mountPath( m.split( "/" ).last() ) ;
				}
			}else{
				return utility::mountPath( [ &m,this ](){

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

		if( utility::equalsAtleastOne( m_exe,"Securefs","Cryfs","Gocryptfs","Ecryptfs","Encfs" ) ){

			m_ui->pbOptions->setEnabled( true ) ;
		}else{
			m_ui->pbOptions->setEnabled( false ) ;
		}

		m_ui->label_3->setVisible( true ) ;

		m_ui->checkBoxOpenReadOnly->setVisible( false ) ;

		m_ui->lineEditFolderPath->setVisible( true ) ;

		m_ui->pbOpenFolderPath->setVisible( true ) ;

		m_ui->pbMountPoint->setVisible( false ) ;

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

		if( m_exe == "Ecryptfs" ){

			this->hide() ;

			ecryptfscreateoptions::instance( this,[ this ]( const QStringList& e ){

				utility2::stringListToStrings( e,m_createOptions,m_configFile ) ;

				this->ShowUI() ;
			} ) ;

		}else if( m_exe == "Gocryptfs" ){

			this->hide() ;

			gocryptfscreateoptions::instance( m_parentWidget,[ this ]( const QStringList& e ){

				utility2::stringListToStrings( e,m_createOptions,m_configFile ) ;

				this->ShowUI() ;
			} ) ;

		}else if( m_exe == "Securefs" ){

			this->hide() ;

			securefscreateoptions::instance( m_parentWidget,[ this ]( const QStringList& e ){

				utility2::stringListToStrings( e,m_createOptions,m_configFile ) ;

				this->ShowUI() ;
			} ) ;

		}else if( m_exe == "Cryfs" ){

			this->hide() ;

			cryfscreateoptions::instance( m_parentWidget,[ this ]( const QStringList& e ){

				utility2::stringListToStrings( e,m_createOptions,m_configFile ) ;

				this->ShowUI() ;
			} ) ;

		}else if( m_exe == "Encfs" ){

			this->hide() ;

			encfscreateoptions::instance( m_parentWidget,[ this ]( const QStringList& e ){

				utility2::stringListToStrings( e,m_createOptions,m_configFile ) ;

				this->ShowUI() ;
			} ) ;
		}
	}else{
		if( !m_checked ){

			m_checked = true ;

			auto f = utility::readFavorite( m_path ) ;

			m_idleTimeOut  = f.idleTimeOut ;
			m_configFile   = f.configFilePath ;
			m_mountOptions = f.mountOptions ;
		}

		QStringList e{ m_idleTimeOut,m_configFile,m_mountOptions,m_exe } ;

		this->hide() ;

		options::instance( m_parentWidget,m_create,e,[ this ]( const QStringList& e ){

			utility2::stringListToStrings( e,m_idleTimeOut,m_configFile,m_mountOptions ) ;

			if( m_ui->lineEditKey->text().isEmpty() ){

				m_ui->lineEditKey->setFocus() ;
			}else{
				m_ui->pbOpen->setFocus() ;
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

	}else if( m_keyType == keyDialog::keyKeyFile || m_keyType == keyDialog::hmacKeyFile ){

		this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
	}else{
		this->windowSetTitle() ;
	}
}

bool keyDialog::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbCancel() ; } ) ;
}

void keyDialog::cbMountReadOnlyStateChanged( int state )
{
	if( this->upgradingFileSystem() ){

		return ;
	}

	auto e = utility::setOpenVolumeReadOnly( this,state == Qt::Checked ) ;

	m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;
	m_ui->checkBoxOpenReadOnly->setChecked( e ) ;
	m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;

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
	auto e = utility::getExistingDirectory( this,msg,utility::homePath() ) ;

	if( !e.isEmpty() ){

		e = e + "/" + m_ui->lineEditMountPoint->text().split( '/' ).last() ;

		m_ui->lineEditMountPoint->setText( e ) ;
	}
}

void keyDialog::pbFolderPath()
{
	auto msg = tr( "Select A Folder To Create A Mount Point In." ) ;
	auto e = utility::getExistingDirectory( this,msg,utility::homePath() ) ;

	if( !e.isEmpty() ){

		e = e + "/" + m_ui->lineEditFolderPath->text().split( '/' ).last() ;

		m_ui->lineEditFolderPath->setText( e ) ;
	}
}

void keyDialog::enableAll()
{
	m_ui->pbMountPoint->setEnabled( true ) ;
        m_ui->pbOptions->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->pbOpenFolderPath->setEnabled( true ) ;
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbOpen->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->cbKeyType->setEnabled( true ) ;

	auto index = m_ui->cbKeyType->currentIndex() ;

	m_ui->lineEditKey->setEnabled( index == keyDialog::Key ) ;

	auto enable = index == keyDialog::keyfile || index == keyDialog::keyKeyFile ;

	m_ui->pbkeyOption->setEnabled( enable ) ;

	if( utility::enableRevealingPasswords() ){

		m_ui->checkBoxVisibleKey->setEnabled( index == keyDialog::Key ) ;
	}

	if( !utility::platformIsWindows() ){

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
		m_ui->label_3->setVisible( m_create ) ;
		m_ui->checkBoxOpenReadOnly->setVisible( !m_create ) ;
		m_ui->lineEditFolderPath->setVisible( m_create ) ;
		m_ui->pbOpenFolderPath->setVisible( m_create ) ;
	}else{
		m_ui->pbMountPoint->setVisible( e ) ;
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
		auto e = QFileDialog::getOpenFileName( this,msg,utility::homePath() ) ;

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
	auto e = QFileDialog::getOpenFileName( this,msg,utility::homePath() ) ;

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
		}

		if( m_ui->lineEditKey->text().isEmpty() ){

			this->showErrorMessage( tr( "Key Field Is Empty." ) ) ;

			m_ui->lineEditKey->setFocus() ;

			return ;
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

		if( kde || gnome || osx ){

			w = utility::getKey( m_path,m_secrets.walletBk( [ & ](){

				if( wallet == _kwallet() ){

					return LXQt::Wallet::BackEnd::kwallet ;

				}else if( wallet == _gnomeWallet() ){

					return LXQt::Wallet::BackEnd::libsecret ;

				}else if( wallet == _OSXKeyChain() ){

					return LXQt::Wallet::BackEnd::osxkeychain ;
				}else{
					/*
					 * We should not get here.
					 */
					return LXQt::Wallet::BackEnd::internal ;
				}

			}() ).bk() ) ;

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

				this->showErrorMessage( "The Volume Does Not Appear To Have An Entry In The Wallet." ) ;

				this->enableAll() ;

				m_ui->lineEditKey->setEnabled( false ) ;
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
	if( utility::autoOpenFolderOnMount() ){

		utility::Task::exec( m_fileManagerOpen + " " + utility::Task::makePath( m ) ) ;
	}
}

void keyDialog::reportErrorMessage( const siritask::cmdStatus& s )
{
	QString msg ;

	if( s == siritask::status::cryfsMigrateFileSystem ){

		m_ui->checkBoxOpenReadOnly->setText( tr( "Upgrade File System" ) ) ;
	}else{
		m_ui->checkBoxOpenReadOnly->setText( m_checkBoxOriginalText ) ;
	}

	switch( s.status() ){

	case siritask::status::success :

		/*
		 * Should not get here
		 */

	case siritask::status::volumeCreatedSuccessfully :

		msg = tr( "Volume Created Successfully." ) ;
		break ;

	case siritask::status::cryfs :

		msg = tr( "Failed To Unlock A Cryfs Volume.\nWrong Password Entered." ) ;
		break;

	case siritask::status::sshfs :

		msg = tr( "Failed To Connect To The Remote Computer.\nWrong Password Entered." ) ;
		break;

	case siritask::status::encfs :

		msg = tr( "Failed To Unlock An Encfs Volume.\nWrong Password Entered." ) ;
		break;

	case siritask::status::gocryptfs :

		msg = tr( "Failed To Unlock A Gocryptfs Volume.\nWrong Password Entered." ) ;
		break;

	case siritask::status::ecryptfs :

		msg = tr( "Failed To Unlock An Ecryptfs Volume.\nWrong Password Entered." ) ;
		break;

	case siritask::status::ecryptfsIllegalPath :

		msg = tr( "A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit." ) ;
		break;

	case siritask::status::ecrypfsBadExePermissions :

		msg = tr( "This Backend Requires Root's Privileges And An attempt To Acquire Them Has Failed." ) ;
		break;

	case siritask::status::securefs :

		msg = tr( "Failed To Unlock A Securefs Volume.\nWrong Password Entered." ) ;
		break;

	case siritask::status::sshfsNotFound :

		msg = tr( "Failed To Complete The Request.\nSshfs Executable Could Not Be Found." ) ;
		break;

	case siritask::status::backEndDoesNotSupportCustomConfigPath :

		msg = tr( "Backend Does Not Support Custom Configuration File Path." ) ;
		break;

	case siritask::status::cryfsNotFound :

		msg = tr( "Failed To Complete The Request.\nCryfs Executable Could Not Be Found." ) ;
		break;

	case siritask::status::cryfsMigrateFileSystem :

		msg = tr( "This Volume Of Cryfs Needs To Be Upgraded To Work With The Version Of Cryfs You Are Using.\n\nThe Upgrade is IRREVERSIBLE And The Volume Will No Longer Work With Older Versions of Cryfs.\n\nTo Do The Upgrade, Check The \"Upgrade File System\" Option And Unlock The Volume Again." ) ;

		break;

	case siritask::status::encfsNotFound :

		msg = tr( "Failed To Complete The Request.\nEncfs Executable Could Not Be Found." ) ;
		break;

	case siritask::status::ecryptfs_simpleNotFound :

		msg = tr( "Failed To Complete The Request.\nEcryptfs-simple Executable Could Not Be Found." ) ;
		break;

	case siritask::status::gocryptfsNotFound :

		msg = tr( "Failed To Complete The Request.\nGocryptfs Executable Could Not Be Found." ) ;
		break;

	case siritask::status::securefsNotFound :

		msg = tr( "Failed To Complete The Request.\nSecurefs Executable Could Not Be Found." ) ;
		break;

	case siritask::status::failedToCreateMountPoint :

		msg = tr( "Failed To Create Mount Point." ) ;
		break;

	case siritask::status::failedToLoadWinfsp :

		msg = tr( "Backend Could Not Load WinFsp. Please Make Sure You Have WinFsp Properly Installed" ) ;
		break;

	case siritask::status::unknown :

		msg = tr( "Failed To Unlock The Volume.\nNot Supported Volume Encountered." ) ;
		break;

	case siritask::status::backendFail :
	default:
		msg = [ & ](){

			auto e = tr( "Failed To Complete The Task And Below Log was Generated By The Backend.\n" ) ;

			return e + "\n----------------------------------------\n" + s.msg() ;
		}() ;
	}

	this->showErrorMessage( { s,msg } ) ;
}

void keyDialog::showErrorMessage( const QString& e )
{
	this->setUIVisible( false ) ;

	m_ui->labelMsg->setText( e ) ;

	m_ui->pbOK->setFocus() ;
}

void keyDialog::showErrorMessage( const siritask::cmdStatus& e )
{
	if( e == siritask::status::backendFail ){

		this->setUIVisible( false ) ;

		m_ui->textEdit->setVisible( true ) ;
		m_ui->labelMsg->setVisible( false ) ;

		m_ui->textEdit->setText( e.msg() ) ;
	}else{
		this->showErrorMessage( e.msg() ) ;
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

	if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

		m_ui->checkBoxVisibleKey->setVisible( true ) ;
		m_ui->pbkeyOption->setVisible( false ) ;
	}else{
		m_ui->checkBoxVisibleKey->setVisible( false ) ;
		m_ui->pbkeyOption->setVisible( true ) ;
	}
}

void keyDialog::encryptedFolderCreate()
{
	auto path = m_ui->lineEditFolderPath->text() ;

	auto m = path.split( '/' ).last() ;

	if( utility::pathExists( path ) ){

		this->showErrorMessage( tr( "Encrypted Folder Path Is Already Taken." ) ) ;

		return this->enableAll() ;
	}

	if( utility::platformIsWindows() ){

		m = utility::freeWindowsDriveLetter() ;

		if( utility::pathExists( m ) ){

			this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

			return this->enableAll() ;
		}
	}else{
		m = utility::mountPath( utility::mountPathPostFix( m ) ) ;

		if( utility::pathExists( m ) && !m_reUseMountPoint ){

			this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

			return this->enableAll() ;
		}
	}

	if( m_key.isEmpty() ){

		this->showErrorMessage( tr( "Atleast One Required Field Is Empty." ) ) ;

		this->enableAll() ;

		m_ui->lineEditKey->setFocus() ;

		return ;
	}

	if( m_exe == "Ecryptfs" ){

		if( m_createOptions.isEmpty() ){

			m_createOptions = "-o " + ecryptfscreateoptions::defaultCreateOptions() ;
		}
	}

	m_working = true ;

	siritask::options s{ path,m,m_key,m_idleTimeOut,m_configFile,
			     m_exe.toLower(),false,m_mountOptions,m_createOptions } ;

	auto e = siritask::encryptedFolderCreate( s ).await() ;

	m_working = false ;

	if( e == siritask::status::success ){

		this->openMountPoint( m ) ;
		this->HideUI() ;
	}else{
		this->reportErrorMessage( e ) ;

		if( e == siritask::status::volumeCreatedSuccessfully ){

			m_closeGUI = true ;
		}else{
			if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

				m_ui->lineEditKey->clear() ;
			}

			this->enableAll() ;

			m_ui->lineEditKey->setFocus() ;
		}
	}
}

void keyDialog::pbSetKeyKeyFile()
{
	m_ui->lineEditSetKeyKeyFile->setText( QFileDialog::getOpenFileName( this,tr( "KeyFile" ),utility::homePath(),nullptr ) ) ;
}

void keyDialog::pbSetKey()
{
	this->setKeyEnabled( false ) ;

	auto keyFile    = m_ui->lineEditSetKeyKeyFile->text() ;
	auto passphrase = m_ui->lineEditSetKeyPassword->text() ;

	Task::run( [ = ](){

		if( m_hmac ){

			return crypto::hmac_key( keyFile,passphrase ) ;
		}else{
			auto exe = utility::externalPluginExecutable() ;

			if( exe.isEmpty() ){

				return QByteArray() ;
			}else{
				exe = exe + " " + utility::Task::makePath( keyFile ) ;

				auto env = utility::systemEnvironment() ;

				env.insert( "LANG","C" ) ;

				env.insert( "PATH",utility::executableSearchPaths( env.value( "PATH" ) ) ) ;

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

		this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;

		return this->enableAll() ;
	}

	if( !m_path.startsWith( "sshfs " ) ){

		if( !utility::pathExists( m_path ) ){

			this->showErrorMessage( tr( "Encrypted Folder Appear To Not Be Present." ) ) ;

			return this->enableAll() ;
		}
	}

	if( !m_path.startsWith( "sshfs " ) ){

		if( m_key.isEmpty() ){

			this->showErrorMessage( tr( "Atleast One Required Field Is Empty." ) ) ;

			this->enableAll() ;

			m_ui->lineEditKey->setFocus() ;

			return ;
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

	siritask::options s{ m_path,m,m_key,m_idleTimeOut,m_configFile,m_exe,ro,m_mountOptions,QString() } ;

	auto e = siritask::encryptedFolderMount( s ).await() ;

	m_working = false ;

	if( e == siritask::status::success ){

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

	if( keyType == keyDialog::Key ){

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
	if( m_ui->cbKeyType->currentIndex() == keyDialog::Key ){

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
	e.remove( '&' ) ;

	auto _showVisibleKeyOption = [ this ]( bool e ){

		bool s = utility::enableRevealingPasswords() ;
		m_ui->checkBoxVisibleKey->setEnabled( e && s ) ;
		m_ui->checkBoxVisibleKey->setChecked( false ) ;
		m_ui->checkBoxVisibleKey->setVisible( e ) ;
		m_ui->pbkeyOption->setVisible( !e ) ;
	} ;

	if( e == tr( "Key" ).remove( '&' ) ){

		this->key() ;

		_showVisibleKeyOption( true ) ;

	}else if( e == tr( "KeyFile" ).remove( '&' ) ){

		_showVisibleKeyOption( false ) ;

		this->keyFile() ;

		this->KeyFile() ;

	}else if( e == tr( "Key+KeyFile" ).remove( '&' ) || e == tr( "ExternalExecutable" ).remove( '&' ) ){

		QString s ;

		if( e == tr( "Key+KeyFile" ).remove( '&' ) ){

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

	}else if( e == tr( "HMAC+KeyFile" ).remove( '&' ) ){

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
