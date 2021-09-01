/*
 *
 *  Copyright (c) 2019
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

#include "favorites2.h"
#include "ui_favorites2.h"

#include "tablewidget.h"
#include "utility.h"
#include "dialogmsg.h"
#include "engines.h"
#include "win.h"

#include <QFileDialog>

const QString COMMENT = "-SiriKali_Comment_ID" ;

Task::future< void >& favorites2::deleteKey( secrets::wallet& wallet,const QString& id )
{
	return Task::run( [ &wallet,id ](){

		wallet->deleteKey( id ) ;
		wallet->deleteKey( id + COMMENT ) ;
	} ) ;
}

Task::future< bool >& favorites2::addKey( secrets::wallet& wallet,
					  const QString& id,
					  const QString& key )
{
	return Task::run( [ &wallet,id,key ](){

		if( wallet->addKey( id,key ) ){

			return true ;
		}else{
			utility::debug() << "Failed To Add Key To Wallet" ;

			return false ;
		}
	} ) ;
}

favorites2::favorites2( QWidget * parent,
			secrets& wallet,
			std::function< void() > function,
			const engines::engine& engine,
			const QString& cp ) :
	QDialog ( parent ),
	m_ui( new Ui::favorites2 ),
	m_secrets( wallet ),
	m_settings( settings::instance() ),
	m_function( std::move( function ) ),
	m_engine( engine ),
	m_cipherPath( cp )
{
	m_ui->setupUi( this ) ;

	this->installEventFilter( this ) ;

	this->setFont( parent->font() ) ;

	this->setFixedSize( this->size() ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;

	m_ui->pbFailedToCreateFavorite->setVisible( false ) ;

	m_ui->labelFailedToCreateFavorite->setVisible( false ) ;

	m_ui->lineEditVolumeType->setEnabled( false ) ;

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->pbVolumePath->setIcon( QIcon( ":/sirikali" ) ) ;

	m_ui->tableWidgetWallet->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->tableWidgetWallet->setMouseTracking( true ) ;

	m_ui->tableWidgetWallet->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	m_ui->pbVolumeTypes->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		for( const auto& it : engines::instance().enginesWithNoConfigFile() ){

			auto ac = m->addAction( it ) ;

			ac->setObjectName( it ) ;

			m->addAction( ac ) ;
		}

		connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

			const auto& m = ac->objectName() ;

			m_ui->lineEditVolumeType->setText( m ) ;

			const auto& engine = engines::instance().getByName( m ) ;

			if( engine.known() && engine.likeSsh() ){

				this->setUiLikeSsh( QString(),engine ) ;
			}else{
				this->setDefaultUI( engine ) ;
			}
		} ) ;

		return m ;
	}() ) ;

	connect( m_ui->cbAutoMountAtStartUp,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoMountFavoritesOnStartUp( e ) ;
	} ) ;

	m_ui->cbAutoMountWhenAvailable->setChecked( m_settings.autoMountFavoritesOnAvailable() ) ;

	connect( m_ui->cbShowFavoriteListOnContextMenu,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.showFavoritesInContextMenu( e ) ;
	} ) ;

	connect( m_ui->cbSetMountPathNotPrefix,&QCheckBox::stateChanged,[ this ]( int s ){

		if( s == Qt::CheckState::Checked ){

			m_ui->labelMountPointPrefix->setText( tr( "Mount Point Path" ) ) ;
		}else{
			m_ui->labelMountPointPrefix->setText( tr( "Mount Point Prefix" ) ) ;
		}
	} ) ;

	connect( m_ui->cbAllowExternalToolsToReadPasswords,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.allowExternalToolsToReadPasswords( e ) ;
	} ) ;

	connect( m_ui->cbAutoMountWhenAvailable,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoMountFavoritesOnAvailable( e ) ;
	} ) ;

	connect( m_ui->cbShowMountDialogWhenAutoMounting,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.showMountDialogWhenAutoMounting( e ) ;
	} ) ;

	connect( m_ui->pbConfigFilePath,&QPushButton::clicked,[ this ](){

		this->configPath() ;
	} ) ;

	connect( m_ui->pbAdd,&QPushButton::clicked,[ this ](){

		this->updateFavorite( false ) ;
	} ) ;

	m_ui->pbFolderPath->setObjectName( "FolderHandle" ) ;

	connect( m_ui->pbFolderPath,&QPushButton::clicked,[ this ](){

		if( m_ui->pbFolderPath->objectName() == "FolderHandle" ){

			this->folderPath() ;
		}else{
			this->filePath() ;
		}
	} ) ;

	connect( m_ui->pbFailedToCreateFavorite,&QPushButton::clicked,[ this ](){

		m_ui->pbFailedToCreateFavorite->setVisible( false ) ;

		m_ui->labelFailedToCreateFavorite->setVisible( false ) ;

		m_ui->pbAdd->setEnabled( true ) ;

		m_ui->pbAdd->setFocus() ;
	} ) ;

	connect( m_ui->tableWidgetWallet,&QTableWidget::customContextMenuRequested,[ this ]( QPoint s ){

		Q_UNUSED( s )

		auto item = m_ui->tableWidgetWallet->currentItem() ;

		if( item ){

			this->showContextMenu( item,true ) ;
		}
	} ) ;

	connect( m_ui->tableWidgetWallet,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * c,QTableWidgetItem * p ){

		tablewidget::selectRow( c,p ) ;
	} ) ;

	connect( m_ui->pbPreMount,&QPushButton::clicked,[ this ](){

		this->setCommand( m_ui->lineEditPreMount ) ;
	} ) ;

	connect( m_ui->pbPreUnMount,&QPushButton::clicked,[ this ](){

		this->setCommand( m_ui->lineEditPreUnMount ) ;
	} ) ;

	connect( m_ui->pbPostMount,&QPushButton::clicked,[ this ](){

		this->setCommand( m_ui->lineEditPostMount ) ;
	} ) ;

	connect( m_ui->pbPostUnmount,&QPushButton::clicked,[ this ](){

		this->setCommand( m_ui->lineEditPostUnmount ) ;
	} ) ;

	connect( m_ui->pbEdit,&QPushButton::clicked,[ this ](){

		m_editRow = m_ui->tableWidget->currentRow() ;

		this->updateFavorite( true ) ;
	} ) ;

	connect( m_ui->cbReverseMode,&QCheckBox::toggled,[ this ]( bool e ){

		m_reverseMode = e ;
	} ) ;

	connect( m_ui->cbVolumeNoPassword,&QCheckBox::toggled,[ this ]( bool e ){

		m_volumeNeedNoPassword = e ;
	} ) ;

	connect( m_ui->cbReadOnlyMode,&QCheckBox::toggled,[ this ]( bool e ){

		m_mountReadOnly = e ;
	} ) ;

	connect( m_ui->pbIdentityFile,&QPushButton::clicked,[ this ](){

		auto e = this->getExistingFile( tr( "Path To A Config File" ) ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditIdleTimeOut->setText( e ) ;
		}
	} ) ;

	connect( m_ui->tabWidget,&QTabWidget::currentChanged,[ this ]( int index ){

		this->tabChanged( index ) ;
	} ) ;

	connect( m_ui->pbAddKeyToWallet,&QPushButton::clicked,[ this ](){

		this->addkeyToWallet() ;
	} ) ;

	connect( m_ui->pbAddToWallets,&QPushButton::clicked,[ this ](){

		auto a = m_ui->lineEditEncryptedFolderPath->toPlainText() ;

		if( !a.isEmpty() ){

			auto b = m_ui->lineEditVolumeType->toPlainText() ;

			if( b.isEmpty() ){

				m_ui->lineEditVolumePath->setText( a ) ;
			}else{
				m_ui->lineEditVolumePath->setText( b + " " + a ) ;
			}

			m_ui->tabWidget->setCurrentIndex( 2 ) ;
		}
	} ) ;

	connect( m_ui->pbVolumePath,&QPushButton::clicked,[ this ](){

		auto s = this->getExistingDirectory( tr( "Path To An Encrypted Folder" ) ) ;

		if( !s.isEmpty() ){

			m_ui->lineEditVolumePath->setText( s ) ;
		}
	} ) ;

	connect( &m_volPathFav,&QMenu::triggered,[ this ]( QAction * ac ){

		m_ui->lineEditVolumePath->setText( ac->objectName() ) ;
	} ) ;

	connect( m_ui->pbVolumePathFromFavorites,&QPushButton::clicked,[ this ](){

		m_volPathFav.exec() ;
	} ) ;

	using bk = LXQt::Wallet::BackEnd ;

	m_ui->rbNone->setEnabled( true ) ;

	auto _set_supported = []( QRadioButton * rb,LXQt::Wallet::BackEnd e ){

		rb->setEnabled( LXQt::Wallet::backEndIsSupported( e ) ) ;
	} ;

	_set_supported( m_ui->rbInternalWallet,bk::internal ) ;
	_set_supported( m_ui->rbKWallet,bk::kwallet ) ;
	_set_supported( m_ui->rbLibSecret,bk::libsecret ) ;
	_set_supported( m_ui->rbMacOSKeyChain,bk::osxkeychain ) ;
	_set_supported( m_ui->rbWindowsDPAPI,bk::windows_dpapi ) ;

	auto walletBk = m_settings.autoMountBackEnd() ;

	if( walletBk == bk::windows_dpapi ){

		m_ui->label_22->setEnabled( true ) ;
		m_ui->pbChangeWalletPassword->setEnabled( true ) ;

		m_ui->label_22->setText( tr( "Change Window's Wallet Password" ) ) ;

		m_ui->rbWindowsDPAPI->setChecked( true ) ;

	}else if( walletBk == bk::internal ){

		m_ui->label_22->setEnabled( true ) ;
		m_ui->pbChangeWalletPassword->setEnabled( [ this ](){

			auto a = m_settings.walletName() ;
			auto b = m_settings.applicationName() ;

			return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
		}() ) ;

		m_ui->label_22->setText( tr( "Change Internal Wallet Password" ) ) ;

		m_ui->rbInternalWallet->setChecked( true ) ;

	}else if( walletBk == bk::osxkeychain ){

		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbMacOSKeyChain->setChecked( true ) ;

	}else if( walletBk == bk::libsecret ){

		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbLibSecret->setChecked( true ) ;

	}else if( walletBk == bk::kwallet ){

		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbKWallet->setChecked( true ) ;
	}else{
		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

		m_ui->rbNone->setChecked( true ) ;
	}

	connect( m_ui->rbInternalWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Change Internal Wallet Password" ) ) ;
			m_ui->label_22->setEnabled( true ) ;
			m_ui->pbChangeWalletPassword->setEnabled( true ) ;
			this->walletBkChanged( bk::internal ) ;
			m_settings.autoMountBackEnd( bk::internal ) ;

			m_ui->pbChangeWalletPassword->setEnabled( [ this ](){

				auto a = m_settings.walletName() ;
				auto b = m_settings.applicationName() ;

				return LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd::internal,a,b ) ;
			}() ) ;
		}
	} ) ;

	connect( m_ui->rbWindowsDPAPI,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Change Window's Wallet Password" ) ) ;

			m_ui->label_22->setEnabled( true ) ;
			m_ui->pbChangeWalletPassword->setEnabled( true ) ;
			this->walletBkChanged( bk::windows_dpapi ) ;
			m_settings.autoMountBackEnd( bk::windows_dpapi ) ;
		}
	} ) ;

	connect( m_ui->rbKWallet,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Not Applicable" ) ) ;

			m_ui->label_22->setEnabled( false ) ;
			m_ui->pbChangeWalletPassword->setEnabled( false ) ;
			this->walletBkChanged( bk::kwallet ) ;
			m_settings.autoMountBackEnd( bk::kwallet ) ;
		}
	} ) ;

	connect( m_ui->rbLibSecret,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Not Applicable" ) ) ;
			m_ui->label_22->setEnabled( false ) ;
			m_ui->pbChangeWalletPassword->setEnabled( false ) ;
			this->walletBkChanged( bk::libsecret ) ;
			m_settings.autoMountBackEnd( bk::libsecret ) ;
		}
	} ) ;

	connect( m_ui->rbMacOSKeyChain,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->label_22->setText( tr( "Not Applicable" ) ) ;
			m_ui->label_22->setEnabled( false ) ;
			m_ui->pbChangeWalletPassword->setEnabled( false ) ;
			this->walletBkChanged( bk::osxkeychain ) ;
			m_settings.autoMountBackEnd( bk::osxkeychain ) ;
		}
	} ) ;

	connect( m_ui->rbNone,&QRadioButton::toggled,[ this ](){

		m_ui->label_22->setText( tr( "Not Applicable" ) ) ;
		m_ui->label_22->setEnabled( false ) ;
		m_ui->pbChangeWalletPassword->setEnabled( false ) ;
		this->setControlsAvailability( false,true ) ;
		m_settings.autoMountBackEnd( settings::walletBackEnd() ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		utility::setWindowsMountPointOptions( this,m_ui->lineEditMountPath,m_ui->pbMountPointPath ) ;
		m_ui->labelMountPointPrefix->setText( tr( "Mount Point Path" ) ) ;
	}else{
		m_ui->pbMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

		m_ui->labelMountPointPrefix->setText( tr( "Mount Point Prefix" ) ) ;

		connect( m_ui->pbMountPointPath,&QPushButton::clicked, [ this ](){

			this->mountPointPath() ;
		} ) ;
	}

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		const auto& volumes = favorites::instance().readFavorites() ;

		auto s = static_cast< size_t >( item->row() ) ;

		if( s < volumes.size() ){

			this->setVolumeProperties( volumes[ s ] ) ;
		}else{
			utility::debug() << "Warning: Invalid row number, not updating in favorites2::favorites2" ;
		}
	} ) ;

	connect( m_ui->tableWidget,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * x,QTableWidgetItem * y ){

		tablewidget::selectRow( x,y ) ;
	} ) ;

	m_ui->labelPassword->setVisible( false ) ;
	m_ui->pbVisiblePassword->setVisible( false ) ;

	connect( m_ui->pbVisiblePassword,&QPushButton::clicked,[ this ](){

		m_ui->labelPassword->clear() ;
		m_ui->labelPassword->setVisible( false ) ;
		m_ui->pbVisiblePassword->setVisible( false ) ;
	} ) ;

	m_ui->cbShowPassword->setChecked( true ) ;

	m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;

	connect( m_ui->cbShowPassword,&QCheckBox::stateChanged,[ this ]( int s ){

		if( s ){

			m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;
		}else{
			m_ui->lineEditPassword->setEchoMode( QLineEdit::Normal ) ;
		}
	} ) ;

	connect( m_ui->pbChangeWalletPassword,&QPushButton::clicked,[ this ](){

		auto a = m_settings.walletName() ;
		auto b = m_settings.applicationName() ;

		if( m_ui->rbInternalWallet->isChecked() ){

			this->hide() ;

			m_secrets.changeInternalWalletPassword( a,b,[ this ]( bool s ){

				if( s ){

					this->walletBkChanged( LXQt::Wallet::BackEnd::internal ) ;
				}

				this->show() ;
			} ) ;

		}else if( m_ui->rbWindowsDPAPI->isChecked() ){

			this->hide() ;

			m_secrets.changeWindowsDPAPIWalletPassword( a,b,[ this ]( bool s ){

				if( s ){

					this->walletBkChanged( LXQt::Wallet::BackEnd::windows_dpapi ) ;
				}

				this->show() ;
			} ) ;
		}
	} ) ;

	m_ui->pbFolderPath->setIcon( QIcon( ":/sirikali.png" ) ) ;
	m_ui->pbConfigFilePath->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pbIdentityFile->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pbAddToWallets->setIcon( QIcon( ":/lock.png" ) ) ;

	QIcon exeIcon( ":/executable.png" ) ;

	m_ui->pbPreMount->setIcon( exeIcon ) ;
	m_ui->pbPostMount->setIcon( exeIcon ) ;
	m_ui->pbPreUnMount->setIcon( exeIcon ) ;
	m_ui->pbPostUnmount->setIcon( exeIcon ) ;

	m_ui->cbAutoMount->setChecked( false ) ;

	m_ui->cbReadOnlyMode->setEnabled( utility::platformIsNOTWindows() ) ;

	this->checkFavoritesConsistency() ;

	const auto& volumes = favorites::instance().readFavorites() ;

	this->updateVolumeList( volumes,0 ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Enter,Qt::Key_Return,Qt::Key_Menu } ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->shortcutPressed() ;
		} ) ;

		return ac ;
	}() ) ;

	auto optionsMenu = new QMenu( this ) ;

	optionsMenu->setFont( this->font() ) ;

	connect( optionsMenu->addAction( tr( "Toggle AutoMount" ) ),&QAction::triggered,[ this ](){

		this->toggleAutoMount() ;
	} ) ;

	optionsMenu->addSeparator() ;

	connect( optionsMenu->addAction( tr( "Edit" ) ),&QAction::triggered,[ this ](){

		this->edit() ;
	} ) ;

	optionsMenu->addSeparator() ;

	connect( optionsMenu->addAction( tr( "Remove Selected Entry" ) ),&QAction::triggered,[ this ](){

		this->removeEntryFromFavoriteList() ;
	} ) ;

	optionsMenu->addSeparator() ;

	connect( optionsMenu->addAction( tr( "Add Entry To Default Wallet" ) ),&QAction::triggered,[ this ](){

		auto table = m_ui->tableWidget ;

		auto m = table->currentRow() ;

		if( m != -1 ){

			auto a = table->item( m,0 )->text() ;

			if( !a.isEmpty() && !m_ui->rbNone->isChecked() ){

				m_ui->lineEditVolumePath->setText( a ) ;

				m_ui->tabWidget->setCurrentIndex( 2 ) ;

				m_ui->lineEditPassword->setFocus() ;
			}
		}
	} ) ;

	optionsMenu->addSeparator() ;

	optionsMenu->addAction( tr( "Cancel" ) ) ;

	m_ui->pbOptions->setMenu( optionsMenu ) ;

	this->ShowUI() ;
}

favorites2::~favorites2()
{
	delete m_ui ;
}

void favorites2::setCommand( QLineEdit * lineEdit )
{
	auto path = this->getExistingFile( tr( "Select A Command Executable" ) ) ;

	if( !path.isEmpty() ){

		lineEdit->setText( path ) ;
	}
}

void favorites2::addkeyToWallet()
{
	auto a = m_ui->lineEditVolumePath->text() ;
	auto b = m_ui->lineEditPassword->text() ;

	if( a.isEmpty() || b.isEmpty() ){

		return ;
	}

	m_ui->pbAddKeyToWallet->setEnabled( false ) ;

	m_ui->tableWidgetWallet->setEnabled( false ) ;
	m_ui->pbAddKeyToWallet->setEnabled( false ) ;

	favorites2::addKey( m_wallet.get(),a,b ).then( [ this,a ]( bool s ){

		m_ui->tableWidgetWallet->setEnabled( true ) ;
		m_ui->pbAddKeyToWallet->setEnabled( true ) ;

		if( s ){

			tablewidget::addRow( m_ui->tableWidgetWallet,{ a } ) ;

			m_ui->lineEditPassword->clear() ;
			m_ui->lineEditPassword->setEchoMode( QLineEdit::Password ) ;
			m_ui->lineEditVolumePath->clear() ;
			m_ui->lineEditVolumePath->setFocus() ;
		}

		m_ui->pbAddKeyToWallet->setEnabled( true ) ;
	} ) ;
}

void favorites2::deleteKeyFromWallet( const QString& id )
{
	m_ui->tableWidgetWallet->setEnabled( false ) ;
	m_ui->pbAddKeyToWallet->setEnabled( false ) ;
	favorites2::deleteKey( m_wallet.get(),id ).await() ;
	m_ui->pbAddKeyToWallet->setEnabled( true ) ;
	m_ui->tableWidgetWallet->setEnabled( true ) ;
}

QStringList favorites2::readAllKeys()
{
	QStringList a ;

	const auto s = Task::await( [ & ](){ return m_wallet->readAllKeys() ; } ) ;

	for( const auto& it : s ){

		if( !it.endsWith( "-SiriKali_Comment_ID" ) ){

			a.append( it ) ;
		}
	}

	return a ;
}

void favorites2::showContextMenu( QTableWidgetItem * item,bool itemClicked )
{
	QMenu m ;

	connect( m.addAction( tr( "Delete Entry" ) ),&QAction::triggered,[ this ](){

		auto table = m_ui->tableWidgetWallet ;

		auto row = table->currentRow() ;

		if( row != -1 ){

			auto a = table->item( row,0 )->text() ;
			this->deleteKeyFromWallet( a ) ;
			tablewidget::deleteRow( table,row ) ;
		}
	} ) ;

	connect( m.addAction( tr( "Show Password" ) ),&QAction::triggered,[ this ](){

		auto table = m_ui->tableWidgetWallet ;

		auto row = table->currentRow() ;

		if( row != -1 ){

			auto a = Task::await( [ & ]()->QString{

				return m_wallet->readValue( table->item( row,0 )->text() ) ;
			} ) ;

			if( !a.isEmpty() ){

				auto e = QObject::tr( "The Password Is \"%1\"" ).arg( a ) ;

				m_ui->labelPassword->setText( e ) ;
				m_ui->labelPassword->setVisible( true ) ;
				m_ui->pbVisiblePassword->setVisible( true ) ;
			}
		}
	} ) ;

	m.addAction( tr( "Close Menu" ) ) ;

	m.setFont( this->font() ) ;

	if( itemClicked ){

		m.exec( QCursor::pos() ) ;
	}else{
		auto p = this->pos() ;

		auto x = p.x() + 100 + m_ui->tableWidgetWallet->columnWidth( 0 ) ;
		auto y = p.y() + 50 + m_ui->tableWidgetWallet->rowHeight( 0 ) * item->row() ;

		p.setX( x ) ;
		p.setY( y ) ;
		m.exec( p ) ;
	}
}

template< typename Function >
static auto _hide_ui( Function hide,LXQt::Wallet::BackEnd bk )
{
	return [ hide = std::move( hide ),bk ](){

		using w = LXQt::Wallet::BackEnd ;

		if( bk == w::internal || bk == w::windows_dpapi ){

			hide() ;
		}
	} ;
}

void favorites2::walletBkChanged( LXQt::Wallet::BackEnd bk )
{
	this->setControlsAvailability( true,true ) ;

	m_wallet = m_secrets.walletBk( bk ) ;

	m_wallet.open( [ this ](){

		for( const auto& it : this->readAllKeys() ){

			tablewidget::addRow( m_ui->tableWidgetWallet,{ it } ) ;
		}

	},_hide_ui( [ this ](){ this->hide() ; },bk ),[ this ]( bool opened ){

		if( opened ){

			for( const auto& it : this->readAllKeys() ){

				tablewidget::addRow( m_ui->tableWidgetWallet,{ it } ) ;
			}
		}else{
			m_ui->lineEditVolumePath->clear() ;

			this->setControlsAvailability( false,true ) ;
		}

		this->show() ;
	} ) ;
}

void favorites2::setControlsAvailability( bool e,bool m )
{
	if( m ){

		tablewidget::clearTable( m_ui->tableWidgetWallet ) ;
	}

	m_ui->label_21->setEnabled( e ) ;
	m_ui->label_23->setEnabled( e ) ;
	m_ui->tableWidgetWallet->setEnabled( e ) ;
	m_ui->pbAddKeyToWallet->setEnabled( e ) ;
	m_ui->lineEditVolumePath->setEnabled( e ) ;
	m_ui->lineEditPassword->setEnabled( e ) ;
	m_ui->pbVolumePath->setEnabled( e ) ;
	m_ui->pbVolumePathFromFavorites->setEnabled( e ) ;
	m_ui->cbShowPassword->setEnabled( e ) ;
}

void favorites2::tabChanged( int index )
{
	if( index == 0 ){

		/*
		 * favorites list tab
		 */

		m_ui->lineEditVolumePath->clear() ;
		m_ui->lineEditPassword->clear() ;

		m_editMode = false ;

	}else if( index == 1 ){

		/*
		 * Add/Edit tab
		 */

		m_ui->lineEditVolumePath->clear() ;
		m_ui->lineEditPassword->clear() ;

		m_ui->pbAddToWallets->setEnabled( !m_ui->rbNone->isChecked() ) ;

		if( m_editMode ){

			m_ui->pbEdit->setEnabled( true ) ;
			m_ui->pbAdd->setEnabled( false ) ;
			m_ui->pbEdit->setFocus() ;

			m_ui->labelMountPointPrefix->setText( tr( "Mount Point Path" ) ) ;
		}else{
			m_ui->labelMountPointPrefix->setText( tr( "Mount Point Prefix" ) ) ;

			this->clearEditVariables() ;

			if( !m_cipherPath.isEmpty() ){

				m_ui->lineEditEncryptedFolderPath->setText( m_cipherPath ) ;
				m_cipherPath.clear() ;
			}

			m_ui->pbEdit->setEnabled( false ) ;
			m_ui->pbAdd->setEnabled( true ) ;

			if( utility::platformIsWindows() ){

				m_ui->lineEditMountPath->setText( utility::freeWindowsDriveLetter() ) ;
			}else{
				m_ui->lineEditMountPath->setText( m_settings.mountPath() ) ;
			}

			this->setDefaultUI( engines::instance().getUnKnown() ) ;

			m_ui->pbAdd->setFocus() ;
		}

	}else if( index == 2 ){

		/*
		 * Manage keys in wallets
		 */

		m_editMode = false ;

		auto w = m_settings.autoMountBackEnd() ;

		if( w.isValid() ){

			this->walletBkChanged( w.bk() ) ;
		}else {
			this->setControlsAvailability( false,true ) ;
		}

		m_volPathFav.clear() ;

		favorites::instance().entries( [ & ]( const favorites::entry& e ){

			const auto& s = e.volumePath ;

			m_volPathFav.addAction( s )->setObjectName( s ) ;
		} ) ;

		m_ui->pbVolumePathFromFavorites->setMenu( &m_volPathFav ) ;
	}else{
		/*
		 * Settings
		 */

		m_editMode = false ;

		m_ui->lineEditVolumePath->clear() ;
		m_ui->lineEditPassword->clear() ;
	}
}

