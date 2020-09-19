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
#include "../dialogmsg.h"

#include "../utility.h"
#include "../utility2.h"
#include "../settings.h"

#include <QFileDialog>

options::options( const engines::engine& engine,const engines::engine::mountGUIOptions& s ) :
	QDialog( s.parent ),
	m_ui( new Ui::options ),
	m_engine( engine ),
	m_create( s.create ),
	m_setOptions( s.fMountOptions ),
	m_parentWidget( s.parent )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->pbConfigFile->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->pbKeyFile->setIcon( QIcon( ":/file.png" ) ) ;

	this->setWindowIcon( QIcon( ":/sirikali.png" ) ) ;

	connect( m_ui->pbConfigFile,SIGNAL( clicked() ),this,SLOT( pbConfigFile() ) ) ;

	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbKeyFile() ) ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbSet() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	connect( m_ui->checkBox,&QCheckBox::stateChanged,[ this ]( bool e ){

		m_setGUIOptions.checkBoxChecked = e ;
	} ) ;

	m_setGUIOptions.mOpts = s.mOpts ;

	m_ui->lineEditIdleTime->setText( m_setGUIOptions.mOpts.idleTimeOut ) ;
	m_ui->lineEditConfigFilePath->setText( m_setGUIOptions.mOpts.configFile ) ;
	m_ui->lineEditMountOptions->setText( m_setGUIOptions.mOpts.mountOpts.join( ',' ) ) ;
	m_ui->lineEditKeyFile->setText( m_setGUIOptions.mOpts.keyFile ) ;

	const auto& name = m_engine.name() ;

	m_setGUIOptions.checkBoxText = tr( "Reverse Mode." ) ;

	m_setGUIOptions.keyFileTitle = tr( "Unlock %1 Volume With A KeyFile." ).arg( name ) ;

	m_ui->labelConfigFile->setText( tr( "Unlock %1 Volume With A Configuration File." ).arg( name ) ) ;

	utility::setWindowOptions( this ) ;
	settings::instance().setParent( s.parent,&m_parentWidget,this ) ;
}

void options::pbConfigFile()
{
	auto e = [ this ](){

		return QFileDialog::getOpenFileName( this,
						     QObject::tr( "Select %1 Configuration File." ).arg( m_engine.name() ),
						     settings::instance().homePath() ) ;
	}() ;

        if( !e.isEmpty() ){

		m_ui->lineEditConfigFilePath->setText( e ) ;
	}
}

void options::pbKeyFile()
{
	auto e = [ this ](){

		return QFileDialog::getOpenFileName( this,
						     QObject::tr( "Select %1 KeyFile." ).arg( m_engine.name() ),
						     settings::instance().homePath() ) ;
	}() ;

	if( !e.isEmpty() ){

		m_ui->lineEditKeyFile->setText( e ) ;
	}
}

void options::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->pbCancel() ;
}

void options::pbSet()
{
	auto idle = m_ui->lineEditIdleTime->text() ;

	auto mountOpts = m_ui->lineEditMountOptions->text() ;

	auto opts = m_setGUIOptions.updateOptions( m_setGUIOptions ) ;

	auto configFile = m_ui->lineEditConfigFilePath->text() ;

	auto keyFile = m_ui->lineEditKeyFile->text() ;

	QString empty ;

	this->Hide( { idle,configFile,mountOpts,keyFile,empty,empty,opts } ) ;
}

void options::pbCancel()
{
	this->Hide() ;
}

void options::Hide( const engines::engine::mOpts& e )
{
	this->hide() ;
	m_setOptions( e ) ;
	this->deleteLater() ;
}

options::~options()
{
	delete m_ui ;
}

void options::ShowUI()
{
	const auto& e = m_setGUIOptions ;

	m_ui->labelConfigFile->setEnabled( e.enableConfigFile ) ;
	m_ui->labelIdleTimeout->setEnabled( e.enableIdleTime ) ;
	m_ui->labelMountOptions->setEnabled( e.enableMountOptions ) ;
	m_ui->labelKeyFile->setEnabled( e.enableKeyFile ) ;

	m_ui->labelKeyFile->setText( e.keyFileTitle ) ;

	m_ui->pbKeyFile->setEnabled( e.enableKeyFile ) ;
	m_ui->pbConfigFile->setEnabled( e.enableConfigFile ) ;

	m_ui->lineEditConfigFilePath->setEnabled( e.enableConfigFile ) ;
	m_ui->lineEditKeyFile->setEnabled( e.enableKeyFile ) ;
	m_ui->lineEditMountOptions->setEnabled( e.enableMountOptions ) ;
	m_ui->lineEditIdleTime->setEnabled( e.enableIdleTime ) ;

	m_ui->checkBox->setEnabled( e.enableCheckBox ) ;
	m_ui->checkBox->setChecked( e.checkBoxChecked ) ;

	m_ui->checkBox->setText( e.checkBoxText ) ;

	[ this ](){

		std::array< QLineEdit *,4 > lineEdits = { m_ui->lineEditKeyFile,
							  m_ui->lineEditConfigFilePath,
							  m_ui->lineEditMountOptions,
							  m_ui->lineEditIdleTime } ;

		for( const auto it : lineEdits ){

			if( it->isEnabled() && it->text().isEmpty() ){

				return it->setFocus() ;
			}
		}

		m_ui->pbCancel->setFocus() ;
	}() ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

options::Options& options::GUIOptions()
{
	return m_setGUIOptions ;
}
