/*
 *
 *  Copyright (c) 2012-2015
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

#include "options.h"
#include "ui_options.h"
#include "dialogmsg.h"

#include "utility.h"
#include "utility2.h"
#include "settings.h"

#include <QFileDialog>

options::options( QWidget * parent,bool r,const Options& l,
		  std::function< void( const Options& ) >&& e ) :
	QDialog( parent ),
	m_ui( new Ui::options ),
	m_create( r ),
	m_setOptions( std::move( e ) ),
	m_parentWidget( parent )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

        m_ui->pushButton->setIcon( QIcon( ":/file.png" ) ) ;

	this->setWindowIcon( QIcon( ":/sirikali.png" ) ) ;

        connect( m_ui->pushButton,SIGNAL( clicked() ),this,SLOT( pushButton() ) ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbSet() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	QString idleTimeOut ;
	QString configFilePath ;
	QString mountOptions ;
	QString type ;

	utility2::stringListToStrings( l.options,idleTimeOut,configFilePath,mountOptions,type ) ;

	m_ui->checkBox->setChecked( l.reverseMode ) ;

	m_type = type.toLower() ;

	m_ui->lineEditIdleTime->setText( idleTimeOut ) ;
	m_ui->lineConfigFilePath->setText( configFilePath ) ;
	m_ui->lineEditMountOptions->setText( mountOptions ) ;

	utility::setWindowOptions( this ) ;
	settings::instance().setParent( parent,&m_parentWidget,this ) ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;

	if( m_ui->lineEditIdleTime->text().isEmpty() ){

		m_ui->lineEditIdleTime->setFocus() ;

	}else if( m_ui->lineConfigFilePath->text().isEmpty() ){

		m_ui->lineConfigFilePath->setFocus() ;

	}else if( m_ui->lineEditMountOptions->text().isEmpty() ){

		m_ui->lineEditMountOptions->setFocus() ;
	}else{
		m_ui->pbOK->setFocus() ;
	}
}

void options::pushButton()
{
	auto e = [ this ](){

		if( m_create ){

			QFileDialog dialog( this ) ;

			dialog.setFileMode( QFileDialog::AnyFile ) ;

			dialog.setDirectory( settings::instance().homePath() ) ;

			dialog.setAcceptMode( QFileDialog::AcceptSave ) ;

			dialog.selectFile( [ this ](){

				if( m_type == "cryfs" ){

					return "cryfs.config" ;

				}else if( m_type == "encfs" ){

					return "encfs6.xml" ;

				}else if( m_type == "gocryptfs" ){

					return "gocryptfs.conf" ;

				}else if( m_type == "securefs" ){

					return "securefs.json" ;

				}else if( m_type == "ecryptfs" ){

					return "ecryptfs.config" ;
				}else{
					return "" ;
				}
			}() ) ;

			if( dialog.exec() ){

				auto q = dialog.selectedFiles() ;

				if( !q.isEmpty() ){

					return q.first() ;
				}
			}

			return QString() ;
		}else{
			return QFileDialog::getOpenFileName( this,
							     tr( "Select Cryfs/Gocryptfs Configuration File" ),
							     settings::instance().homePath() ) ;
		}
	}() ;

        if( !e.isEmpty() ){

		m_ui->lineConfigFilePath->setText( e ) ;
        }
}

void options::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void options::pbSet()
{
	auto e = m_ui->lineEditIdleTime->text() ;

	auto m = m_ui->lineEditMountOptions->text() ;

	this->Hide( { { e,m_ui->lineConfigFilePath->text(),m },m_ui->checkBox->isChecked() } ) ;
}

void options::pbCancel()
{
	this->Hide() ;
}

void options::Hide( const Options& e )
{
	this->hide() ;
	m_setOptions( e ) ;
	this->deleteLater() ;
}

options::~options()
{
	delete m_ui ;
}
