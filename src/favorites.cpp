/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "favorites.h"
#include "ui_favorites.h"
#include <iostream>

#include <QTableWidgetItem>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>

#include "utility.h"
#include "dialogmsg.h"
#include "tablewidget.h"
#include "settings.h"

favorites::favorites( QWidget * parent,favorites::type type ) : QDialog( parent ),
	m_ui( new Ui::favorites )
{
	m_ui->setupUi( this ) ;

	this->setFont( parent->font() ) ;

	connect( m_ui->pbConfigFilePath,SIGNAL( clicked() ),this,SLOT( configPath() ) ) ;
	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pbFolderPath,SIGNAL( clicked() ),this,SLOT( folderPath() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

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

	m_ui->cbReadOnlyMode->setEnabled( !utility::platformIsWindows() ) ;

	m_ui->pbAdd->setObjectName( "Add" ) ;

	if( utility::platformIsWindows() ){

		utility::setWindowsMountPointOptions( this,m_ui->lineEditMountPath,m_ui->pbMountPointPath ) ;
	}else{
		m_ui->pbMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

		connect( m_ui->lineEditEncryptedFolderPath,SIGNAL( textChanged( QString ) ),
			 this,SLOT( devicePathTextChange( QString ) ) ) ;
		connect( m_ui->pbMountPointPath,SIGNAL( clicked() ),this,SLOT( mountPointPath() ) ) ;
	}

	m_ui->pbFolderPath->setIcon( QIcon( ":/sirikali.png" ) ) ;
	m_ui->pbConfigFilePath->setIcon( QIcon( ":/file.png" ) ) ;
	m_ui->pbIdentityFile->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->cbAutoMount->setChecked( false ) ;

	auto table = m_ui->tableWidget ;

	table->setColumnWidth( 0,285 ) ;
	table->setColumnWidth( 1,285 ) ;
	table->setColumnWidth( 2,100 ) ;
	table->setColumnWidth( 3,140 ) ;
	table->setColumnWidth( 4,115 ) ;
	table->setColumnWidth( 5,115 ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Enter,Qt::Key_Return,Qt::Key_Menu } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( shortcutPressed() ) ) ;

		return ac ;
	}() ) ;

	this->installEventFilter( this ) ;

	settings::instance().setParent( parent,&m_parentWidget,this ) ;

	utility::setWindowOptions( this ) ;

	this->checkFavoritesConsistency() ;

	this->ShowUI( type ) ;
}

void favorites::checkFavoritesConsistency()
{
	int s = m_ui->tableWidget->columnCount() - settings::instance().favoritesEntrySize() ;

	if( s == 0 ){

		/*
		 * It is ok,the size of the table is the same as what is in favorites
		 */
	}else if( s > 0 ){

		/*
		 * We will get here when a column was added in the table and we need to update
		 * favorites entries to accomodate it.
		 */

		auto e = settings::instance().readFavorites() ;

		settings::instance().clearFavorites() ;

		for( const auto& it : e ){

			settings::instance().addToFavorite( it.configStringList() ) ;
		}
	}else{
		/*
		 * We will get here when a column was removed from the table and we need to update
		 * favorites entries to accomodate it.
		 */
	}
}

bool favorites::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void favorites::devicePathTextChange( QString txt )
{
	if( txt.isEmpty() ){

		m_ui->lineEditMountPath->clear() ;
	}else{
		auto s = txt.split( "/" ).last() ;

		if( s.isEmpty() ){

			m_ui->lineEditMountPath->setText( txt ) ;
		}else{
			auto m = settings::instance().mountPath( s ) ;
			m_ui->lineEditMountPath->setText( m ) ;
		}
	}
}

void favorites::shortcutPressed()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void favorites::ShowUI( favorites::type type )
{
	m_type = type ;

	m_ui->tableWidget->setColumnWidth( 0,285 ) ;
	m_ui->tableWidget->setColumnWidth( 1,285 ) ;

	tablewidget::clearTable( m_ui->tableWidget ) ;

	auto _add_entry = [ this ]( const QStringList& l ){

		if( l.size() > 1 ){

			this->addEntries( l ) ;
		}
	} ;

	for( const auto& it : settings::instance().readFavorites() ){

		_add_entry( it.list() ) ;
	}

	m_ui->lineEditEncryptedFolderPath->clear() ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPath->setText( "Z:" ) ;
	}else{
		m_ui->lineEditMountPath->clear() ;
	}

	if( m_type == favorites::type::sshfs ){

		m_ui->lineEditMountOptions->setText( "idmap=user,StrictHostKeyChecking=no" ) ;
		m_ui->lineEditEncryptedFolderPath->setText( "sshfs " ) ;
		m_ui->labelName ->setText( tr( "Remote Ssh Server Address\n(Example: sshfs woof@bar.foo:/remote/path)" ) ) ;
		m_ui->labelCofigFilePath->setText( tr( "SSH_AUTH_SOCK Socket Path (Optional)" ) ) ;
		m_ui->labelIdleTimeOut->setText( tr( "IdentityFile Path (Optional)" ) ) ;
	}else{
		m_ui->pbIdentityFile->setVisible( false ) ;
	}

	m_ui->tableWidget->setFocus() ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