template< typename M >
static void _updateList( QTableWidget * table,const QFont& font,
			 const std::vector<favorites::entry>& e,const M& m )
{
	tablewidget::clearTable( table ) ;

	for( size_t i = 0 ; i < e.size() ; i++ ){

		const auto& s = e[ i ].volumePath ;

		tablewidget::addRow( table,{ s },font ) ;
		tablewidget::setRowToolTip( table,static_cast< int >( i ),s ) ;
	}

	tablewidget::selectRow( table,m ) ;
}

void favorites2::updateVolumeList( const std::vector<favorites::entry>& e,const QString& volPath )
{
	_updateList( m_ui->tableWidget,this->font(),e,volPath ) ;

	this->setVolumeProperties( [ & ]()->utility2::result_ref< const favorites::entry& >{

		for( const auto& it : e ){

			if( it.volumePath == volPath ){

				return it ;
			}
		}

		return {} ;
	}() ) ;
}

void favorites2::updateVolumeList( const std::vector< favorites::entry >& e,size_t row )
{
	if( e.size() == 0 ){

		tablewidget::clearTable( m_ui->tableWidget ) ;

		this->clearVolumeProperties() ;

		utility::debug() << "Information: Favorites list is empty" ;
	}else{
		_updateList( m_ui->tableWidget,this->font(),e,static_cast< int >( row ) ) ;

		if( row < e.size() ){

			this->setVolumeProperties( e[ row ] ) ;
		}else{
			utility::debug() << "Warning: Row count mismatch in favorites2::updateVolumeList" ;
		}
	}
}

