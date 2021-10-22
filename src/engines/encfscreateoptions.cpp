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

#include "encfscreateoptions.h"
#include "ui_encfscreateoptions.h"

#include "../utility.h"
#include "task.hpp"

#include "../engines.h"

encfscreateoptions::encfscreateoptions( const engines::engine::createGUIOptions& s ) :
	QDialog( s.parent ),
	m_ui( new Ui::encfscreateoptions ),
	m_function( s.fCreateOptions )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	m_ui->plainTextEdit->appendPlainText( tr( "Normally EncFS provides a plaintext view of data on demand: it stores enciphered data and displays plaintext data. With this option set, it takes as source plaintext data and produces enciphered data on-demand. This can be useful for creating remote encrypted backups, where you do not wish to keep the local files unencrypted." ) ) ;

	this->show() ;
}

encfscreateoptions::~encfscreateoptions()
{
	delete m_ui ;
}

void encfscreateoptions::pbSelectConfigPath()
{
}

void encfscreateoptions::pbOK()
{
	engines::engine::booleanOptions opts ;

	opts.unlockInReverseMode = m_ui->checkBox->isChecked() ;

	this->HideUI( { QString(),QString(),QString(),opts } ) ;
}

void encfscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void encfscreateoptions::HideUI( const engines::engine::cOpts& opts )
{
	this->hide() ;
	m_function( opts ) ;
	this->deleteLater() ;
}

void encfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
