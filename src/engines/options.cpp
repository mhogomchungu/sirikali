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

options::options( QWidget * parent,
		  const engines::engine& engine,
		  bool r,
		  const engines::engine::mountOptions& l,
		  engines::engine::fMountOptions e ) :
	QDialog( parent ),
	m_ui( new Ui::options ),
	m_engine( engine ),
	m_create( r ),
	m_setOptions( std::move( e ) ),
	m_parentWidget( parent )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;

	m_ui->pushButton->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->pushButton_2->setIcon( QIcon( ":/file.png" ) ) ;

	this->setWindowIcon( QIcon( ":/sirikali.png" ) ) ;

	connect( m_ui->pushButton,SIGNAL( clicked() ),this,SLOT( pushButton() ) ) ;

	connect( m_ui->pushButton_2,SIGNAL( clicked() ),this,SLOT( pushButton_2() ) ) ;

	connect( m_ui->pbOK,SIGNAL( clicked() ),this,SLOT( pbSet() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbCancel() ) ) ;

	connect( m_ui->checkBox,&QCheckBox::stateChanged,[ this ]( bool e ){

		m_setGUIOptions.checkBoxChecked = e ;
	} ) ;

	m_setGUIOptions.configFileTitle = QObject::tr( "Unlock A %1 Volume With Specified Configuration File." ).arg( engine.name() ) ;
	m_setGUIOptions.fileDialogText  = QObject::tr( "Select %1 Configuration File." ).arg( engine.name() ) ;

	QString idleTimeOut ;
	QString configFilePath ;
	QString mountOptions ;
	QString keyFile ;

	utility2::stringListToStrings( l.options,idleTimeOut,configFilePath,mountOptions,keyFile ) ;

	m_ui->lineEditIdleTime->setText( idleTimeOut ) ;
	m_ui->lineConfigFilePath->setText( configFilePath ) ;
	m_ui->lineEditMountOptions->setText( mountOptions ) ;

	if( !keyFile.isEmpty() ){

		m_ui->lineEditIdleTime->setText( keyFile ) ;
	}

	utility::setWindowOptions( this ) ;
	settings::instance().setParent( parent,&m_parentWidget,this ) ;

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

		return QFileDialog::getOpenFileName( this,
						     m_setGUIOptions.fileDialogText,
						     settings::instance().homePath() ) ;
	}() ;

        if( !e.isEmpty() ){

		m_ui->lineConfigFilePath->setText( e ) ;
	}
}

void options::pushButton_2()
{
	auto e = [ this ](){

		return QFileDialog::getOpenFileName( this,
						     m_setGUIOptions.fileDialogText,
						     settings::instance().homePath() ) ;
	}() ;

	if( !e.isEmpty() ){

		m_ui->lineEditIdleTime->setText( e ) ;
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

	engines::engine::options::booleanOptions opts ;

	m_setGUIOptions.updateOptions( opts,m_setGUIOptions ) ;

	auto filePath = m_ui->lineConfigFilePath->text() ;

	/*
	 * idleTimeOut,configFile,mountOptions and keyFile
	 */
	if( m_setGUIOptions.fileType == options::Options::KEY::USES_KEYFILE_ONLY ){

		this->Hide( { { idle,QString(),mountOpts,filePath },opts } ) ;

	}else if( m_setGUIOptions.fileType == options::Options::KEY::USES_CONFIG_FILE_ONLY ){

		this->Hide( { { idle,filePath,mountOpts,QString() },opts } ) ;

	}else if( m_setGUIOptions.fileType == options::Options::KEY::USES_KEYFILE_AND_CONFIG_FILE ){

		this->Hide( { { QString(),filePath,mountOpts,idle },opts } ) ;
	}else{
		this->Hide( { { idle,filePath,mountOpts,QString() },opts } ) ;
	}
}

void options::pbCancel()
{
	this->Hide() ;
}

void options::Hide( const engines::engine::mountOptions& e )
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
	m_ui->label->setText( m_setGUIOptions.configFileTitle ) ;

	m_ui->label->setEnabled( m_setGUIOptions.enableConfigFile ) ;
	m_ui->label_3->setEnabled( m_setGUIOptions.enableIdleTime ) ;
	m_ui->label_4->setEnabled( m_setGUIOptions.enableMountOptions ) ;

	m_ui->lineEditIdleTime->setEnabled( m_setGUIOptions.enableIdleTime ) ;
	m_ui->lineEditMountOptions->setEnabled( m_setGUIOptions.enableMountOptions ) ;

	m_ui->pushButton->setEnabled( m_setGUIOptions.enableConfigFile ) ;
	m_ui->lineConfigFilePath->setEnabled( m_setGUIOptions.enableConfigFile ) ;

	m_ui->checkBox->setEnabled( m_setGUIOptions.enableCheckBox ) ;
	m_ui->checkBox->setText( m_setGUIOptions.checkBoxText ) ;
	m_ui->checkBox->setChecked( m_setGUIOptions.checkBoxChecked ) ;

	m_ui->pushButton_2->setVisible( m_setGUIOptions.setVisiblePushButton_2 ) ;

	m_ui->label_3->setText( m_setGUIOptions.idleTitle ) ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

options::Options& options::GUIOptions()
{
	return m_setGUIOptions ;
}
