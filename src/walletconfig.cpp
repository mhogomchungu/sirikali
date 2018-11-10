/*
 *  Copyright ( c ) 2012-2015
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

#include "walletconfig.h"
#include "ui_walletconfig.h"

#include <QCloseEvent>

#include "dialogmsg.h"
#include "tablewidget.h"
#include "walletconfiginput.h"
#include "utility.h"
#include "task.hpp"
#include "settings.h"

#include <Qt>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#define COMMENT "-SiriKali_Comment_ID"

walletconfig::walletconfig( QWidget * parent,secrets::wallet&& wallet,std::function< void() > e ) :
	QDialog( parent ),
	m_ui( new Ui::walletconfig ),
	m_wallet( std::move( wallet ) ),
	m_function( std::move( e ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	settings::instance().setParent( parent,&m_parentWidget,this ) ;

	this->setFont( parent->font() ) ;

	m_ui->tableWidget->setColumnWidth( 0,386 ) ;
	m_ui->tableWidget->setColumnWidth( 1,237 ) ;
	m_ui->tableWidget->hideColumn( 2 ) ;

	connect( m_ui->pbAdd,SIGNAL( clicked() ),this,SLOT( pbAdd() ) ) ;
	connect( m_ui->pbClose,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pbDelete,SIGNAL( clicked() ),this,SLOT( pbDelete() ) ) ;
	connect( m_ui->tableWidget,SIGNAL( currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ),
		 this,SLOT(currentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ) ) ;
	connect( m_ui->tableWidget,SIGNAL( itemClicked( QTableWidgetItem * ) ),
		 this,SLOT( itemClicked( QTableWidgetItem * ) ) ) ;

	this->installEventFilter( this ) ;

	this->disableAll() ;

	if( m_wallet->opened() ){

		this->accessWallet() ;
	}else{
		m_wallet->setImage( QIcon( ":/sirikali" ) ) ;

		auto a = settings::instance().walletName( m_wallet->backEnd() ) ;
		auto b = settings::instance().applicationName() ;

		m_wallet->open( a,b,[ this ]( bool opened ){

			if( opened ){

				this->accessWallet() ;
			}else{
				this->HideUI() ;
			}
		} ) ;
	}
}

bool walletconfig::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void walletconfig::currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous )
{
	tablewidget::selectRow( current,previous ) ;
}

void walletconfig::itemClicked( QTableWidgetItem * item )
{
	Q_UNUSED( item ) ;

	QMenu m ;

	m.addAction( tr( "Delete Entry" ),this,SLOT( pbDelete() ) ) ;

	m.exec( QCursor::pos() ) ;
}

void walletconfig::itemClicked_0( QTableWidgetItem * item )
{
	this->disableAll() ;

	m_row = item->row() ;

	m_volumeID = m_ui->tableWidget->item( m_row,0 )->text() ;

	int r = DialogMsg( m_parentWidget,this ).ShowUIYesNo( tr( "WARNING!" ),tr( "Are you sure you want to delete a volume with an id of \"%1\"?" ).arg( m_volumeID ) ) ;

	if( r == QMessageBox::Yes ){

		Task::run( [ this ](){

			m_wallet->deleteKey( m_volumeID ) ;
			m_wallet->deleteKey( m_volumeID + COMMENT ) ;

		} ).then( [ this ](){

			tablewidget::deleteRow( m_ui->tableWidget,m_row ) ;
			this->enableAll() ;
			m_ui->tableWidget->setFocus() ;
		} ) ;
	}else{
		this->enableAll() ;
		m_ui->tableWidget->setFocus() ;
	}
}

void walletconfig::pbDelete()
{
	auto item = m_ui->tableWidget->currentItem() ;

	if( item ){

		this->itemClicked_0( item ) ;
	}
}

void walletconfig::pbClose()
{
	this->HideUI() ;
}

void walletconfig::pbAdd()
{
	this->disableAll() ;

	walletconfiginput::instance( m_parentWidget,this,[ this ]( const QString& volumeID,
				     const QString& comment,const QString& key ){

		m_comment  = comment ;
		m_volumeID = volumeID ;
		m_key      = key ;

		Task::run( [ this ](){

			auto _add = [ this ](){

				if( m_wallet->addKey( m_volumeID,m_key ) ){

					if( m_wallet->addKey( m_volumeID + COMMENT,m_comment ) ){

						return true ;
					}else{
						m_wallet->deleteKey( m_volumeID ) ;

						return false ;
					}
				}else{
					return false ;
				}
			} ;

			for( int i = 0 ; i < 2 ; i++ ){

				if( _add() ){

					return true ;
				}else{
					utility::Task::waitForTwoSeconds() ;
				}
			}

			return false ;

		} ).then( [ this ]( bool success ){

			if( success ){

				tablewidget::addRow( m_ui->tableWidget,{ m_volumeID,m_comment } ) ;
			}else{
				DialogMsg( m_parentWidget,this ).ShowUIOK( tr( "ERROR!" ),tr( "Failed To Add the Key In The Wallet." ) ) ;
			}

			this->enableAll() ;
			this->raise() ;
			this->activateWindow() ;
			m_ui->tableWidget->setFocus() ;
		} ) ;

	},[ this ](){

		this->enableAll() ;
		this->raise() ;
		this->activateWindow() ;
	} ) ;
}

void walletconfig::accessWallet()
{
	using walletKeys = decltype( m_wallet->readAllKeyValues() ) ;

	utility::setWindowOptions( this ) ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;

	Task::run( [ this ](){

		return m_wallet->readAllKeyValues() ;

	} ).then( [ this ]( const walletKeys& keys ){

		auto _getEntry = [&]( const QString& acc )-> const QByteArray& {

			for( const auto& it : keys ){

				if( it.first == acc ){

					return it.second ;
				}
			}

			static QByteArray ShouldNotGetHere ;
			return ShouldNotGetHere ;
		} ;

		/*
		 * each volume gets two entries in wallet:
		 * First one in the form of  : entry         -> entry password
		 * Second one in the form of : entry-COMMENT -> comment
		 *
		 * This allows to store a a volume volume,a comment about the volume
		 * and the passphrase.
		 *
		 */

		auto table = m_ui->tableWidget ;

		for( const auto& it : keys ){

			const auto& acc = it.first ;

			if( !acc.endsWith( COMMENT ) ){

				const auto& e = _getEntry( acc + COMMENT ) ;

				tablewidget::addRow( table,{ acc,e } ) ;
			}
		}

		this->enableAll() ;
		m_ui->tableWidget->setFocus() ;
	} ) ;
}

void walletconfig::enableAll()
{
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->pbAdd->setEnabled( true ) ;
	m_ui->pbClose->setEnabled( true ) ;
	m_ui->pbDelete->setEnabled( true ) ;
	m_ui->tableWidget->setEnabled( true ) ;
}

void walletconfig::disableAll()
{
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->pbAdd->setEnabled( false ) ;
	m_ui->pbClose->setEnabled( false ) ;
	m_ui->pbDelete->setEnabled( false ) ;
	m_ui->tableWidget->setEnabled( false ) ;
}

void walletconfig::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
	m_function() ;
}

void walletconfig::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

walletconfig::~walletconfig()
{
	delete m_ui ;
}