void favorites2::showUpdatedEntry( const favorites::entry& e )
{
	this->updateVolumeList( favorites::instance().readFavorites(),e.volumePath ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;
}

void favorites2::toggleAutoMount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		auto e = this->getEntry( row ) ;

		if( e.has_value() ){

			auto f = e.value() ;

			f.autoMount.toggle() ;

			if( m_ui->textEditAutoMount->toPlainText() == "false" ){

				m_ui->textEditAutoMount->setText( "true" ) ;
			}else{
				m_ui->textEditAutoMount->setText( "false" ) ;
			}

			favorites::instance().replaceFavorite( e.value(),f ) ;
		}else{
			utility::debug() << "Warning: favorites2::toggleAutoMount() out of range" ;
		}
	}
}

void favorites2::edit()
{
	auto table = m_ui->tableWidget ;

	m_editRow = table->currentRow() ;
	m_editMode = true ;

	const auto& volumes = favorites::instance().readFavorites() ;

	if( m_editRow >= 0 && m_editRow < int( volumes.size() ) ){

		this->clearEditVariables() ;

		const auto& entry = volumes[ size_t( m_editRow ) ] ;

		m_ui->lineEditEncryptedFolderPath->setText( entry.volumePath ) ;
		m_ui->lineEditMountPath->setText( entry.mountPointPath ) ;
		m_ui->cbAutoMount->setChecked( entry.autoMount ) ;
		m_ui->lineEditPreMount->setText( entry.preMountCommand ) ;
		m_ui->lineEditPreUnMount->setText( entry.preUnmountCommand ) ;
		m_ui->lineEditPostMount->setText( entry.postMountCommand ) ;
		m_ui->lineEditPostUnmount->setText( entry.postUnmountCommand ) ;
		m_ui->lineEditMountOptions->setText( entry.mountOptions ) ;

		m_ui->lineEditConfigFilePath->setText( entry.configFilePath ) ;
		m_ui->lineEditIdleTimeOut->setText( entry.idleTimeOut ) ;

		m_ui->cbReverseMode->setChecked( entry.reverseMode ) ;
		m_ui->cbVolumeNoPassword->setChecked( entry.volumeNeedNoPassword ) ;
		m_ui->cbReadOnlyMode->setChecked( entry.readOnlyMode.True() ) ;
		m_ui->cbVolumeNoPassword->setChecked( entry.volumeNeedNoPassword ) ;

		auto ss = m_ui->lineEditEncryptedFolderPath->toPlainText() ;

		const auto& engines = engines::instance() ;

		const auto& engine = engines.getByPaths( ss ) ;

		if( engine->known() ){

			if( engine->likeSsh() ){

				this->setUiLikeSsh( engine.cipherFolder(),engine.get() ) ;

				m_ui->lineEditConfigFilePath->setText( entry.identityAgent ) ;
				m_ui->lineEditIdleTimeOut->setText( entry.identityFile ) ;
			}else{
				m_ui->lineEditEncryptedFolderPath->setText( engine.cipherFolder() ) ;

				m_ui->lineEditVolumeType->clear() ;

				for( const auto& it : engines.enginesWithNoConfigFile() ){

					if( it == engine->name() || it == engine->displayName() ){

						m_ui->lineEditVolumeType->setText( engine->uiName() ) ;

						break ;
					}
				}
			}
		}

		m_ui->pbAdd->setEnabled( false ) ;
		m_ui->tabWidget->setCurrentIndex( 1 ) ;
	}
}

