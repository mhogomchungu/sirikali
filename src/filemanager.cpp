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

fileManager::fileManager( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::fileManager )
{
	m_ui->setupUi( this ) ;

	connect( m_ui->pushButton,SIGNAL( clicked( bool ) ),this,SLOT( set() ) ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	m_ui->label->setText( tr( "Enter Below The Name Of The Application You Want To Be Used To Open Mount Points." ) ) ;

	m_ui->lineEdit->setText( utility::fileManager() ) ;

	this->show() ;
}

fileManager::~fileManager()
{
	delete m_ui ;
}

void fileManager::set()
{
	utility::setFileManager( m_ui->lineEdit->text() ) ;

	this->hide() ;
	this->deleteLater() ;
}

void fileManager::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;
	this->deleteLater() ;
}
