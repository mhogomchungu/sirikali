/*
 *
 *  Copyright ( c ) 2015
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

#include "help.h"
#include "ui_help.h"

#include <QObject>
#include <QCloseEvent>
#include <QEvent>

#include "utility.h"

help::help( QWidget * parent,const QString& path,std::function< void() > function ) :
	QDialog( parent ),
	m_ui( new Ui::help ),
	m_openPath( path ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	m_ui->pushButton_2->setVisible( false ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	this->setFont( parent->font() ) ;

	connect( m_ui->pushButton,SIGNAL( clicked( bool ) ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pushButton_2,SIGNAL( clicked( bool ) ),this,SLOT( pbOpenPDF() ) ) ;

	this->show() ;
}

help::~help()
{
	delete m_ui ;
}

void help::pbClose()
{
	m_function() ;
	this->hide() ;
	this->deleteLater() ;
}

void help::pbOpenPDF()
{
}

void help::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbClose() ;
}

bool help::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->pbClose() ; } ) ;
}
