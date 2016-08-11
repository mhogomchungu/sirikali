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

favorites::favorites( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::favorites )
{
	m_ui->setupUi( this ) ;

	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;
	this->setFont( parent->font() ) ;

	this->setFixedSize( this->size() ) ;

	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( add() ) ) ;
	connect( m_ui->pbFolderPath,SIGNAL( clicked() ),this,SLOT( folderPath() ) ) ;
	connect( m_ui->pbMountPointPath,SIGNAL( clicked() ),this,SLOT( mountPointPath() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( cancel() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),this,
		SLOT( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),this,
		SLOT( itemClicked( QTableWidgetItem * ) ) ) ;
	connect( m_ui->lineEditEncryptedFolderPath,SIGNAL( textChanged( QString ) ),this,SLOT( devicePathTextChange( QString ) ) ) ;

	m_ui->pbFolderPath->setIcon( QIcon( ":/sirikali.png" ) ) ;
	m_ui->pbMountPointPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->lineEditEncryptedFolderPath->setEnabled( false ) ;

	this->addAction( [ this ](){

		auto ac = new QAction( this ) ;

		ac->setShortcuts( { Qt::Key_Enter,Qt::Key_Return,Qt::Key_Menu } ) ;

		connect( ac,SIGNAL( triggered() ),this,SLOT( shortcutPressed() ) ) ;

		return ac ;
	}() ) ;

	this->installEventFilter( this ) ;

	this->ShowUI() ;
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

		_add_entry( utility::split( it,'\t' ) ) ;
	}

	m_ui->lineEditEncryptedFolderPath->clear() ;
	m_ui->lineEditMountPath->clear() ;
	m_ui->tableWidget->setFocus() ;

	this->show() ;
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

void favorites::removeEntryFromFavoriteList()
{
	auto table = m_ui->tableWidget ;

	if( table->rowCount() > 0 ){

		table->setEnabled( false ) ;

		auto row = table->currentRow() ;

		auto p = table->item( row,0 )->text() ;
		auto q = table->item( row,1 )->text() ;

		if( !p.isEmpty() && !q.isEmpty() ){

			utility::removeFavoriteEntry( QString( "%1\t%2" ).arg( p,q ) ) ;

			tablewidget::deleteRow( table,row ) ;
		}

		table->setEnabled( true ) ;
	}
}

void favorites::cancel()
{
	this->HideUI() ;
}

void favorites::add()
{
	DialogMsg msg( this ) ;

	auto dev = m_ui->lineEditEncryptedFolderPath->text() ;
	auto path = m_ui->lineEditMountPath->text() ;

	if( dev.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Encrypted folder address field is empty" ) ) ;
	}
	if( path.isEmpty() ){

		return msg.ShowUIOK( tr( "ERROR!" ),tr( "Mount point path field is empty" ) ) ;
	}

	m_ui->tableWidget->setEnabled( false ) ;

	this->addEntries( { dev,path } ) ;

	utility::addToFavorite( dev,path ) ;

	m_ui->lineEditEncryptedFolderPath->clear() ; ;
	m_ui->lineEditMountPath->clear() ;

	m_ui->tableWidget->setEnabled( true ) ;
}

QString favorites::getExistingDirectory( const QString& r )
{
	auto e = QFileDialog::getExistingDirectory( this,r,QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

	while( true ){

		if( e.endsWith( '/' ) ){

			e.truncate( e.length() - 1 ) ;
		}else{
			break ;
		}
	}

	return e ;
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
