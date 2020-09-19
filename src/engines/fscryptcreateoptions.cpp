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

#include "../utility.h"

#include <QFileDialog>
#include <QDir>

fscryptcreateoptions::fscryptcreateoptions( const engines::engine::createGUIOptions& s,
					    const QStringList& list ) :
	QDialog( s.parent ),
	m_ui( new Ui::fscryptcreateoptions ),
	m_function( s.fCreateOptions ),
	m_protectorNames( list )
{
	m_ui->setupUi( this ) ;

	m_ui->rbCustomKey->setChecked( true ) ;

	m_ui->lineEditKeyFile->setEnabled( false ) ;
	m_ui->pbKeyFile->setEnabled( false ) ;

	m_ui->pbKeyFile->setIcon( QIcon( ":/file.png" ) ) ;

	connect( m_ui->pbKeyFile,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Select A 32 Byte KeyFile" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditKeyFile->setText( e ) ;
		}
	} ) ;

	connect( m_ui->rbLoginPassPhrase,&QRadioButton::toggled,[ this ]( bool e ){

		if( !e ){

			m_ui->lineEditProtectorName->clear() ;
		}

		m_ui->lineEditProtectorName ->setEnabled( !e ) ;
	} ) ;

	connect( m_ui->rbRawKey,&QRadioButton::toggled,[ this ]( bool e ){

		if( !e ){

			m_ui->lineEditKeyFile->clear() ;
		}

		m_ui->lineEditKeyFile->setEnabled( e ) ;
		m_ui->pbKeyFile->setEnabled( e ) ;
	} ) ;

	connect( m_ui->rbCustomKey,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ui->lineEditProtectorName->setFocus() ;
		}
	} ) ;

	connect( m_ui->pbCancel,&QPushButton::pressed,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->pbOK,&QPushButton::pressed,[ this ](){

		auto m = m_ui->lineEditProtectorName->text() ;

		auto mm = m_ui->lineEditKeyFile->text() ;

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

		if( !mm.isEmpty() ){

			_add( "--key=" + mm ) ;
		}

		this->HideUI( { m,QString(),QString() } ) ;
	} ) ;

	m_ui->lineEditProtectorName->setFocus() ;

	this->show() ;
}

fscryptcreateoptions::~fscryptcreateoptions()
{
	delete m_ui ;
}

void fscryptcreateoptions::HideUI( const engines::engine::cOpts& opt )
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
