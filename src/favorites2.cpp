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

#include <QFileDialog>

favorites2::favorites2( QWidget * parent,
			favorites::type type,
			std::function< void() > function,
			const QString& cp ) :
	QDialog ( parent) ,
	m_ui( new Ui::favorites2 ),
	m_type( type ),
	m_settings( settings::instance() ),
	m_function( std::move( function ) ),
	m_cipherPath( cp )
{
	m_ui->setupUi( this ) ;

	this->setFont( parent->font() ) ;

	this->setFixedSize( this->size() ) ;

	m_settings.setParent( parent,&m_parentWidget,this ) ;

	auto table = m_ui->tableWidget ;

	table->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui->pbVolumeTypes->setMenu( [ this ](){

		auto m = new QMenu( this ) ;

		for( const auto& it : engines::instance().enginesWithNoConfigFile() ){

			auto ac = m->addAction( it ) ;

			ac->setObjectName( it ) ;

			m->addAction( ac ) ;
		}

		connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

			m_ui->lineEditVolumeType->setText( ac->objectName() ) ;
		} ) ;

		return m ;
	}() ) ;

	connect( m_ui->pbConfigFilePath,&QPushButton::clicked,[ this ](){

		this->configPath() ;
	} ) ;

	connect( m_ui->pbAdd,&QPushButton::clicked,[ this ](){

		this->updateFavorite( false ) ;
	} ) ;

	connect( m_ui->pbFolderPath,&QPushButton::clicked,[ this ](){

		this->folderPath() ;
	} ) ;

	auto _setCommand = [ this ]( QLineEdit * lineEdit ){

		auto path = this->getExistingFile( tr( "Select A Command Executable" ) ) ;

		if( !path.isEmpty() ){

			lineEdit->setText( path ) ;
		}
	} ;

	connect( m_ui->pbPreMount,&QPushButton::clicked,[ = ](){

		_setCommand( m_ui->lineEditPreMount ) ;
	} ) ;

	connect( m_ui->pbPreUnMount,&QPushButton::clicked,[ = ](){

		_setCommand( m_ui->lineEditPreUnMount ) ;
	} ) ;

	connect( m_ui->pbPostMount,&QPushButton::clicked,[ = ](){

		_setCommand( m_ui->lineEditPostMount ) ;
	} ) ;

	connect( m_ui->pbPostUnmount,&QPushButton::clicked,[ = ](){

		_setCommand( m_ui->lineEditPostUnmount ) ;
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

	if( utility::platformIsWindows() ){

		utility::setWindowsMountPointOptions( this,m_ui->lineEditMountPath,m_ui->pbMountPointPath ) ;
		m_ui->labelDeviceAddress->setText( tr( "Mount Point Path" ) ) ;
	}else{
		m_ui->pbMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

		m_ui->labelDeviceAddress->setText( tr( "Mount Point Prefix" ) ) ;

		connect( m_ui->pbMountPointPath,&QPushButton::clicked, [ this ](){

			this->mountPointPath() ;
		} ) ;
	}

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( table,&QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		const auto volumes = favorites::instance().readFavorites() ;

		this->setVolumeProperties( volumes[ size_t( item->row() ) ] ) ;
	} ) ;

	connect( table,&QTableWidget::currentItemChanged,[]( QTableWidgetItem * x,QTableWidgetItem * y ){

		tablewidget::selectRow( x,y ) ;
	} ) ;

	m_ui->pbFolderPath->setIcon( QIcon( ":/sirikali.png" ) ) ;
	m_ui->pbConfigFilePath->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pbIdentityFile->setIcon( QIcon( ":/file.png" ) ) ;

	QIcon exeIcon( ":/executable.png" ) ;

	m_ui->pbPreMount->setIcon( exeIcon ) ;
	m_ui->pbPostMount->setIcon( exeIcon ) ;
	m_ui->pbPreUnMount->setIcon( exeIcon ) ;
	m_ui->pbPostUnmount->setIcon( exeIcon ) ;

	m_ui->cbAutoMount->setChecked( false ) ;

	m_ui->cbReadOnlyMode->setEnabled( utility::platformIsNOTWindows() ) ;

	this->checkFavoritesConsistency() ;

	const auto volumes = favorites::instance().readFavorites() ;

	this->updateVolumeList( volumes,0 ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Enter,Qt::Key_Return,Qt::Key_Menu } ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->shortcutPressed() ;
		} ) ;

		return ac ;
	}() ) ;

	this->setOptionMenu( m_optionMenu,false ) ;
	m_ui->pbOptions->setMenu( &m_optionMenu ) ;

	this->ShowUI( type ) ;
}