void favorites2::configPath()
{
	auto e = this->getExistingFile( tr( "Path To A Config File" ) ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditConfigFilePath->setText( e ) ;
	}
}

void favorites2::removeEntryFromFavoriteList()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		auto aa = this->getEntry( row ) ;

		if( aa.has_value() ){

			favorites::instance().removeFavoriteEntry( aa.value() ) ;

			const auto& volumes = favorites::instance().readFavorites() ;

			this->updateVolumeList( volumes,volumes.size() - 1 ) ;
		}else{
			utility::debug() << "Warning: favorites2::removeEntryFromFavoriteList() out of range" ;
		}
	}
}

void favorites2::updateFavorite( bool edit )
{
	DialogMsg msg( m_parentWidget,this ) ;

	utility::hideQWidget thisWidget( this ) ;

	const auto type = m_ui->lineEditVolumeType->toPlainText() ;
	const auto dev  = m_ui->lineEditEncryptedFolderPath->toPlainText() ;
	const auto path = m_ui->lineEditMountPath->toPlainText() ;

	const auto& engine = engines::instance().getByName( type ) ;

	if( dev.isEmpty() ){

		thisWidget.hide() ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Encrypted Folder Address Field Is Empty" ) ) ;
	}
	if( path.isEmpty() ){

		thisWidget.hide() ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount Point Path Field Is Empty" ) ) ;
	}

	m_ui->tableWidget->setEnabled( false ) ;

	bool likeSsh = false ;

	auto dev_path = [ & ](){

		if( type.isEmpty() ){

			return dev ;
		}else{
			likeSsh = engine.known() && engine.likeSsh() ;

			if( dev.startsWith( type + " ",Qt::CaseInsensitive ) ){

				return dev ;
			}else{
				return type + " " + dev ;
			}
		}
	}() ;

	favorites::entry e ;

	e.volumePath           = dev_path ;
	e.mountPointPath       = path ;
	e.autoMount            = m_ui->cbAutoMount->isChecked() ;
	e.mountOptions         = m_ui->lineEditMountOptions->toPlainText() ;
	e.volumeNeedNoPassword = m_ui->cbVolumeNoPassword->isChecked() ;
	e.preMountCommand      = m_ui->lineEditPreMount->text() ;
	e.preUnmountCommand    = m_ui->lineEditPreUnMount->text() ;
	e.postMountCommand     = m_ui->lineEditPostMount->text() ;
	e.postUnmountCommand   = m_ui->lineEditPostUnmount->text() ;
	e.reverseMode          = m_ui->cbReverseMode->isChecked() ;

	if( likeSsh ){

		e.identityAgent = m_ui->lineEditConfigFilePath->toPlainText() ;
		e.identityFile  = m_ui->lineEditIdleTimeOut->toPlainText() ;

		auto a = m_ui->lineEditSshPortNumber->text() ;

		auto b = "port=" + a ;

		if( !a.isEmpty() && a != "22" ){

			e.volumePath = utility::likeSshaddPortNumber( e.volumePath,a ) ;

			if( e.mountOptions.isEmpty() ){

				e.mountOptions = b ;
			}else{
				e.mountOptions += "," + b ;
			}
		}
	}else{
		e.configFilePath = m_ui->lineEditConfigFilePath->toPlainText() ;
		e.idleTimeOut    = m_ui->lineEditIdleTimeOut->toPlainText() ;
	}

	if( edit ){

		const auto& f = this->getEntry( m_editRow ) ;

		if( f.has_value() ){

			if( engine.known() && engine.usesOnlyMountPoint() ){

				e.mountPointPath = dev ;
			}

			favorites::instance().replaceFavorite( f.value(),e ) ;
		}else{
			utility::debug() << "Warning: favorites2::getEntry() out of range" ;
		}
	}else{
		if( utility::platformIsWindows() ){

			if( !utility::isDriveLetter( path ) ){

				if( !m_ui->cbSetMountPathNotPrefix->isChecked() ){

					e.mountPointPath = path + "/" + utility::split( dev,'/' ).last() ;
				}			}
		}else{
			if( !m_ui->cbSetMountPathNotPrefix->isChecked() ){

				e.mountPointPath = path + "/" + utility::split( dev,'/' ).last() ;
			}
		}

		auto a = favorites::instance().add( e ) ;

		if( a != favorites::error::SUCCESS ){

			m_ui->pbFailedToCreateFavorite->setVisible( true ) ;

			m_ui->labelFailedToCreateFavorite->setVisible( true ) ;

			m_ui->pbAdd->setEnabled( false ) ;

			m_ui->pbFailedToCreateFavorite->setFocus() ;

			if( a == favorites::error::ENTRY_ALREADY_EXISTS ){

				m_ui->labelFailedToCreateFavorite->setText( tr( "Entry Already Exist" ) ) ;
			}else{
				m_ui->labelFailedToCreateFavorite->setText( tr( "Failed To Create Entry" ) ) ;
			}

			m_ui->tableWidget->setEnabled( true ) ;

			return ;
		}
	}

	m_ui->lineEditPreMount->clear() ;
	m_ui->lineEditPreUnMount->clear() ;
	m_ui->lineEditPostMount->clear() ;
	m_ui->lineEditPostUnmount->clear() ;
	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;
	m_ui->lineEditConfigFilePath->clear() ;
	m_ui->lineEditIdleTimeOut->clear() ;
	m_ui->lineEditMountOptions->clear() ;
	m_ui->lineEditVolumeType->clear() ;
	m_ui->cbReverseMode->setChecked( false ) ;
	m_ui->cbReadOnlyMode->setChecked( false ) ;
	m_ui->cbVolumeNoPassword->setChecked( false ) ;
	m_ui->cbAutoMount->setChecked( false ) ;

	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;

	this->showUpdatedEntry( e ) ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void favorites2::cancel()
{
	this->HideUI() ;
}

void favorites2::folderPath()
{
	auto e = this->getExistingDirectory( tr( "Path To An Encrypted Folder" ) ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditEncryptedFolderPath->setText( e ) ;

		const auto& engine = engines::instance().getByPaths( e ) ;

		if( engine->known() && !engine->hasConfigFile() ){

			m_ui->lineEditVolumeType->setText( engine->uiName() ) ;
		}
	}
}

void favorites2::filePath()
{
	auto e = utility::getExistingFile( this,tr( "Select A File" ),QDir::homePath() ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditEncryptedFolderPath->setText( e ) ;

		const auto& engine = engines::instance().getByPaths( e ) ;

		if( engine->known() && !engine->hasConfigFile() ){

			m_ui->lineEditVolumeType->setText( engine->uiName() ) ;
		}
	}
}

void favorites2::mountPointPath()
{
	auto e = this->getExistingDirectory( tr( "Path To Mount Folder" ) ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditMountPath->setText( e ) ;
	}
}

void favorites2::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void favorites2::itemClicked( QTableWidgetItem * current,bool clicked )
{
	Q_UNUSED( current )
	Q_UNUSED( clicked )
}

void favorites2::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true ) ;
}

