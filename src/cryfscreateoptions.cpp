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

static void _get_crypto_options( QComboBox * combobox )
{
	auto exe = utility::executableFullPath( "cryfs" ) + " --show-ciphers" ;

	auto e = utility::Task( exe,utility::systemEnvironment(),[](){},false ) ;

	if( e.success() ){

		auto s = e.splitOutput( '\n',false ) ;

		if( s.isEmpty() ){

			combobox->addItem( "aes-256-gcm" ) ;
		}else{
			combobox->addItems( s ) ;
		}
	}else{
		combobox->addItem( "aes-256-gcm" ) ;
	}
}

cryfscreateoptions::cryfscreateoptions( QWidget * parent,
					std::function< void( const QString& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::cryfscreateoptions ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	m_ui->lineEdit->setText( "32768" ) ;

	m_ui->comboBox->setFocus() ;

	auto& e = Task::run( [ this ](){ _get_crypto_options( m_ui->comboBox ) ; } ) ;

	e.then( [ this ](){ this->show() ; } ) ;
}

cryfscreateoptions::~cryfscreateoptions()
{
	delete m_ui ;
}

void cryfscreateoptions::pbOK()
{
	auto e = QString( "--cipher %1 --blocksize %2" ) ;

	auto s = m_ui->lineEdit->text() ;

	if( s.isEmpty() ){

		s = "32768" ;
	}

	m_function( e.arg( m_ui->comboBox->currentText(),s ) ) ;

	this->HideUI() ;
}

void cryfscreateoptions::pbCancel()
{
	m_function( QString() ) ;

	this->HideUI() ;
}

void cryfscreateoptions::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void cryfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