favorites2::~favorites2()
{
	delete m_ui ;
}

void favorites2::tabChanged( int index )
{
	if( index == 0 ){

		m_editMode = false ;
	}else{
		if( m_editMode ){

			m_ui->pbEdit->setEnabled( true ) ;
			m_ui->pbAdd->setEnabled( false ) ;
			m_ui->pbEdit->setFocus() ;
		}else{
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

			m_ui->pbAdd->setFocus() ;
		}
	}
}

template< typename M >
static void _updateList( QTableWidget * table,const QFont& font,
			 const std::vector<favorites::entry>& e,const M& m )
{
	tablewidget::clearTable( table ) ;

	for( const auto& it : e ){

		tablewidget::addRow( table,{ it.volumePath },font ) ;
	}

	tablewidget::selectRow( table,m ) ;
}

void favorites2::updateVolumeList( const std::vector<favorites::entry>& e,const QString& volPath )
{
	_updateList( m_ui->tableWidget,this->font(),e,volPath ) ;

	auto _update = [ & ]()->const favorites::entry&{

		for( const auto& it : e ){

			if( it.volumePath == volPath ){

				return it ;
			}
		}

		static favorites::entry m ;
		return m ;
	} ;

	this->setVolumeProperties( _update() ) ;
}

void favorites2::updateVolumeList( const std::vector< favorites::entry >& e,int row )
{
	_updateList( m_ui->tableWidget,this->font(),e,row ) ;

	if( size_t( row ) < e.size() ){

		this->setVolumeProperties( e[ size_t( row ) ] ) ;
	}else{
		this->setVolumeProperties( {} ) ;
	}
}

void favorites2::showUpdatedEntry( const favorites::entry& e )
{
	this->updateVolumeList( favorites::instance().readFavorites(),e.volumePath ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;
}

void favorites2::setOptionMenu( QMenu& m,bool addCancel )
{
	m.setFont( this->font() ) ;

	connect( m.addAction( tr( "Toggle AutoMount" ) ),&QAction::triggered,[ this ](){

		this->toggleAutoMount() ;
	} ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "Edit" ) ),&QAction::triggered,[ this ](){

		this->edit() ;
	} ) ;

	m.addSeparator() ;

	connect( m.addAction( tr( "Remove Selected Entry" ) ),&QAction::triggered,[ this ](){

		this->removeEntryFromFavoriteList() ;
	} ) ;

	m.addSeparator() ;

	if( addCancel ){

		m.addSeparator() ;

		m.addAction( tr( "Cancel" ) ) ;
	}
}

void favorites2::toggleAutoMount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		auto e = this->getEntry( row ) ;

		if( !e.volumePath.isEmpty() ){

			auto f = e ;

			f.autoMount.toggle() ;

			if( m_ui->textEditAutoMount->toPlainText() == "false" ){

				m_ui->textEditAutoMount->setText( "true" ) ;
			}else{
				m_ui->textEditAutoMount->setText( "false" ) ;
			}

			favorites::instance().replaceFavorite( e,f ) ;
		}
	}
}

