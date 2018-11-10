/*
 *
 *  Copyright ( c ) 2017
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

#include "filemanager.h"
#include "ui_filemanager.h"

#include "utility.h"
#include "settings.h"

fileManager::fileManager( QWidget * parent,QString& e,bool s ) :
	QDialog( parent ),
	m_ui( new Ui::fileManager ),
	m_fileManager( e ),
	m_setFileManager( s )
{
	m_ui->setupUi( this ) ;

	connect( m_ui->pushButton,SIGNAL( clicked( bool ) ),this,SLOT( set() ) ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	if( m_setFileManager ){

		m_ui->label->setText( tr( "Enter Below The Name Of The Application You Want To Be Used To Open Mount Points." ) ) ;

		m_ui->lineEdit->setText( settings::instance().fileManager() ) ;

		m_ui->lineEdit->setVisible( true ) ;
		m_ui->lineEdit_2->setVisible( false ) ;

		//m_ui->lineEdit->setFocus() ;

		this->setWindowTitle( tr( "Set File Manager" ) ) ;
	}else{
		this->setWindowTitle( tr( "Set Executable For \"ExternalExecutable\" Plugin" ) ) ;

		m_ui->label->setText( tr( "Enter Below The Name Of The External Executable And Its Arguments To Use When Invoking \"ExternalExecutable\" Plugin." ) ) ;

		m_ui->lineEdit->setVisible( false ) ;
		m_ui->lineEdit_2->setVisible( true ) ;

		m_ui->lineEdit_2->setText( settings::instance().externalPluginExecutable() ) ;

		//m_ui->lineEdit_2->setFocus() ;
	}

	m_ui->pushButton->setFocus() ;

	this->show() ;
}

fileManager::~fileManager()
{
	delete m_ui ;
}

void fileManager::set()
{
	settings& s = settings::instance() ;

	if( m_setFileManager ){

		s.setFileManager( m_ui->lineEdit->text() ) ;

		m_fileManager = s.fileManager() ;
	}else{
		s.setExternalPluginExecutable( m_ui->lineEdit_2->text() ) ;
	}

	this->hide() ;
	this->deleteLater() ;
}

void fileManager::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;
	this->deleteLater() ;
}