void favorites::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void favorites::addEntries( const QStringList& l )
{
	tablewidget::addRow( m_ui->tableWidget,l ) ;
}

void favorites::itemClicked( QTableWidgetItem * current )
{
	this->itemClicked( current,true ) ;
}

void favorites::edit()
{
	auto table = m_ui->tableWidget ;
	m_editRow = table->currentRow() ;

	if( m_editRow >= 0 ){

		m_ui->lineEditEncryptedFolderPath->clear() ;
		m_ui->lineEditMountPath->clear() ;
		m_ui->lineEditConfigFilePath->clear() ;
		m_ui->lineEditIdleTimeOut->clear() ;
		m_ui->lineEditMountOptions->clear() ;

		m_ui->pbAdd->setText( tr( "Edit" ) ) ;
		m_ui->pbAdd->setObjectName( "Edit" ) ;
		m_ui->lineEditEncryptedFolderPath->setText( table->item( m_editRow,0 )->text() ) ;
		m_ui->lineEditMountPath->setText( table->item( m_editRow,1 )->text() ) ;
		m_ui->cbAutoMount->setChecked( table->item( m_editRow,2 )->text() == "true" ) ;

		auto a = table->item( m_editRow,3 )->text() ;
		auto b = table->item( m_editRow,4 )->text() ;
		auto c = table->item( m_editRow,5 )->text() ;

		if( a != "N/A" ){

			m_ui->lineEditConfigFilePath->setText( a ) ;
		}

		if( b != "N/A" ){

			m_ui->lineEditIdleTimeOut->setText( b ) ;
		}

		m_ui->cbReverseMode->setChecked( c.contains( favorites::reverseModeOption ) ) ;
		m_ui->cbVolumeNoPassword->setChecked( c.contains( favorites::volumeNeedNoPassword ) ) ;
		m_ui->cbReadOnlyMode->setChecked( c.contains( favorites::mountReadOnly ) ) ;

		if( c != "N/A" ){

			QString aa = favorites::reverseModeOption ;
			QString bb = favorites::volumeNeedNoPassword ;
			QString cc = favorites::mountReadOnly ;

			m_ui->lineEditMountOptions->setText( utility::removeOption( c,aa,bb,cc ) ) ;
		}
	}
}

void favorites::itemClicked( QTableWidgetItem * current,bool clicked )
{
	if( current ){

		QMenu m ;

		m.setFont( this->font() ) ;

		connect( m.addAction( tr( "Toggle AutoMount" ) ),
			 SIGNAL( triggered() ),this,SLOT( toggleAutoMount() ) ) ;

		m.addSeparator() ;

		connect( m.addAction( tr( "Edit" ) ),
			 SIGNAL( triggered() ),this,SLOT( edit() ) ) ;

		m.addSeparator() ;

		connect( m.addAction( tr( "Remove Selected Entry" ) ),
			 SIGNAL( triggered() ),this,SLOT( removeEntryFromFavoriteList() ) ) ;

		m.addSeparator() ;

		m.addAction( tr( "Cancel" ) ) ;

		if( clicked ){

			m.exec( QCursor::pos() ) ;
		}else{
			int x = m_ui->tableWidget->columnWidth( 0 ) ;
			int y = m_ui->tableWidget->rowHeight( current->row() ) * current->row() + 20 ;
			m.exec( m_ui->tableWidget->mapToGlobal( QPoint( x,y ) ) ) ;
		}
	}
}

favorites::entry favorites::getEntry( int row )
{
	QStringList e ;

	auto table = m_ui->tableWidget ;

	for( int i = 0 ; i < table->columnCount() ; i++ ){

		e.append( table->item( row,i )->text() ) ;
	}

	return e ;
}