void favorites2::edit()
{
	auto table = m_ui->tableWidget ;

	m_editRow = table->currentRow() ;
	m_editMode = true ;

	const auto volumes = favorites::instance().readFavorites() ;

	if( m_editRow >= 0 && m_editRow < int( volumes.size() ) ){

		this->clearEditVariables() ;

		auto entry = volumes[ size_t( m_editRow ) ] ;

		m_ui->lineEditEncryptedFolderPath->setText( entry.volumePath ) ;
		m_ui->lineEditMountPath->setText( entry.mountPointPath ) ;
		m_ui->cbAutoMount->setChecked( entry.autoMount.True() ) ;
		m_ui->lineEditPreMount->setText( entry.preMountCommand ) ;
		m_ui->lineEditPreUnMount->setText( entry.preUnmountCommand ) ;
		m_ui->lineEditPostMount->setText( entry.postMountCommand ) ;
		m_ui->lineEditPostUnmount->setText( entry.postUnmountCommand ) ;
		m_ui->lineEditMountOptions->setText( entry.mountOptions ) ;

		for( const auto& it : engines::instance().enginesWithNoConfigFile() ){

			auto s = m_ui->lineEditEncryptedFolderPath->toPlainText() ;

			if( s.startsWith( it + " ",Qt::CaseInsensitive ) ){

				m_ui->lineEditEncryptedFolderPath->setText( s.mid( it.size() + 1 ) ) ;
				m_ui->lineEditVolumeType->setText( it ) ;
				break ;
			}
		}

		m_ui->lineEditConfigFilePath->setText( entry.configFilePath ) ;
		m_ui->lineEditIdleTimeOut->setText( entry.idleTimeOut ) ;

		m_ui->cbReverseMode->setChecked( entry.reverseMode ) ;
		m_ui->cbVolumeNoPassword->setChecked( entry.volumeNeedNoPassword ) ;
		m_ui->cbReadOnlyMode->setChecked( entry.readOnlyMode.True() ) ;
		m_ui->cbVolumeNoPassword->setChecked( entry.volumeNeedNoPassword ) ;

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

		favorites::instance().removeFavoriteEntry( this->getEntry( row ) ) ;

		const auto volumes = favorites::instance().readFavorites() ;

		this->updateVolumeList( volumes,int( volumes.size() ) - 1 ) ;
	}
}

