/*
 *
 *  Copyright (c) 2018
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

#include "debugwindow.h"
#include "ui_debugwindow.h"
#include "utility.h"

debugWindow::debugWindow( QWidget * parent ) :
	QWidget( parent ),
	m_ui( new Ui::debugWindow )
{
	m_ui->setupUi( this ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	m_ui->pbClose->setFocus() ;

	connect( m_ui->pbClear,&QPushButton::clicked,[ this ](){

		m_ui->textEdit->setText( QString() ) ;
	} ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->Hide() ;
	} ) ;

	utility::setDebugWindow( this ) ;
}

debugWindow::~debugWindow()
{
	delete  m_ui ;
}

void debugWindow::Show()
{
	this->show() ;
}

void debugWindow::Hide()
{
	this->hide() ;
}

void debugWindow::UpdateOutPutSlot( const QString& e,bool s )
{
	if( this->isVisible() || s ){

		auto s = m_ui->textEdit->toPlainText() ;
		m_ui->textEdit->setText( s + "\n" + e ) ;
	}
}

void debugWindow::UpdateOutPut( const QString& e,bool m )
{
	QMetaObject::invokeMethod( this,
				   "UpdateOutPutSlot",
				   Qt::QueuedConnection,
				   Q_ARG( QString,e ),
				   Q_ARG( bool,m ) ) ;
}

void debugWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}
