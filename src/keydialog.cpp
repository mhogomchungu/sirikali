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
#include "dialogmsg.h"
#include "task.hpp"
#include "utility.h"
#include "lxqt_wallet.h"
#include "utility2.h"
#include "plugin.h"
#include "crypto.h"
#include "configfileoption.h"
#include "processManager.h"

/*
 *
 * Called when mounting volumes
 *
 */
keyDialog::keyDialog( QWidget * parent,
		      secrets& s,
		      bool o,
		      const QString& q,
		      keyDialog::volumeList z,
		      std::function< void() > f,
		      std::function< void() > g ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_fileManagerOpen( q ),
	m_autoOpenMountPoint( o ),
	m_create( false ),
	m_secrets( s ),
	m_settings( settings::instance() ),
	m_done( std::move( f ) ),
	m_updateVolumeList( std::move( g ) ),
	m_walletKey( s )
{
	m_ui->setupUi( this ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;
	utility::setWindowOptions( this ) ;

	this->setFont( parent->font() ) ;

	this->setUpInitUI() ;

	keyDialog::volumeList bb ;
	keyDialog::volumeList cc ;

	/*
	 * We are sorting in a way that makes all volumes that meet below two criteria to appear first.
	 *
	 * 1. Have auto mount option set.
	 * 2. Do not require password.
	 * 3. Have password.
	 */

	/*
	 * We put first volumes that have auto mount option set and have password or don't need it.
	 */
	for( auto&& it : z ){

		const auto& e = it.volEntry ;

		if( e.favorite().autoMount ){

			if( it.engine->requiresNoPassword() || it.volEntry.favorite().volumeNeedNoPassword ){

				m_volumes.emplace_back( std::move( it ) ) ;

			}else if( !e.password().isEmpty() ){

				m_volumes.emplace_back( std::move( it ) ) ;
			}else{
				bb.emplace_back( std::move( it ) ) ;
			}
		}else{
			bb.emplace_back( std::move( it ) ) ;
		}
	}

	/*
	 * We put second volumes that have password or don't need it.
	 */
	for( auto&& it : bb ){

		const auto& e = it.volEntry ;

		if( it.engine->requiresNoPassword() || e.favorite().volumeNeedNoPassword ){

			m_volumes.emplace_back( std::move( it ) ) ;

		}else if( !e.password().isEmpty() ){

			m_volumes.emplace_back( std::move( it ) ) ;
		}else{
			cc.emplace_back( std::move( it ) ) ;
		}
	}

	for( auto&& it : cc ){

		m_volumes.emplace_back( std::move( it ) ) ;
	}

	if( this->mountedAll() ){

		this->HideUI() ;
	}else{
		this->ShowUI() ;

		this->setVolumeToUnlock() ;
	}
}

void keyDialog::autoMount( const keyDialog::entry& ee )
{
	const auto& e = ee.volEntry.favorite() ;

	if( e.autoMount ){

		if( e.volumeNeedNoPassword ){

			this->openVolume() ;

		}else if( !ee.volEntry.password().isEmpty() ){

			this->openVolume() ;

		}else if( ee.engine->requiresNoPassword() ){

			this->openVolume() ;
		}
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

/*
 *
 * Called when creating volumes
 *
 */
keyDialog::keyDialog( QWidget * parent,secrets& s,
		      std::function< void() > p,
		      std::function< void() > l,
		      bool o,
		      const QString& q,
		      const QString& exe ) :
	QDialog( parent ),
	m_ui( new Ui::keyDialog ),
	m_exe( exe ),
	m_fileManagerOpen( q ),
	m_autoOpenMountPoint( o ),
	m_create( true ),
	m_secrets( s ),
	m_settings( settings::instance() ),
	m_engine( m_exe ),
	m_cancel( std::move( p ) ),
	m_updateVolumeList( std::move( l ) ),
	m_walletKey( s )
{
	m_ui->setupUi( this ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;
	utility::setWindowOptions( this ) ;

	this->setFont( parent->font() ) ;

	this->setUpInitUI() ;

	this->setUpVolumeProperties( {} ) ;

	m_ui->lineEditMountPoint->setText( QString() ) ;

	this->ShowUI() ;
}

void keyDialog::setUpInitUI()
{
	this->setUIVisible( true ) ;

	m_keyType.keyOptions( m_ui->cbKeyType,this,&keyDialog::keyTypeChanged ) ;

	if( utility::platformIsWindows() ){

		/*
		 * We are disabling this functionality on windows for now
		 * it's currently not possible to open a volume in read only mode.
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

		m_ui->lineEditKey->setText( utility::convertPassword( m_key ) ) ;
		m_ui->pbOpen->setFocus() ;
	}

	m_ui->checkBoxVisibleKey->setToolTip( tr( "Check This Box To Make Password Visible" ) ) ;

	m_ui->checkBoxVisibleKey->setEnabled( m_settings.enableRevealingPasswords() ) ;
}

void keyDialog::setVolumeToUnlock()
{
	const auto& s = m_volumes[ m_counter ] ;

	m_counter++ ;

	this->setUpVolumeProperties( s ) ;

	auto a = QString::number( m_counter ) ;
	auto b = QString::number( m_volumes.size() ) ;

	auto likeSsh = m_engine->known() && m_engine->likeSsh() ;

	if( m_volumes.size() > 1 ){

		if( likeSsh ){

			const auto& m = m_engine.cipherFolder() ;

			this->windowSetTitle( tr( "(%1/%2) Connecting To \"%3\"" ).arg( a,b,m ) ) ;

			m_ui->pbOpen->setText( tr( "Connect" ) ) ;
		}else{
			m_ui->pbOpen->setText( tr( "Open" ) ) ;

			this->windowSetTitle( tr( "(%1/%2) Unlocking \"%3\"" ).arg( a,b,m_path ) ) ;
		}
	}else{
		if( likeSsh ){

			const auto& m = m_engine.cipherFolder() ;

			this->windowSetTitle( tr( "Connecting To \"%1\"" ).arg( m ) ) ;

			m_ui->pbOpen->setText( tr( "Connect" ) ) ;
		}else{
			m_ui->pbOpen->setText( tr( "Open" ) ) ;

			this->windowSetTitle( tr( "Unlocking \"%1\"" ).arg( m_path ) ) ;
		}
	}

	this->autoMount( s ) ;
}

QString keyDialog::mountPointPath( const engines::engine& engine,
				   const QString& cipherPath,
				   const QString& moutPointPath,
				   settings& settings,
				   bool reUseMountPoint,
				   const std::function< void() >& function )
{
	if( engine.known() && !engine.backendRequireMountPath() ){

		return QObject::tr( "Not Used" ) ;
	}

	auto m = moutPointPath ;

	if( utility::platformIsWindows() ){

		function() ;

		if( m.isEmpty() ){

			if( settings.windowsUseMountPointPath( engine ) ){

				auto mm = settings.windowsMountPointPath() ;

				utility::createFolder( mm ) ;

				return mm + utility::split( cipherPath,'/' ).last() ;
			}else{
				return utility::freeWindowsDriveLetter() ;
			}
		}else{
			return m ;
		}
	}

	if( m.startsWith( "/" ) ){

		if( reUseMountPoint ){

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
		if( reUseMountPoint ){

			if( m.isEmpty() ){

				return settings.mountPath( cipherPath.split( "/" ).last() ) ;
			}else{
				return settings.mountPath( m.split( "/" ).last() ) ;
			}
		}else{
			return settings.mountPath( [ & ](){

				if( m.isEmpty() ){

					return utility::mountPathPostFix( cipherPath.split( "/" ).last() ) ;
				}else{
					return utility::mountPathPostFix( m ) ;
				}
			}() ) ;
		}
	}
}

void keyDialog::setUpVolumeProperties( const keyDialog::entry& ee )
{
	m_working          = false ;
	const auto& e      = ee.volEntry.favorite() ;
	m_path             = e.volumePath ;
	m_mountOptions     = e ;
	m_favoriteReadOnly = e.readOnlyMode ;

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

	m_ui->lineEditKey->setText( ee.volEntry.password() ) ;

	this->setUIVisible( true ) ;

	this->SetUISetKey( false ) ;

	m_keyType.setAsKey() ;

	if( m_create ){

		m_ui->lineEditMountPoint->setFocus() ;
	}else{
		if( ee.engine->known() ){

			m_engine = ee.engine ;
		}else{
			m_engine = { m_path,m_mountOptions.configFile } ;
		}

		m_ui->pbMountPoint_1->setEnabled( m_engine->supportsMountPathsOnWindows() ) ;

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

		m_enableUsingPassword = true ;

		if( m_engine->known() ){

			if( e.volumeNeedNoPassword || m_engine->requiresNoPassword() ){

				m_enableUsingPassword = false ;
			}

			m_ui->cbKeyType->setEnabled( m_enableUsingPassword ) ;

			m_ui->lineEditKey->setEnabled( m_enableUsingPassword ) ;

			m_mountOptions.configFile = m_engine.configFilePath() ;

			const auto& aa = m_engine->name() ;

			_addAction( aa,aa,false ) ;
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
				const auto& engine = engines::instance().getByName( ac->objectName() ) ;
				m_engine = { engine,m_path,m_mountOptions.configFile } ;
			}
		} ) ;

		m_ui->pbOptions->setMenu( m ) ;

		if( m_ui->lineEditKey->isEnabled() && ee.volEntry.password().isEmpty() ){

			m_ui->lineEditKey->setFocus() ;
		}else{
			m_ui->pbOpen->setFocus() ;
		}
	}

	auto function = [ this ](){

		utility::setWindowsMountPointOptions( this,m_ui->lineEditMountPoint,m_ui->pbMountPoint ) ;
	} ;

	auto s = keyDialog::mountPointPath( m_engine.get(),m_path,e.mountPointPath,
					    m_settings,m_reUseMountPoint,std::move( function ) ) ;

	m_ui->lineEditMountPoint->setText( s ) ;
}

void keyDialog::setDefaultUI()
{
	if( m_create ){

		m_ui->pbOptions->setEnabled( m_engine->hasGUICreateOptions() ) ;

		m_ui->label_3->setVisible( true ) ;

		m_ui->checkBoxOpenReadOnly->setVisible( false ) ;

		m_ui->lineEditFolderPath->setVisible( true ) ;

		m_ui->pbOpenFolderPath->setVisible( true ) ;

		m_ui->pbMountPoint->setVisible( false ) ;

		m_ui->pbMountPoint_1->setVisible( false ) ;

		m_ui->lineEditFolderPath->setEnabled( false ) ;

		m_ui->pbkeyOption->setVisible( false ) ;
	}else{
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

		this->hide() ;

		m_engine->GUICreateOptions( { m_parentWidget,m_createOptions,[ & ]( const engines::engine::cOpts& e ){

			if( e.success ){

				m_createOptions = e ;
			}

			this->ShowUI() ;
		} } ) ;
	}else{
		if( !m_checked ){

			m_checked = true ;

			auto f = favorites::instance().readFavorite( m_path ) ;

			if( f.has_value() ){

				m_mountOptions = f.value() ;
			}
		}

		this->hide() ;

		m_engine->GUIMountOptions( { m_parentWidget,m_create,m_mountOptions,[ this ]( const engines::engine::mOpts& e ){

			if( e.success ){

				m_mountOptions = e ;

				if( m_ui->lineEditKey->text().isEmpty() ){

					m_ui->lineEditKey->setFocus() ;
				}else{
					m_ui->pbOpen->setFocus() ;
				}
			}

			this->ShowUI() ;
		} } ) ;
	}
}

void keyDialog::passWordTextChanged( QString e )
{
	if( m_keyType.key() ){

		int r = m_keyStrength.quality( e ) ;

		if( r < 0 ){

			this->setWindowTitle( tr( "Passphrase Quality: 0%" ) ) ;
		}else{
			this->setWindowTitle( tr( "Passphrase Quality: %1%" ).arg( QString::number( r ) ) ) ;
		}

	}else if( m_keyType.keyKeyFile() || m_keyType.hmacKeyFile() || m_keyType.yubikey() ){

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
	auto m = static_cast< keyDialog::keyType::name >( e ) ;

	return m == keyDialog::keyType::name::Key || m == keyDialog::keyType::name::yubikey ;
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

	auto r = m_ui->lineEditFolderPath->text() ;

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
	m_keyType.setEnabled( m_enableUsingPassword ) ;

	auto index = m_ui->cbKeyType->currentIndex() ;

	if( m_enableUsingPassword ){

		m_ui->lineEditKey->setEnabled( this->keySelected( index ) ) ;
	}else{
		m_ui->lineEditKey->setEnabled( false ) ;
	}

	auto enable = m_keyType.equalsAtLeastOne( keyType::name::keyfile,keyType::name::keyKeyFile ) ;

	m_ui->pbkeyOption->setEnabled( enable ) ;

	if( m_settings.enableRevealingPasswords() ){

		m_ui->checkBoxVisibleKey->setEnabled( this->keySelected( index ) ) ;
	}

	if( utility::platformIsNOTWindows() ){

		m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;
	}else{
		m_ui->pbMountPoint_1->setEnabled( m_engine->supportsMountPathsOnWindows() ) ;
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

	if( m_keyType.containsSecretStorage() ){

		secrets::wallet::walletKey w ;

		LXQt::Wallet::BackEnd ww ;

		switch( m_keyType.type() ){

			case keyType::name::kdewallet :

				ww = LXQt::Wallet::BackEnd::kwallet ;
				break ;

			case keyType::name::libsecret :

				ww = LXQt::Wallet::BackEnd::libsecret ;
				break ;

			case keyType::name::osxkeychain :

				ww = LXQt::Wallet::BackEnd::osxkeychain ;
				break ;

			case keyType::name::windowsdpapi :

				ww = LXQt::Wallet::BackEnd::windows_dpapi ;
				break ;

			case keyType::name::internalWallet :

				ww = LXQt::Wallet::BackEnd::internal ;
				break ;

			default :
				/*
				 * Should not get here
				 */
				ww = LXQt::Wallet::BackEnd::internal ;
		}

		using mm = LXQt::Wallet::BackEnd ;

		if( ww == mm::kwallet || ww == mm::libsecret || ww == mm::osxkeychain ){

			w = m_secrets.walletBk( ww ).getKey( m_path ) ;

		}else if( ww == mm::internal || ww == mm::windows_dpapi ){

			w = m_secrets.walletBk( ww ).getKey( m_path,this ) ;

			if( w.notConfigured ){

				this->showErrorMessage( tr( "Internal Wallet Is Not Configured." ) ) ;
				return ;
			}
		}else{
			/*
			 * We shouldn't get here
			 */
			return this->openVolume() ;
		}

		if( w.opened ){

			auto wallet = m_keyType.toString() ;

			if( w.key.isEmpty() ){

				m_walletType = wallet ;

				auto s = tr( "Volume Not Found in \"%1\".\n\nSet The Volume Key To Add It To The Wallet Before Mounting." ).arg( wallet ) ;

				this->setKeyInWallet( wallet,s ) ;
			}else{
				m_key = utility::convertPassword( w.key ) ;
				this->openVolume() ;
			}
		}
	}else{
		this->openVolume() ;
	}
}

void keyDialog::openMountPoint( const QString& m )
{
	if( m_settings.autoOpenFolderOnMount() ){

		utility::Task::exec( m_fileManagerOpen,{ m } ) ;
	}
}

void keyDialog::reportErrorMessage( const engines::engine::cmdStatus& s )
{
	m_status = s.status() ;

	m_ui->checkBoxOpenReadOnly->setEnabled( true ) ;

	if( m_status == engines::engine::status::cryfsMigrateFileSystem ){

		m_ui->checkBoxOpenReadOnly->setText( tr( "Upgrade File System" ) ) ;

	}else if( m_status == engines::engine::status::cryfsReplaceFileSystem ){

		m_ui->checkBoxOpenReadOnly->setText( tr( "Replace File System" ) ) ;
	}else{
		m_ui->checkBoxOpenReadOnly->setText( m_checkBoxOriginalText ) ;

		if( utility::platformIsWindows() ){

			m_ui->checkBoxOpenReadOnly->setEnabled( false ) ;

			m_ui->checkBoxOpenReadOnly->setChecked( false ) ;
		}
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
	auto deleteKey = utility2::make_raii( [ & ](){ m_walletKey.deleteKey() ; } ) ;

	auto path = m_ui->lineEditFolderPath->text() ;

	auto m = utility::split( path,'/' ).last() ;

	if( utility::pathExists( path ) ){

		return this->showErrorMessage( tr( "Encrypted Folder Path Is Already Taken." ) ) ;
	}

	if( utility::platformIsWindows() ){

		if( m_settings.windowsUseMountPointPath( m_engine.get() ) ){

			m = m_settings.windowsMountPointPath() + m ;

			if( processManager::get().mountPointTaken( m ) ){

				return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
			}

			if( utility::pathExists( m ) && utility::folderNotEmpty( m ) ){

				return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
			}
		}else{
			m = utility::freeWindowsDriveLetter() ;

			if( utility::pathExists( m ) ){

				return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
			}
		}
	}else{
		m = m_settings.mountPath( utility::mountPathPostFix( m ) ) ;

		if( utility::pathExists( m ) && !m_reUseMountPoint ){

			return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
		}
	}

	if( m_engine->takesTooLongToUnlock() ){

		m_warningLabel.showCreate( m_engine->name() ) ;
	}

	this->disableAll() ;

	m_working = true ;

	auto e = siritask::encryptedFolderCreate( { { path,m,m_key,m_createOptions },m_engine.get() } ) ;

	m_warningLabel.hide() ;

	m_working = false ;

	if( e == engines::engine::status::success ){

		if( !e.engine().autorefreshOnMountUnMount() ){

			m_updateVolumeList() ;
		}

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

void keyDialog::encryptedFolderMount()
{
	auto m = m_ui->lineEditMountPoint->text() ;

	if( m.isEmpty() ){

		this->showErrorMessage( tr( "Atleast One Required Field Is Empty." ) ) ;

		return m_ui->lineEditMountPoint->setFocus() ;
	}

	if( utility::pathExists( m ) && !m_reUseMountPoint ){

		if( utility::platformIsWindows() ){

			m_settings.reUseMountPoint( true ) ;
			m_reUseMountPoint = true ;
		}else{
			return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
		}
	}

	if( utility::platformIsWindows() ){

		if( processManager::get().mountPointTaken( m ) ){

			return this->showErrorMessage( tr( "Mount Point Path Already Taken." ) ) ;
		}

		if( !utility::isDriveLetter( m ) && utility::folderNotEmpty( m ) ){

			return this->showErrorMessage( tr( "Mount Point Path Is Not Empty." ) ) ;
		}
	}

	m_mountOptions.opts.unlockInReadOnly = m_ui->checkBoxOpenReadOnly->isChecked() ;

	if( this->upgradingFileSystem() ){

		m_mountOptions.opts.allowUpgradeFileSystem = m_ui->checkBoxOpenReadOnly->isChecked() ;
	}

	if( this->replaceFileSystem() ){

		m_mountOptions.opts.allowReplacedFileSystem = m_ui->checkBoxOpenReadOnly->isChecked() ;
	}

	m_working = true ;

	if( m_engine->unknown() ){

		m_engine = { m_path,m_mountOptions.configFile } ;
	}else{
		m_engine = { m_engine.get(),m_engine.cipherFolder(),m_mountOptions.configFile } ;
	}

	if( m_engine->takesTooLongToUnlock() ){

		m_warningLabel.showUnlock( m_engine->name() ) ;
	}

	this->disableAll() ;

	auto e = siritask::encryptedFolderMount( { { m_path,m,m_key,m_mountOptions },false,m_engine } ) ;

	m_warningLabel.hide() ;

	m_working = false ;

	if( e == engines::engine::status::success ){

		if( !e.engine().autorefreshOnMountUnMount() ){

			m_updateVolumeList() ;
		}

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

		auto type = m_keyType.type() ;

		if( type == keyType::name::kdewallet ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::kwallet ) ;

		}else if( type == keyType::name::libsecret ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::libsecret ) ;

		}else if( type == keyType::name::osxkeychain ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::osxkeychain ) ;

		}else if( type == keyType::name::windowsdpapi ){

			return m_secrets.walletBk( LXQt::Wallet::BackEnd::windows_dpapi ) ;
		}else{
			return m_secrets.walletBk( LXQt::Wallet::BackEnd::internal ) ;
		}
	}() ;

	auto m = [ & ](){

		if( w->backEnd() == LXQt::Wallet::BackEnd::internal ||
		    w->backEnd() == LXQt::Wallet::BackEnd::windows_dpapi ){

			return w.openSync( [](){ return true ; },
					   [ this ](){ this->hide() ; },
					   [ this ](){ this->show() ; } ) ;
		}else{
			return w.open( [](){ return true ; } ) ;
		}
	}() ;

	if( m ){

		QString id ;

		if( m_create ){

			id = m_ui->lineEditFolderPath->text() ;
		}else{
			id = m_path ;
		}

		if( w->readValue( id ).isEmpty() ){

			if( favorites2::addKey( w,id,passphrase ).await() ){

				m_keyType.setType( keyType::name::Key ) ;
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

				return ;
			}else{
				m_ui->labelSetKey->setText( tr( "Failed To Add A Volume To The A Wallet." ) ) ;
			}
		}else{
			m_ui->labelSetKey->setText( tr( "Volume Already Exists In The Wallet." ) ) ;
		}

		return ;
	}else{
		m_ui->labelSetKey->setText( tr( "Failed To Open Wallet." ) ) ;
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

	Task::run( [ this,keyFile,passphrase ](){

		if( m_hmac ){

			return crypto::hmac_key( keyFile,passphrase ) ;
		}else{
			auto exe = m_settings.externalPluginExecutable() ;

			if( exe.isEmpty() ){

				return QByteArray() ;
			}else{
				const auto& env = utility::systemEnvironment() ;

				auto e = utility::split( exe,' ' ) ;

				exe = e.takeAt( 0 ) ;

				e.append( keyFile ) ;

				return utility::Task( exe,e,20000,env,passphrase.toUtf8() ).stdOut() ;
			}
		}

	} ).then( [ this ]( const QByteArray& e ){

		this->setKeyEnabled( true ) ;

		if( e.isEmpty() ){

			m_ui->labelSetKey->setText( tr( "Failed To Generate Key." ) ) ;
		}else{
			m_keyType.setAsKey() ;

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
	m_keyType.setAsKey() ;
}

void keyDialog::setKeyEnabled( bool e )
{
	m_ui->labelSetKeyKeyFile->setEnabled( e ) ;
	m_ui->labelSetKeyPassword->setEnabled( e ) ;
	m_ui->lineEditSetKeyKeyFile->setEnabled( e ) ;
	m_ui->lineEditSetKeyPassword->setEnabled( e ) ;
	m_ui->pbSetKey->setEnabled( m_enableUsingPassword ) ;
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

bool keyDialog::upgradingFileSystem()
{
	return m_status == engines::engine::status::cryfsMigrateFileSystem ;
}

bool keyDialog::replaceFileSystem()
{
	return m_status == engines::engine::status::cryfsReplaceFileSystem ;
}

void keyDialog::keyTypeChanged()
{
	m_walletType.clear() ;

	auto _showVisibleKeyOption = [ this ]( bool e ){

		bool s = m_settings.enableRevealingPasswords() ;
		m_ui->checkBoxVisibleKey->setEnabled( e && s ) ;
		m_ui->checkBoxVisibleKey->setChecked( false ) ;
		m_ui->checkBoxVisibleKey->setVisible( e ) ;
		m_ui->pbkeyOption->setVisible( !e ) ;
	} ;

	if( m_keyType.keyFile() ){

		this->keyFile() ;

	}else if( m_keyType.yubikey() ){

		this->yubiKey() ;

		_showVisibleKeyOption( true ) ;

	}else if( m_keyType.equalsAtLeastOne( keyType::name::keyKeyFile,
					      keyType::name::externalExecutable ) ){

		QString s ;

		if( m_keyType.equals( keyType::name::keyKeyFile ) ){

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

	}else if( m_keyType.hmacKeyFile() ){

		_showVisibleKeyOption( false ) ;

		auto q = QFileDialog::getOpenFileName( this,tr( "Select A KeyFile" ),QDir::homePath() ) ;

		if( q.isEmpty() ){

			m_keyType.setAsKey() ;

			m_ui->lineEditKey->setText( QString() ) ;
		}else{
			this->disableAll() ;

			Task::run( [ q = std::move( q ) ](){

				return crypto::hmac_key( q,QString() ) ;

			} ).then( [ this ]( QByteArray key ){

				m_key = std::move( key ) ;

				m_keyType.setAsKey() ;

				m_ui->lineEditKey->setText( utility::convertPassword( m_key ) ) ;

				if( m_keyStrength && m_create ){

					this->setWindowTitle( tr( "Passphrase Quality: 100%" ) ) ;
				}

				this->enableAll() ;
			} ) ;
		}

	}else if( m_keyType.containsSecretStorage() ){

		if( m_create ){

			if( m_ui->lineEditMountPoint->text().isEmpty() ){

				this->showErrorMessage( tr( "Volume Name Field Is Empty." ) ) ;
				m_ui->cbKeyType->setCurrentIndex( 0 ) ;
				m_ui->lineEditMountPoint->setFocus() ;
				m_ui->checkBoxVisibleKey->setVisible( false ) ;
				return ;
			}

			auto s = tr( "Create A Volume With Specified Key And Then Add The Key In \n\"%1\"." ) ;

			auto m = m_keyType.toString() ;
			this->setKeyInWallet( m,s.arg( m ) ) ;
		}else{
			this->secretStorage() ;

			_showVisibleKeyOption( false ) ;
		}
	}else{
		this->key() ;

		_showVisibleKeyOption( true ) ;
	}
}

void keyDialog::openVolume()
{
	auto _run = [ this ](){

		if( m_create ){

			this->encryptedFolderCreate() ;
		}else{
			this->encryptedFolderMount() ;
		}
	} ;

	if( m_keyType.yubikey() ){

		auto s = m_ui->lineEditKey->text().toUtf8() ;

		if( s.isEmpty() ){

			s = "\n" ;
		}

		auto m = utility::yubiKey( s ) ;

		if( m.has_value() ){

			m_key = m.value() ;

			_run() ;
		}else{
			this->showErrorMessage( tr( "Failed To Locate Or Run Yubikey's \"ykchalresp\" Program." ) ) ;
			return ;
		}

	}else if( m_keyType.key() ){

		m_key = utility::convertPassword( m_ui->lineEditKey->text() ) ;

		_run() ;

	}else if( m_keyType.keyKeyFile() ){

		/*
		 * Should not get here.
		 */

	}else if( m_keyType.hmacKeyFile() ){

		/*
		 * Should not get here.
		 */

	}else if( m_keyType.keyFile() ){

		Task::run( [ this ](){

			return utility::fileContents( m_ui->lineEditKey->text() ) ;

		} ).then( [ this,_run = std::move( _run ) ]( QByteArray key ){

			m_key = std::move( key ) ;

			if( utility::containsAtleastOne( m_key,'\n','\0','\r' ) ){

				this->showErrorMessage( keyDialog::keyFileError() ) ;
			}else{
				_run() ;
			}
		} ) ;
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

void keyDialog::secretStorage()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/module.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( tr( "Plugin name" ) ) ;
	m_ui->lineEditKey->setEnabled( false ) ;
	m_ui->lineEditKey->setText( m_keyType.toString() ) ;
}

void keyDialog::key()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label->setText( m_keyType.toString() ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->checkBoxVisibleKey->setChecked( false ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKey->setFocus() ;
}

void keyDialog::yubiKey()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/passphrase.png" ) ) ;
	m_ui->pbkeyOption->setEnabled( false ) ;
	m_ui->label->setText( m_keyType.toString( keyType::name::Key ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Password ) ;
	m_ui->checkBoxVisibleKey->setChecked( false ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;
	m_ui->lineEditKey->setFocus() ;
}

void keyDialog::keyFile()
{
	m_ui->pbkeyOption->setIcon( QIcon( ":/keyfile.png" ) ) ;
	m_ui->lineEditKey->setEchoMode( QLineEdit::Normal ) ;
	m_ui->label->setText( m_keyType.toString() ) ;
	m_ui->pbkeyOption->setEnabled( true ) ;
	m_ui->lineEditKey->clear() ;
	m_ui->lineEditKey->setEnabled( true ) ;

	auto msg = tr( "Select A File To Be Used As A Keyfile." ) ;
	auto e = QFileDialog::getOpenFileName( this,msg,m_settings.homePath() ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditKey->setText( e ) ;
	}else{
		m_keyType.setAsKey() ;
	}
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
	m_warningLabel.setWarningLabel( m_ui->cryfsWarning ) ;
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

keyDialog::keyType::keyType()
{
}

void keyDialog::keyType::setType( keyDialog::keyType::name s )
{
	for( const auto& it : m_entries ){

		if( it.typeKey == s ){

			m_comboBox->setCurrentIndex( static_cast< int >( it.index ) ) ;

			break ;
		}
	}
}

bool keyDialog::keyType::key() const
{
	return this->type() == keyType::name::Key ;
}

void keyDialog::keyType::setAsKey()
{
	this->setType( keyType::name::Key ) ;
}

bool keyDialog::keyType::keyFile() const
{
	return this->type() == keyType::name::keyfile ;
}

bool keyDialog::keyType::hmacKeyFile() const
{
	return this->type() == keyType::name::hmacKeyFile ;
}

bool keyDialog::keyType::keyKeyFile() const
{
	return this->type() == keyType::name::keyKeyFile ;
}

bool keyDialog::keyType::yubikey() const
{
	return this->type() == keyType::name::yubikey ;
}

bool keyDialog::keyType::exernalExecutable() const
{
	return this->type() == keyType::name::externalExecutable ;
}

void keyDialog::keyType::setEnabled( bool e )
{
	m_comboBox->setEnabled( e ) ;
}

const QString& keyDialog::keyType::toString( keyDialog::keyType::name s ) const
{
	for( const auto& it : m_entries ){

		if( it.typeKey == s ){

			return it.keyName ;
		}
	}

	static QString ShouldNotGetHere ;

	return ShouldNotGetHere ;
}

const QString& keyDialog::keyType::toString() const
{
	return m_entries[ this->currentIndex() ].keyName ;
}

keyDialog::keyType::name keyDialog::keyType::type() const
{
	return m_entries[ this->currentIndex() ].typeKey ;
}

bool keyDialog::keyType::containsSecretStorage() const
{
	auto m = this->type() ;

	return  m == keyType::name::internalWallet ||
		m == keyType::name::libsecret ||
		m == keyType::name::osxkeychain ||
		m == keyType::name::kdewallet ||
		m == keyType::name::windowsdpapi ;
}

void keyDialog::keyType::keyOptions( QComboBox * s,keyDialog * object,void( keyDialog::*method )() )
{
	m_comboBox = s ;

	this->addItem( keyType::name::Key,tr( "Password" ) ) ;
	this->addItem( keyType::name::keyfile,tr( "KeyFile" ) ) ;
	this->addItem( keyType::name::keyKeyFile,tr( "Key+KeyFile" ) ) ;
	this->addItem( keyType::name::hmacKeyFile,tr( "HMAC+KeyFile" ) ) ;
	this->addItem( keyType::name::yubikey,tr( "YubiKey Challenge/Response" ) ) ;
	this->addItem( keyType::name::externalExecutable,tr( "ExternalExecutable" ) ) ;

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::internal ) ){

		this->addItem( keyType::name::internalWallet,tr( "Internal Wallet" ) ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

		this->addItem( keyType::name::libsecret,tr( "Gnome Wallet" ) ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

		this->addItem( keyType::name::kdewallet,tr( "Kde Wallet" ) ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::osxkeychain ) ){

		this->addItem( keyType::name::kdewallet,tr( "OSX KeyChain" ) ) ;
	}

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::windows_dpapi ) ){

		this->addItem( keyType::name::windowsdpapi,tr( "Windows DPAPI" ) ) ;
	}

	this->setAsKey() ;

	connect( m_comboBox,static_cast< void( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ),
		 [ this,function = utility2::make_mem_fn( object,method ) ]( int e ){

		if( e >= 0 && e < static_cast< int >( m_entries.size() ) ){

			function() ;
		}
	} ) ;
}

size_t keyDialog::keyType::currentIndex() const
{
	auto s = static_cast< size_t >( m_comboBox->currentIndex() ) ;

	if( s < m_entries.size() ){

		return s ;
	}else{
		utility::debug() << "Error: index out of range in keyDialog::keyType::currentIndex()" ;
		return 0 ;
	}
}

void keyDialog::keyType::addItem( keyDialog::keyType::name keyType,const QString& keyName )
{
	auto m = m_entries.size() ;
	m_entries.emplace_back( position{ m,keyType,keyName } ) ;
	m_comboBox->addItem( keyName ) ;
}
