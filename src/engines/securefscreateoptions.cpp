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

#include "../utility.h"

#include <QFileDialog>

securefscreateoptions::securefscreateoptions( const engines::engine& engine,
					      const engines::engine::createGUIOptions& s,
					      bool e ) :
	QDialog( s.parent ),
	m_ui( new Ui::securefscreateoptions ),
	m_configFileName( engine.configFileName() ),
	m_function( s.fCreateOptions )
{
	m_ui->setupUi( this ) ;

	m_ui->pbConfigFile_2->setEnabled( e ) ;
	m_ui->lineEdit_2->setEnabled( e ) ;
	m_ui->label->setEnabled( e ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbConfigFile,SIGNAL( clicked() ),this,SLOT( pbConfigFilePath() ) ) ;
	connect( m_ui->pbConfigFile_2,SIGNAL( clicked() ),this,SLOT( pbKeyFilePath() ) ) ;

	m_ui->pbConfigFile->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->pbConfigFile_2->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->comboBox->setFocus() ;

	auto ss = tr( "The \"lite format\" simply encrypts filenames and file contents separately, similar to how encfs operates, although with more security.\n\nThe \"full format\" maps files, directories and symlinks in the virtual filesystem all to regular files in the underlying filesystem. The directory structure is flattened and recorded as B-trees in files.\n\nThe lite format has become the default on Unix-like operating systems as it is much faster and features easier conflict resolution, especially when used with DropBox, Google Drive, etc. The full format, however, leaks fewer information about the filesystem hierarchy, runs relatively independent of the features of the underlying filesystem, and is in general more secure." ) ;

	m_ui->plainTextEdit->appendPlainText( ss ) ;

	this->show() ;
}

securefscreateoptions::~securefscreateoptions()
{
	delete m_ui ;
}

void securefscreateoptions::pbOK()
{
	this->hide() ;

	if( m_ui->comboBox->currentIndex() == 1 ){

		this->HideUI( { "--format,2",m_ui->lineEdit->text(),m_ui->lineEdit_2->text() } ) ;
	}else{
		this->HideUI( { "--format,4",m_ui->lineEdit->text(),m_ui->lineEdit_2->text() } ) ;
	}
}

void securefscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void securefscreateoptions::pbConfigFilePath()
{
	m_ui->lineEdit->setText( utility::configFilePath( this,m_configFileName ) ) ;
}

void securefscreateoptions::pbKeyFilePath()
{
	auto e = QFileDialog::getOpenFileName( this,tr( "Select A KeyFile" ),QDir::homePath() ) ;

	if( !e.isEmpty() ){

		m_ui->lineEdit_2->setText( e ) ;
	}
}

void securefscreateoptions::HideUI( const engines::engine::cOpts& opts )
{
	this->hide() ;
	m_function( opts ) ;
	this->deleteLater() ;
}

void securefscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
