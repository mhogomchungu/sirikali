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

#include "checkforupdateswindow.h"
#include "ui_checkforupdateswindow.h"

checkforupdateswindow::checkforupdateswindow( QWidget * parent,functions& ff ) :
	QDialog( parent ),
	m_ui( new Ui::checkforupdateswindow ),
	m_functions( ff )
{
	m_ui->setupUi( this ) ;

	this->window()->setWindowTitle( tr( "Version Info" ) ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,&QPushButton::clicked,[ this ](){

		this->closeUI() ;
	} ) ;
}

void checkforupdateswindow::ShowUI( const QString& e )
{
	m_ui->textEdit->setHtml( e ) ;

	this->show() ;
}

checkforupdateswindow::~checkforupdateswindow()
{
	delete m_ui ;
}

void checkforupdateswindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->closeUI() ;
}

void checkforupdateswindow::closeUI()
{
	m_functions.second() ;
	this->hide() ;
	this->deleteLater() ;
}
