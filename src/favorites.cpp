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

favorites::favorites( QWidget * parent ) : QDialog( parent ),
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

	//m_ui->lineEditEncryptedFolderPath->setEnabled( false ) ;

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

	utility::setParent( parent,&m_parentWidget,this ) ;

	utility::setWindowOptions( this ) ;

	this->checkFavoritesConsistency() ;

	this->ShowUI() ;
}

void favorites::checkFavoritesConsistency()
{
	int s = m_ui->tableWidget->columnCount() - utility::favoritesEntrySize() ;

	if( s == 0 ){

		/*
		 * It is ok,the size of the table is the same as what is in favorites
		 */
	}else if( s > 0 ){

		/*
		 * We will get here when a column was added in the table and we need to update
		 * favorites entries to accomodate it.
		 */

		auto e = utility::readFavorites() ;

		utility::clearFavorites() ;

		for( const auto& it : e ){

			utility::addToFavorite( it.configStringList() ) ;
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
			auto m = utility::mountPath( s ) ;
			m_ui->lineEditMountPath->setText( m ) ;
		}
	}
}

void favorites::shortcutPressed()
{
	this->itemClicked( m_ui->tableWidget->currentItem(),false ) ;
}

void favorites::ShowUI()
{
	m_ui->tableWidget->setColumnWidth( 0,285 ) ;
	m_ui->tableWidget->setColumnWidth( 1,285 ) ;

	tablewidget::clearTable( m_ui->tableWidget ) ;

	auto _add_entry = [ this ]( const QStringList& l ){

		if( l.size() > 1 ){

			this->addEntries( l ) ;
		}
	} ;

	for( const auto& it : utility::readFavorites() ){

		_add_entry( it.list() ) ;
	}

	m_ui->lineEditEncryptedFolderPath->clear() ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPath->setText( "Z:" ) ;
	}else{
		m_ui->lineEditMountPath->clear() ;
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

void favorites::itemClicked( QTableWidgetItem * current,bool clicked )
{
	if( current ){

		QMenu m ;

		m.setFont( this->font() ) ;

		connect( m.addAction( tr( "Toggle AutoMount" ) ),
			 SIGNAL( triggered() ),this,SLOT( toggleAutoMount() ) ) ;

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

		utility::replaceFavorite( e,f ) ;
	}
}

void favorites::removeEntryFromFavoriteList()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		table->setEnabled( false ) ;

		auto row = table->currentRow() ;

		utility::removeFavoriteEntry( this->getEntry( row ) ) ;

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

	if( dev.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Encrypted Folder Address Field Is Empty" ) ) ;
	}
	if( path.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount Point Path Field Is Empty" ) ) ;
	}
	if( !mOpts.isEmpty() ){

		if( mOpts.startsWith( "-" ) || mOpts.contains( ",-" ) ){

			return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount Options Can Not Contain \"-\"" ) ) ;
		}
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

	QStringList e = { dev,
			  path,
			  autoMount,
			  _option( m_ui->lineEditConfigFilePath->text() ),
			  _option( m_ui->lineEditIdleTimeOut->text() ),
			  _option( mOpts ) } ;

	this->addEntries( e ) ;

	utility::addToFavorite( e ) ;

	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;

	m_ui->tableWidget->setEnabled( true ) ;
}

void favorites::configPath()
{
	auto e = this->getExistingFile( tr( "Path To A Config File" ) ) ;

	m_ui->lineEditConfigFilePath->setText( e ) ;
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