void favorites2::shortcutPressed()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void favorites2::clearEditVariables()
{
	m_ui->lineEditVolumeType->clear() ;
	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;
	m_ui->lineEditConfigFilePath->clear() ;
	m_ui->lineEditIdleTimeOut->clear() ;
	m_ui->lineEditMountOptions->clear() ;
	m_ui->cbReverseMode->setChecked( false ) ;
	m_ui->cbVolumeNoPassword->setChecked( false ) ;
	m_ui->cbReadOnlyMode->setChecked( false ) ;
	m_ui->cbAutoMount->setChecked( false ) ;
}

void favorites2::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void favorites2::HideUI()
{
	this->hide() ;
	m_function() ;
	this->deleteLater() ;
}

void favorites2::checkFavoritesConsistency()
{
}

utility2::result_ref< const favorites::entry& > favorites2::getEntry( int row )
{
	auto m = static_cast< size_t >( row ) ;

	const auto& ff = favorites::instance() ;

	const auto& volumes = ff.readFavorites() ;

	if( m < volumes.size() ){

		return volumes[ m ] ;
	}else{
		return {} ;
	}
}

QString favorites2::getExistingFile( const QString& r )
{
	auto e = QFileDialog::getOpenFileName( this,r,QDir::homePath() ) ;

	while( true ){

		if( e.endsWith( '/' ) ){

			e.truncate( e.length() - 1 ) ;
		}else{
			break ;
		}
	}

	return e ;
}