void favorites::toggleAutoMount()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		auto row = table->currentRow() ;

		auto item = table->item( row,2 ) ;

		auto e = this->getEntry( row ) ;

		item->setText( [ & ](){

			if( item->text() == "true" ){

				return "false" ;
			}else{
				return "true" ;
			}
		}() ) ;

		auto f = this->getEntry( row ) ;

		settings::instance().replaceFavorite( e,f ) ;
	}
}

void favorites::removeEntryFromFavoriteList()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		table->setEnabled( false ) ;

		auto row = table->currentRow() ;

		settings::instance().removeFavoriteEntry( this->getEntry( row ) ) ;

		tablewidget::deleteRow( table,row ) ;

		table->setEnabled( true ) ;
	}
}

void favorites::cancel()
{
	this->HideUI() ;
}

void favorites::add()
{
	DialogMsg msg( m_parentWidget,this ) ;

	auto dev = m_ui->lineEditEncryptedFolderPath->text() ;
	auto path = m_ui->lineEditMountPath->text() ;
	auto mOpts = m_ui->lineEditMountOptions->text() ;

	if( m_reverseMode ){

		if( mOpts.isEmpty() ){

			mOpts = favorites::reverseModeOption ;
		}else{
			mOpts += QString( "," ) + favorites::reverseModeOption ;
		}
	}

	if( m_volumeNeedNoPassword ){

		if( mOpts.isEmpty() ){

			mOpts = favorites::volumeNeedNoPassword ;
		}else{
			mOpts += QString( "," ) + favorites::volumeNeedNoPassword ;
		}
	}

	if( m_mountReadOnly ){

		if( mOpts.isEmpty() ){

			mOpts = favorites::mountReadOnly ;
		}else{
			mOpts += QString( "," ) + favorites::mountReadOnly ;
		}
	}

	if( dev.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Encrypted Folder Address Field Is Empty" ) ) ;
	}
	if( path.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount Point Path Field Is Empty" ) ) ;
	}

	m_ui->tableWidget->setEnabled( false ) ;

	auto _option = []( const QString& e )->QString{

		if( e.isEmpty() ){

			return "N/A" ;
		}else{
			return e ;
		}
	} ;

	auto autoMount = [ this ](){

		if( m_ui->cbAutoMount->isChecked() ){

			return "true" ;
		}else{
			return "false" ;
		}
	}() ;

	auto configPath = m_ui->lineEditConfigFilePath->text() ;
	auto idleTimeOUt = m_ui->lineEditIdleTimeOut->text() ;

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

	QStringList e = { dev,
			  path,
			  autoMount,
			  _option( configPath ),
			  _option( idleTimeOUt ),
			  _option( mOpts ) } ;

	if( m_ui->pbAdd->objectName() == "Edit" ){

		m_ui->pbAdd->setText( tr( "Add" ) ) ;
		m_ui->pbAdd->setObjectName( "Add" ) ;

		auto f = this->getEntry( m_editRow ) ;

		settings::instance().replaceFavorite( f,e ) ;

		tablewidget::updateRow( m_ui->tableWidget,e,m_editRow ) ;

		m_ui->lineEditEncryptedFolderPath->clear() ;
		m_ui->lineEditMountPath->clear() ;
		m_ui->lineEditConfigFilePath->clear() ;
		m_ui->lineEditIdleTimeOut->clear() ;
		m_ui->lineEditMountOptions->clear() ;
	}else{
		tablewidget::addRow( m_ui->tableWidget,e ) ;
		settings::instance().addToFavorite( e ) ;
		m_ui->lineEditEncryptedFolderPath->clear() ;
		m_ui->lineEditMountPath->clear() ;
		m_ui->lineEditConfigFilePath->clear() ;
		m_ui->lineEditIdleTimeOut->clear() ;
		m_ui->lineEditMountOptions->clear() ;
	}

	m_ui->cbReverseMode->setChecked( false ) ;
	m_ui->cbReadOnlyMode->setChecked( false ) ;
	m_ui->cbVolumeNoPassword->setChecked( false ) ;
	m_ui->cbAutoMount->setChecked( false ) ;

	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void favorites::configPath()
{
	auto e = this->getExistingFile( tr( "Path To A Config File" ) ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditConfigFilePath->setText( e ) ;
	}
}

QString favorites::getExistingFile( const QString& r )
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

QString favorites::getExistingDirectory( const QString& r )
{
	return utility::getExistingDirectory( this,r,QDir::homePath() ) ;
}

void favorites::folderPath()
{
	auto e = this->getExistingDirectory( tr( "Path To An Encrypted Folder" ) ) ;

	if( !e.isEmpty() ){

		m_ui->lineEditEncryptedFolderPath->setText( e ) ;
	}
}

void favorites::mountPointPath()
{
	auto e = this->getExistingDirectory( tr( "Path To Mount Folder" ) ) ;

	if( !e.isEmpty() ){

		auto z = m_ui->lineEditMountPath->text() ;

		if( !z.isEmpty() ){

			z = z.split( '/' ).last() ;

			m_ui->lineEditMountPath->setText( e + "/" + z ) ;
		}
	}
}

favorites::~favorites()
{
	delete m_ui ;
}

void favorites::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

void favorites::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

favorites::entry::entry()
{
}

favorites::entry::entry( const QStringList& e )
{
	this->config( e ) ;
}

favorites::entry::entry( const QString& r )
{
	this->config( r.split( '\t',QString::SkipEmptyParts ) ) ;
}

QStringList favorites::entry::list( bool e ) const
{
	if( e ){

		return this->configString().split( '\t',QString::SkipEmptyParts ) ;
	}else{
		return { volumePath,
			 mountPointPath,
			 autoMountVolume,
			 configFilePath,
			 idleTimeOut,
			 mountOptions } ;
	}
}

QString favorites::entry::string( char s ) const
{
	return this->list().join( QString( s ) ) ;
}

QString favorites::entry::configString() const
{
	auto _opt = [ ]( const QString& e )->QString{

		if( e.isEmpty() ){

			return "N/A" ;
		}else{
			return e ;
		}
	} ;

	auto e = "%1\t%2\t%3\t%4\t%5\t%6\t" ;

	return QString( e ).arg( volumePath,mountPointPath,autoMountVolume,
				 _opt( configFilePath ),_opt( idleTimeOut ),
				 _opt( mountOptions ) ) ;
}

QStringList favorites::entry::configStringList() const
{
	return this->configString().split( "\t",QString::SkipEmptyParts ) ;
}

bool favorites::entry::operator!=( const favorites::entry& other ) const
{
	return !( *this == other ) ;
}

bool favorites::entry::operator==( const favorites::entry& other ) const
{
	return  this->volumePath      == other.volumePath &&
		this->mountPointPath  == other.mountPointPath &&
		this->autoMountVolume == other.autoMountVolume &&
		this->configFilePath  == other.configFilePath &&
		this->idleTimeOut     == other.idleTimeOut &&
		this->readOnlyMode    == other.readOnlyMode &&
		this->mountOptions    == other.mountOptions &&
		this->reverseMode     == other.reverseMode &&
		this->volumeNeedNoPassword == other.volumeNeedNoPassword ;
}

bool favorites::entry::autoMount() const
{
	return autoMountVolume == "true" ;
}

QString favorites::entry::sanitizedMountOptions() const
{
	return sanitizedMountOptions( mountOptions ) ;
}

QString favorites::entry::sanitizedMountOptions( const QString& s )
{
	auto l = s.split( ',',QString::SkipEmptyParts ) ;

	l.removeAll( favorites::reverseModeOption ) ;
	l.removeAll( favorites::volumeNeedNoPassword ) ;
	l.removeAll( favorites::mountReadOnly ) ;

	return l.join( "," ) ;
}

void favorites::entry::config( const QStringList& e )
{
	utility2::stringListToStrings( e,
				       volumePath,
				       mountPointPath,
				       autoMountVolume,
				       configFilePath,
				       idleTimeOut,
				       mountOptions ) ;

	if( configFilePath == "N/A" ){

		configFilePath.clear() ;
	}

	if( idleTimeOut == "N/A" ){

		idleTimeOut.clear() ;
	}

	if( mountOptions == "N/A" ){

		mountOptions.clear() ;
	}

	reverseMode          = mountOptions.contains( favorites::reverseModeOption ) ;
	volumeNeedNoPassword = mountOptions.contains( favorites::volumeNeedNoPassword ) ;

	if( mountOptions.contains( favorites::mountReadOnly ) ){

		readOnlyMode = readOnly( true ) ;
	}
}

favorites::entry::readOnly::readOnly() : m_isSet( false )
{
}

favorites::entry::readOnly::readOnly( bool e ) : m_readOnlyVolume( e ),m_isSet( true )
{
}

bool favorites::entry::readOnly::onlyRead() const
{
	return m_readOnlyVolume ;
}

bool favorites::entry::readOnly::operator==( const favorites::entry::readOnly& other )
{
	if( other.m_isSet && this->m_isSet ){

		return this->m_readOnlyVolume == other.m_readOnlyVolume ;
	}else{
		return false ;
	}
}

favorites::entry::readOnly::operator bool() const
{
	return m_isSet;
}
