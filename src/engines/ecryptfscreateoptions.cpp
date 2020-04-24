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

#include "ecryptfscreateoptions.h"
#include "ui_ecryptfscreateoptions.h"

#include "../utility.h"
#include "task.hpp"

#include "../settings.h"

ecryptfscreateoptions::ecryptfscreateoptions( const engines::engine& e,
					      const engines::engine::createGUIOptions& s ) :
	QDialog( s.parent ),
	m_ui( new Ui::ecryptfscreateoptions ),
	m_configFileName( e.configFileName() ),
	m_function( s.fCreateOptions )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbConfigPath,SIGNAL( clicked() ),this,SLOT( pbSelectConfigPath() ) ) ;

	m_ui->pbConfigPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->rbEncryptFileNames->setChecked( true ) ;
	m_ui->rbDoNotEnablePlainText->setChecked( true ) ;

	m_ui->groupBox->setEnabled( settings::instance().ecryptfsAllowNotEncryptingFileNames() ) ;

	this->show() ;
}

ecryptfscreateoptions::~ecryptfscreateoptions()
{
	delete m_ui ;
}

void ecryptfscreateoptions::pbSelectConfigPath()
{
	m_ui->lineEdit_2->setText( utility::configFilePath( this,m_configFileName ) ) ;
}

void ecryptfscreateoptions::pbOK()
{
	QString e = ecryptfscreateoptions::defaultMiniCreateOptions() ;

	if( m_ui->rbDoNotEnablePlainText->isChecked() ){

		e += ",ecryptfs_passthrough=n" ;
	}else{
		e += ",ecryptfs_passthrough=y" ;
	}

	if( m_ui->rbEncryptFileNames->isChecked() ){

		e += ",ecryptfs_enable_filename_crypto=y" ;
	}else{
		e += ",ecryptfs_enable_filename_crypto=n" ;
	}

	this->HideUI( { e,m_ui->lineEdit_2->text(),QString() } ) ;
}

void ecryptfscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void ecryptfscreateoptions::HideUI( const engines::engine::cOpts& opts )
{
	this->hide() ;
	m_function( opts ) ;
	this->deleteLater() ;
}

void ecryptfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