QString favorites2::getExistingDirectory( const QString& r )
{
	return utility::getExistingDirectory( this,r,QDir::homePath() ) ;
}

void favorites2::setVolumeProperties( utility2::result_ref< const favorites::entry& > ee )
{
	const auto& e = [ & ](){

		if( ee.has_value() ){

			return ee.value() ;
		}else{
			utility::debug() << "Warning: Unknown Favorite Entry Encountered in favorites2::setVolumeProperties" ;

			return favorites::entry::empty() ;
		}
	}() ;

	m_ui->textEditMountPoint->setText( e.mountPointPath ) ;

	if( e.autoMount.defined() ){

		m_ui->textEditAutoMount->setText( e.autoMount ? "true" : "false" ) ;
	}else{
		m_ui->textEditAutoMount->setText( QString() ) ;
	}

	const auto& engine = engines::instance().getByPaths( e.volumePath ) ;

	if( engine->known() && engine->likeSsh() ){

		m_ui->label_3->setText( tr( "SSH_AUTH_SOCK Socket Path" ) ) ;
		m_ui->label_4->setText( tr( "IdentityFile Path" ) ) ;

		m_ui->textEditConfigFilePath->setText( e.identityAgent ) ;

		m_ui->textEditIdleTimeOut->setText( e.identityFile ) ;

		this->setUiLikeSsh( engine.cipherFolder(),engine.get() ) ;
	}else{
		this->setDefaultUI( engine.get() ) ;

		m_ui->label_3->setText( tr( "Config File Path" ) ) ;
		m_ui->label_4->setText( tr( "Idle Time Out" ) ) ;

		m_ui->textEditConfigFilePath->setText( e.configFilePath ) ;

		m_ui->textEditIdleTimeOut->setText( e.idleTimeOut ) ;
	}

	m_ui->textEditMountOptions->setText( e.mountOptions ) ;

	m_ui->textEditPreMount->setText( e.preMountCommand ) ;

	m_ui->textEditPostMount->setText( e.postMountCommand ) ;

	m_ui->textEditPreUnMount->setText( e.preUnmountCommand ) ;

	m_ui->textEditPostUnmount->setText( e.postUnmountCommand ) ;
}