void favorites2::updateFavorite( bool edit )
{
	DialogMsg msg( m_parentWidget,this ) ;

	utility::hideQWidget thisWidget( this ) ;

	auto dev = m_ui->lineEditEncryptedFolderPath->toPlainText() ;
	auto path = m_ui->lineEditMountPath->toPlainText() ;
	auto mOpts = m_ui->lineEditMountOptions->toPlainText() ;

	if( dev.isEmpty() ){

		thisWidget.hide() ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Encrypted Folder Address Field Is Empty" ) ) ;
	}
	if( path.isEmpty() ){

		thisWidget.hide() ;
		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount Point Path Field Is Empty" ) ) ;
	}

	m_ui->tableWidget->setEnabled( false ) ;

	auto configPath = m_ui->lineEditConfigFilePath->toPlainText() ;
	auto idleTimeOUt = m_ui->lineEditIdleTimeOut->toPlainText() ;

	if( m_type == favorites::type::sshfs ){

		if( !configPath.isEmpty() ){

			if( mOpts.isEmpty() ){

				mOpts = "IdentityAgent=" + configPath ;
			}else{
				mOpts += ",IdentityAgent=" + configPath ;
			}
		}

		if( !idleTimeOUt.isEmpty() ){

			if( mOpts.isEmpty() ){

				mOpts = "IdentityFile=" + idleTimeOUt ;
			}else{
				mOpts += ",IdentityFile=" + idleTimeOUt ;
			}
		}

		configPath.clear() ;
		idleTimeOUt.clear() ;
	}

	auto dev_path = [ & ](){

		auto a = m_ui->lineEditVolumeType->toPlainText() ;

		if( a.isEmpty() ){

			return dev ;
		}else{
			if( dev.startsWith( a + " ",Qt::CaseInsensitive ) ){

				return dev ;
			}else{
				return a + " " + dev ;
			}
		}
	}() ;

	favorites::entry e ;
	e.volumePath = dev_path ;
	e.mountPointPath = path ;
	e.autoMount = m_ui->cbAutoMount->isChecked() ;
	e.configFilePath = configPath ;
	e.idleTimeOut = idleTimeOUt ;
	e.mountOptions = mOpts ;
	e.volumeNeedNoPassword = m_ui->cbVolumeNoPassword->isChecked() ;
	e.preMountCommand = m_ui->lineEditPreMount->text() ;
	e.preUnmountCommand = m_ui->lineEditPreUnMount->text() ;
	e.postMountCommand = m_ui->lineEditPostMount->text() ;
	e.postUnmountCommand = m_ui->lineEditPostUnmount->text() ;

	if( edit ){

		auto f = this->getEntry( m_editRow ) ;

		favorites::instance().replaceFavorite( f,e ) ;
	}else{
		if( utility::platformIsWindows() ){

			if( !utility::isDriveLetter( path ) ){

				e.mountPointPath = path + "/" + utility::split( dev,'/' ).last() ;
			}
		}else{
			e.mountPointPath = path + "/" + utility::split( dev,'/' ).last() ;
		}

		favorites::instance().add( e ) ;
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

void favorites2::devicePathTextChange( QString txt )
{
	if( txt.isEmpty() ){

		m_ui->lineEditMountPath->clear() ;
	}else{
		auto s = txt.split( "/" ).last() ;

		if( s.isEmpty() ){

			m_ui->lineEditMountPath->setText( txt ) ;
		}else{
			auto m = m_settings.mountPath( s ) ;
			m_ui->lineEditMountPath->setText( m ) ;
		}
	}
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

favorites::entry favorites2::getEntry( int row )
{
	size_t m = size_t( row ) ;

	const auto volumes = favorites::instance().readFavorites() ;

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

void favorites2::setVolumeProperties( const favorites::entry& e )
{
	m_ui->textEditMountPoint->setText( e.mountPointPath ) ;

	if( e.autoMount.defined() ){

		m_ui->textEditAutoMount->setText( e.autoMount.True() ? "true" : "false" ) ;
	}else{
		m_ui->textEditAutoMount->setText( QString() ) ;
	}

	m_ui->textEditConfigFilePath->setText( e.configFilePath ) ;

	m_ui->textEditIdleTimeOut->setText( e.idleTimeOut ) ;

	m_ui->textEditMountOptions->setText( e.mountOptions ) ;

	m_ui->textEditPreMount->setText( e.preMountCommand ) ;

	m_ui->textEditPostMount->setText( e.postMountCommand ) ;

	m_ui->textEditPreUnMount->setText( e.preUnmountCommand ) ;

	m_ui->textEditPostUnmount->setText( e.postUnmountCommand ) ;
}

void favorites2::ShowUI( favorites::type type )
{
	m_type = type ;

	m_ui->lineEditEncryptedFolderPath->clear() ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPath->setText( utility::freeWindowsDriveLetter() ) ;
	}else{
		m_ui->lineEditMountPath->setText( m_settings.mountPath() ) ;
	}

	if( m_type == favorites::type::sshfs ){

		m_ui->tabWidget->setCurrentIndex( 1 ) ;

		m_ui->lineEditMountOptions->setText( "idmap=user,StrictHostKeyChecking=no" ) ;
		m_ui->labelName ->setText( tr( "Remote Ssh Server Address\n(Example: woof@example.com:/remote/path)" ) ) ;
		m_ui->labelCofigFilePath->setText( tr( "SSH_AUTH_SOCK Socket Path (Optional)" ) ) ;
		m_ui->labelIdleTimeOut->setText( tr( "IdentityFile Path (Optional)" ) ) ;
		m_ui->lineEditVolumeType->setText( "Sshfs" ) ;
		m_ui->pbEdit->setEnabled( false ) ;
	}else{
		m_ui->pbIdentityFile->setVisible( false ) ;
		m_ui->tabWidget->setCurrentIndex( 0 ) ;
		m_ui->tabWidget_2->setCurrentIndex( 0 ) ;
	}

	if( !m_cipherPath.isEmpty() ){

		m_ui->tabWidget->setCurrentIndex( 1 ) ;
	}

	m_ui->tableWidget->setFocus() ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

void favorites2::addEntries( const QStringList& l )
{
	tablewidget::addRow( m_ui->tableWidget,l ) ;
}

bool favorites2::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}
