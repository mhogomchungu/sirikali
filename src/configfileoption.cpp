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

#include "configfileoption.h"
#include "ui_configfileoption.h"

#include "utility.h"

configFileOption::configFileOption( QWidget * parent,
				    const engines::engine& engine,
				    std::function< void( const QStringList& ) > s ) :
	QDialog( parent ),
	m_ui( new Ui::configFileOption ),
	m_configFileName( engine.configFileName() ),
	m_function( std::move( s ) )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbSet,SIGNAL( clicked() ),this,SLOT( pbSet() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbOpenPath,SIGNAL( clicked() ),this,SLOT( pbSetConfigFile() ) ) ;

	m_ui->pbOpenPath->setIcon( QIcon( ":/folder.png" ) ) ;

	this->show() ;
}

configFileOption::~configFileOption()
{
	delete m_ui ;
}

void configFileOption::pbCancel()
{
	this->hide() ;

	m_function( { m_ui->lineEdit->text() } ) ;

	this->deleteLater() ;
}

void configFileOption::pbSet()
{
	this->pbCancel() ;
}

void configFileOption::pbSetConfigFile()
{
	m_ui->lineEdit->setText( utility::configFilePath( this,m_configFileName ) ) ;
}

void configFileOption::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
