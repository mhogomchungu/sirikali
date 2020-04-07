/*
 *
 *  Copyright (c) 2020
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

#include "fscryptcreateoptions.h"
#include "ui_fscryptcreateoptions.h"

fscryptcreateoptions::fscryptcreateoptions( QWidget * parent,
					    engines::engine::functionOptions function,
					    const QStringList& list ) :
	QDialog( parent ),
	m_ui( new Ui::fscryptcreateoptions ),
	m_function( std::move( function ) ),
	m_protectorNames( list )
{
	m_ui->setupUi( this ) ;

	m_ui->rbCustomKey->setChecked( true ) ;

	m_ui->rbRawKey->setEnabled( false ) ;

	connect( m_ui->rbLoginPassPhrase,&QRadioButton::toggled,[ this ]( bool e ){

		if( !e ){

			m_ui->lineEdit->clear() ;
		}

		m_ui->lineEdit->setEnabled( !e ) ;
	} ) ;

	connect( m_ui->rbCustomKey,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->lineEdit->setFocus() ;
		}
	} ) ;

	connect( m_ui->pbCancel,&QPushButton::pressed,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->pbOK,&QPushButton::pressed,[ this ](){

		auto m = m_ui->lineEdit->text() ;

		auto _add = [ & ]( const QString& opt ){

			if( m.isEmpty() ){

				m = opt ;
			}else{
				m += "," + opt ;
			}
		} ;

		if( !m.isEmpty() ){

			m = "--name=" + m ;
		}

		if( m_ui->rbCustomKey->isChecked() ){

			_add( "--source=custom_passphrase" ) ;

		}else if( m_ui->rbLoginPassPhrase->isChecked() ){

			_add( "--source=pam_passphrase" ) ;

		}else if( m_ui->rbRawKey->isChecked() ){

			_add( "--source=raw_key" ) ;
		}

		this->HideUI( QStringList{ m } ) ;
	} ) ;

	m_ui->lineEdit->setFocus() ;

	this->show() ;
}

fscryptcreateoptions::~fscryptcreateoptions()
{
	delete m_ui ;
}

void fscryptcreateoptions::HideUI( const engines::engine::Options& opt )
{
	this->hide() ;

	m_function( opt ) ;

	this->deleteLater() ;
}

void fscryptcreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->HideUI() ;
}
