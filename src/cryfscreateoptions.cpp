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

#include "cryfscreateoptions.h"
#include "ui_cryfscreateoptions.h"

#include "utility.h"
#include "task.h"

cryfscreateoptions::cryfscreateoptions( QWidget * parent,
					std::function< void( const QStringList& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::cryfscreateoptions ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbConfigPath,SIGNAL( clicked() ),this,SLOT( pbSelectConfigPath() ) ) ;

	m_ui->pbConfigPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->lineEdit->setText( "32768" ) ;

	auto exe = utility::executableFullPath( "cryfs" ) + " --show-ciphers" ;

	utility::Task::run( exe ).then( [ this ]( const utility::Task& e ){

		if( e.success() ){

			auto s = e.splitOutput( '\n',utility::Task::channel::stdError ) ;

			if( s.isEmpty() ){

				m_ui->comboBox->addItem( "aes-256-gcm" ) ;
			}else{
				m_ui->comboBox->addItems( s ) ;
			}
		}else{
			m_ui->comboBox->addItem( "aes-256-gcm" ) ;
		}

		m_ui->comboBox->setFocus() ;

		this->show() ;
	} ) ;
}

cryfscreateoptions::~cryfscreateoptions()
{
	delete m_ui ;
}

void cryfscreateoptions::pbSelectConfigPath()
{
	m_ui->lineEdit_2->setText( utility::configFilePath( this,"cryfs" ) ) ;
}

void cryfscreateoptions::pbOK()
{
	auto e = QString( "--cipher %1 --blocksize %2" ) ;

	auto s = m_ui->lineEdit->text() ;

	if( s.isEmpty() ){

		s = "32768" ;
	}

	e = e.arg( m_ui->comboBox->currentText(),s ) ;

	this->HideUI( { e,m_ui->lineEdit_2->text() } ) ;
}

void cryfscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void cryfscreateoptions::HideUI( const QStringList& e )
{
	this->hide() ;

	m_function( e ) ;

	this->deleteLater() ;
}

void cryfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
