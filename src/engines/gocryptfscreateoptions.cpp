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

#include "gocryptfscreateoptions.h"
#include "ui_gocryptfscreateoptions.h"

#include "../utility.h"
#include "task.hpp"
#include "../engines.h"

gocryptfscreateoptions::gocryptfscreateoptions( QWidget * parent,
					std::function< void( const engines::engine::Options& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::gocryptfscreateoptions ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbConfigPath,SIGNAL( clicked() ),this,SLOT( pbSelectConfigPath() ) ) ;

	m_ui->pbConfigPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->comboBox->addItems( { "AES256-GCM","AES256-SIV" } ) ;

	m_ui->rbEncryptFileNames->setChecked( true ) ;

	m_ui->checkBox->setToolTip( tr( "Normally Gocryptfs provides a plaintext view of data on demand.\nIt stores enciphered data and displays plaintext data.\nWith this option set, it takes as source plaintext data and produces enciphered data on-demand.\nThis can be useful for creating remote encrypted backups,\nwhere you do not wish to keep the local files unencrypted." ) ) ;

	this->show() ;
}

gocryptfscreateoptions::~gocryptfscreateoptions()
{
	delete m_ui ;
}

void gocryptfscreateoptions::pbSelectConfigPath()
{
	m_ui->lineEdit_2->setText( utility::configFilePath( this,"gocryptfs" ) ) ;
}

void gocryptfscreateoptions::pbOK()
{
	auto a = [ this ](){

		if( m_ui->comboBox->currentIndex() == 1 ){

			return "-aessiv" ;
		}else{
			return "" ;
		}
	}() ;

	auto b = [ this ](){

		if( m_ui->rbDoNotEncryptFileNames->isChecked() ){

			return "-plaintextnames" ;
		}else{
			return "" ;
		}
	}() ;

	auto c = { QString( "%1 %2" ).arg( a,b ),m_ui->lineEdit_2->text() } ;

	engines::engine::options::booleanOptions opts ;

	opts.unlockInReverseMode = m_ui->checkBox->isChecked() ;

	this->HideUI( { { c },opts } ) ;
}

void gocryptfscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void gocryptfscreateoptions::HideUI( const engines::engine::Options& opts )
{
	this->hide() ;
	m_function( opts ) ;
	this->deleteLater() ;
}

void gocryptfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
