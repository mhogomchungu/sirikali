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

#include "securefscreateoptions.h"
#include "ui_securefscreateoptions.h"

securefscreateoptions::securefscreateoptions( QWidget * parent,
					      std::function< void( const QString& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::securefscreateoptions ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	this->show() ;
}

securefscreateoptions::~securefscreateoptions()
{
	delete m_ui ;
}

void securefscreateoptions::pbOK()
{
	if( m_ui->comboBox->currentIndex() == 1 ){

		m_function( "--format 2" ) ;
	}else{
		m_function( QString() ) ;
	}

	this->HideUI() ;
}

void securefscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void securefscreateoptions::HideUI()
{
	this->hide() ;
	this->deleteLater() ;
}

void securefscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