void favorites2::clearVolumeProperties()
{
	m_ui->textEditAutoMount->clear() ;

	m_ui->textEditMountPoint->clear() ;

	m_ui->textEditConfigFilePath->clear() ;

	m_ui->textEditIdleTimeOut->clear() ;

	m_ui->textEditMountOptions->clear() ;

	m_ui->textEditPreMount->clear() ;

	m_ui->textEditPostMount->clear() ;

	m_ui->textEditPreUnMount->clear() ;

	m_ui->textEditPostUnmount->clear() ;
}

void favorites2::setUiLikeSsh( const QString& cipherPath,const engines::engine& engine )
{
	m_ui->pbFolderPath->setObjectName( "likeSshHandle" ) ;

	m_ui->pbFolderPath->setIcon( QIcon( ":/sirikali.png" ) ) ;

	m_ui->pbFolderPath->setEnabled( false ) ;

	m_ui->lineEditEncryptedFolderPath->setText( cipherPath ) ;

	m_ui->lineEditVolumeType->setText( engine.uiName() ) ;

	auto s = m_ui->lineEditMountOptions->toPlainText() ;

	if( s.isEmpty() ){

		m_ui->lineEditMountOptions->setText( engine.defaultFavoritesMountOptions() ) ;
	}

	if( utility::platformIsWindows() ){

		m_ui->label_15->setText( tr( "Create Network Drive" ) ) ;
	}

	m_ui->labelName ->setText( tr( "Remote Ssh Server Address\n(Example: woof@example.com:/remote/path)" ) ) ;
	m_ui->labelCofigFilePath->setText( tr( "SSH_AUTH_SOCK Socket Path (Optional)" ) ) ;
	m_ui->labelIdleTimeOut->setText( tr( "IdentityFile Path (Optional)" ) ) ;

	m_ui->lineEditSshPortNumber->setEnabled( true ) ;
	m_ui->labelPortyNumber->setEnabled( true ) ;
	m_ui->labelPortyNumber->setText( tr( "Ssh Port Number" ) ) ;

	m_ui->pbIdentityFile->setVisible( true ) ;

	if( s.contains( "port=",Qt::CaseInsensitive ) ){

		QStringList ss ;

		for( const auto& it : utility::split( s,',' ) ){

			if( it.startsWith( "port=",Qt::CaseInsensitive ) ){

				m_ui->lineEditSshPortNumber->setText( it.mid( 5 ) ) ;
			}else{
				ss.append( it ) ;
			}
		}

		m_ui->lineEditMountOptions->setText( ss.join( ',' ) ) ;
	}else{
		m_ui->lineEditSshPortNumber->setText( "22" ) ;
	}
}

