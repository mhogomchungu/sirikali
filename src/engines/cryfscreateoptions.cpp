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

#include "cryfscreateoptions.h"
#include "ui_cryfscreateoptions.h"

#include "../utility.h"
#include "task.hpp"

#include "../engines.h"

cryfscreateoptions::cryfscreateoptions( const engines::engine& engine,
					const engines::engine::createGUIOptions& s,
					bool allowReplacedFileSystem ) :
	QDialog( s.parent ),
	m_ui( new Ui::cryfscreateoptions ),
	m_configFileName( engine.configFileName() ),
	m_function( s.fCreateOptions )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;
	connect( m_ui->pbConfigPath,SIGNAL( clicked() ),this,SLOT( pbSelectConfigPath() ) ) ;

	if( allowReplacedFileSystem ){

		m_ui->cbAllowReplacedFileSystem->setChecked( s.cOpts.opts.allowReplacedFileSystem ) ;
	}else{
		m_ui->cbAllowReplacedFileSystem->setEnabled( false ) ;
	}

	m_ui->pbConfigPath->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->lineEdit->setText( "32768" ) ;

	m_ui->pbOptions->setMenu( [ w = this ](){

		auto m = new QMenu( w ) ;

		auto s = { "4KB","16KB","32KB","64KB","128KB","512KB","1024KB","2048KB","4096KB" } ;

		for( const auto& it : s ){

			m->addAction( it )->setObjectName( it ) ;
		}

		QObject::connect( m,&QMenu::triggered,[ w ]( QAction * ac ){

			int e = ac->objectName().replace( "KB","" ).toInt() * 1024 ;

			w->m_ui->lineEdit->setText( QString::number( e ) ) ;
		} ) ;

		return m ;
	}() ) ;

	auto& exe = engine.executableFullPath() ;

	utility::Task::run( exe,{ "--show-ciphers" } ).then( [ this ]( const utility::Task& e ){

		if( e.success() ){

			auto s = e.splitOutput( '\n',utility::Task::channel::stdError ) ;

			if( !s.isEmpty() ){

				m_ui->comboBox->addItems( s ) ;
			}else{
				m_ui->comboBox->setEnabled( false ) ;
			}
		}else{
			m_ui->comboBox->setEnabled( false ) ;
		}

		m_ui->pbOK->setFocus() ;

		this->show() ;
	} ) ;
}

cryfscreateoptions::~cryfscreateoptions()
{
	delete m_ui ;
}

void cryfscreateoptions::pbSelectConfigPath()
{
	m_ui->lineEdit_2->setText( utility::configFilePath( this,m_configFileName ) ) ;
}

void cryfscreateoptions::pbOK()
{
	auto e = [ & ](){

		auto s = m_ui->lineEdit->text() ;

		if( s.isEmpty() ){

			s = "32768" ;
		}

		auto m = m_ui->comboBox->currentText() ;

		if( m.isEmpty() ){

			return QString( "--blocksize,%2" ).arg( s ) ;
		}else{
			return QString( "--cipher,%1,--blocksize,%2" ).arg( m,s ) ;
		}
	}() ;

	engines::engine::booleanOptions opts ;

	opts.allowReplacedFileSystem = m_ui->cbAllowReplacedFileSystem->isChecked() ;

	this->HideUI( { e,m_ui->lineEdit_2->text(),QString(),opts } ) ;
}

void cryfscreateoptions::pbCancel()
{
	this->HideUI() ;
}

void cryfscreateoptions::HideUI( const engines::engine::cOpts& opts )
{
	this->hide() ;
	m_function( opts ) ;
	this->deleteLater() ;
}

void cryfscreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}