void favorites2::setDefaultUI( const engines::engine& engine )
{
	m_ui->label_15->setText( tr( "Reverse Mode (Gocryptfs and Encfs Only)" ) ) ;
	m_ui->labelCofigFilePath->setText( tr( "Config File Path (Optional)" ) ) ;
	m_ui->labelIdleTimeOut->setText( tr( "Idle TimeOut (Optional)" ) ) ;
	m_ui->lineEditMountOptions->clear() ;
	m_ui->lineEditSshPortNumber->clear() ;
	m_ui->lineEditSshPortNumber->setEnabled( false ) ;
	m_ui->pbFolderPath->setEnabled( true ) ;
	m_ui->pbIdentityFile->setVisible( false ) ;
	m_ui->labelPortyNumber->clear() ;

	if( engine.known() ){

		auto s = m_ui->lineEditMountOptions->toPlainText() ;

		if( s.isEmpty() ){

			m_ui->lineEditMountOptions->setText( engine.defaultFavoritesMountOptions() ) ;
		}

		if( engine.usesOnlyMountPoint() ){

			m_ui->pbFolderPath->setIcon( QIcon( ":/folder.png" ) ) ;

			m_ui->pbFolderPath->setObjectName( "FolderHandle" ) ;
			m_ui->labelName->setText( tr( "Mount Point Path" ) ) ;

			m_ui->lineEditEncryptedFolderPath->setText( m_settings.mountPath( engine.uiName() ) ) ;

			return ;

		}else if( engine.fileExtensions().size() > 0 ){

			m_ui->pbFolderPath->setObjectName( "FileHandle" ) ;

			m_ui->pbFolderPath->setIcon( QIcon( ":/file.png" ) ) ;

			m_ui->labelName ->setText( tr( "File Path" ) ) ;

			return ;
		}
	}

	m_ui->pbFolderPath->setIcon( QIcon( ":/folder.png" ) ) ;
	m_ui->pbFolderPath->setObjectName( "FolderHandle" ) ;

	m_ui->labelName ->setText( tr( "Encrypted Folder Path" ) ) ;
}

void favorites2::ShowUI()
{
	m_ui->lineEditEncryptedFolderPath->clear() ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPath->setText( utility::freeWindowsDriveLetter() ) ;
	}else{
		m_ui->lineEditMountPath->setText( m_settings.mountPath() ) ;
	}

	if( m_engine.likeSsh() ){

		m_ui->tabWidget->setCurrentIndex( 1 ) ;

		this->setUiLikeSsh( m_cipherPath,m_engine ) ;

		m_ui->pbEdit->setEnabled( false ) ;
	}else{
		m_ui->pbIdentityFile->setVisible( false ) ;
		m_ui->tabWidget->setCurrentIndex( 0 ) ;
		m_ui->tabWidget_2->setCurrentIndex( 0 ) ;
	}

	if( !m_cipherPath.isEmpty() ){

		m_ui->tabWidget->setCurrentIndex( 1 ) ;

		if( !m_engine.hasConfigFile() ){

			m_ui->lineEditVolumeType->setText( m_engine.uiName() ) ;
		}
	}

	m_ui->tableWidget->setFocus() ;

	m_ui->cbAllowExternalToolsToReadPasswords->setChecked( m_settings.allowExternalToolsToReadPasswords() ) ;

	m_ui->cbAutoMountAtStartUp->setChecked( m_settings.autoMountFavoritesOnStartUp() ) ;

	m_ui->cbShowMountDialogWhenAutoMounting->setChecked( m_settings.showMountDialogWhenAutoMounting() ) ;

	m_ui->cbShowFavoriteListOnContextMenu->setChecked( m_settings.showFavoritesInContextMenu() ) ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

bool favorites2::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}
