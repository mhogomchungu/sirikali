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

#include "configoptions.h"
#include "ui_configoptions.h"

#include "utility.h"

#include <QFileDialog>

configOptions::configOptions( QWidget * parent,
			      secrets& a,
			      QMenu * m,
			      configOptions::functions f ) :
	QDialog( parent ),
	m_ui( new Ui::configOptions ),
	m_functions( std::move( f ) ),
	m_secrets( a ),
	m_settings( settings::instance() )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbMountPointPrefix->setIcon( QIcon( ":/folder.png" ) ) ;

	m_ui->pbPostMountCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbPreUnMountCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbSetExternalCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbSetFileManager->setIcon( QIcon( ":/executable.png" ) ) ;

	m_ui->pbRunPeriodicallyCommand->setIcon( QIcon( ":/executable.png" ) ) ;

	connect( m_ui->pushButton,&QPushButton::clicked,[ this ](){ this->HideUI() ; } ) ;

	connect( m_ui->cbAutoOpenMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoOpenFolderOnMount( e ) ;
	} ) ;

	connect( m_ui->cbReUseMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.reUseMountPoint( e ) ;
	} ) ;

	connect( m_ui->cbAutoCheckForUpdates,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoCheck( e ) ;
	} ) ;

	connect( m_ui->cbStartMinimized,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.setStartMinimized( e ) ;
	} ) ;

	if( utility::platformIsWindows() ){

		m_ui->label->setText( tr( "Set Executables Search Path" ) ) ;
		m_ui->lineEditMountPointPrefix->clear() ;
		m_ui->lineEditMountPointPrefix->setEnabled( true ) ;
		m_ui->pbMountPointPrefix->setEnabled( true ) ;
		m_ui->lineEditMountPointPrefix->setText( m_settings.windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	}

	connect( m_ui->pbMountPointPrefix,&QPushButton::clicked,[ this ](){

		auto e = utility::getExistingDirectory( this,QString(),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditMountPointPrefix->setText( e ) ;

			if( utility::platformIsWindows() ){

				m_settings.setWindowsExecutableSearchPath( e ) ;
			}else{
				m_settings.setDefaultMountPointPrefix( e ) ;
			}
		}
	} ) ;

	m_ui->pbSelectLanguage->setMenu( m ) ;

	connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

		m_functions.function_2( ac ) ;

		this->translateUI() ;
	} ) ;

	connect( m_ui->pbPostMountCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Post Mount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditAfterMountCommand->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbPreUnMountCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Pre UnMount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditBeforesUnMount->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbRunPeriodicallyCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Pre UnMount Command" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditRunPeriodically->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetFileManager,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set Command To Open Mount Points" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditFileManager->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetExternalCommand,&QPushButton::pressed,[ this ](){

		auto e = QFileDialog::getOpenFileName( this,tr( "Set External Plugin Executable" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditExecutableKeySource->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSetFileManagerToDefault,&QPushButton::pressed,[ this ](){

		m_settings.setFileManager( QString() ) ;
		m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;
	} ) ;

	connect( m_ui->pbExternalExecutableDefault,&QPushButton::pressed,[ this ](){

		m_settings.setExternalPluginExecutable( QString() ) ;

		m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;
	} ) ;

	connect( m_ui->pbMountPointPrefixDefault,&QPushButton::pressed,[ this ](){

		m_settings.setDefaultMountPointPrefix( QString() ) ;
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	} ) ;

	connect( m_ui->pbRunPostMountCommandDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditAfterMountCommand->clear() ;
	} ) ;

	connect( m_ui->pbRunPreUnMountCommandDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditBeforesUnMount->clear() ;
	} ) ;

	connect( m_ui->pbRunCommandPeriodicallyDefault,&QPushButton::pressed,[ this ](){

		m_ui->lineEditRunPeriodically->clear() ;
	} ) ;

	connect( m_ui->cbHiDPI,&QCheckBox::toggled,[ this ]( bool e ){

		m_ui->lineEditHiDPI->setEnabled( e ) ;
	} ) ;

	connect( m_ui->chShowDebugWindowOnStartup,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.showDebugWindowOnStartup( e ) ;
	} ) ;
}

configOptions::~configOptions()
{
	delete m_ui ;
}

void configOptions::translateUI()
{
	m_ui->retranslateUi( this ) ;

	for( auto& it : m_actionPair ){

		it.first->setText( tr( it.second ) ) ;
	}

	if( utility::platformIsWindows() ){

		m_ui->label->setText( tr( "Set Executables Search Path" ) ) ;
	}
}

void configOptions::ShowUI()
{
	m_ui->cbAutoOpenMountPoint->setChecked( m_settings.autoOpenFolderOnMount() ) ;

	m_ui->cbReUseMountPoint->setChecked( m_settings.reUseMountPoint() ) ;

	m_ui->cbAutoCheckForUpdates->setChecked( m_settings.autoCheck() ) ;

	m_ui->cbStartMinimized->setChecked( m_settings.startMinimized() ) ;

	m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;

	m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;

	m_ui->lineEditAfterMountCommand->setText( m_settings.runCommandOnMount() ) ;

	m_ui->lineEditBeforesUnMount->setText( m_settings.preUnMountCommand() ) ;

	m_ui->lineEditRunPeriodically->setText( m_settings.runCommandOnInterval() ) ;

	m_ui->lineEditRunPeriodicallyInterval->setText( QString::number( m_settings.runCommandOnIntervalTime() ) ) ;

	m_ui->cbHiDPI->setChecked( m_settings.enableHighDpiScaling() ) ;

	m_ui->lineEditHiDPI->setText( m_settings.enabledHighDpiScalingFactor() ) ;

	m_ui->lineEditHiDPI->setEnabled( m_ui->cbHiDPI->isChecked() ) ;

	m_ui->chShowDebugWindowOnStartup->setChecked( m_settings.showDebugWindowOnStartup() ) ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditMountPointPrefix->setText( m_settings.windowsExecutableSearchPath() ) ;
	}else{
		m_ui->lineEditMountPointPrefix->setText( m_settings.mountPath() ) ;
	}

	this->show() ;
}

void configOptions::HideUI()
{
	m_functions.function_1() ;

	m_settings.setFileManager( m_ui->lineEditFileManager->text() ) ;
	m_settings.setExternalPluginExecutable( m_ui->lineEditExecutableKeySource->text() ) ;
	m_settings.preUnMountCommand( m_ui->lineEditBeforesUnMount->text() ) ;
	m_settings.runCommandOnMount( m_ui->lineEditAfterMountCommand->text() ) ;
	m_settings.runCommandOnInterval( m_ui->lineEditRunPeriodically->text() ) ;
	m_settings.enableHighDpiScaling( m_ui->cbHiDPI->isChecked() ) ;

	m_settings.enabledHighDpiScalingFactor( [ this ]()->QString{

		auto s = m_ui->lineEditHiDPI->text() ;
		bool ok ;

		s.toDouble( &ok ) ;

		if( ok ){

			return s ;
		}else{
			return "1.0" ;
		}
	}() ) ;

	bool ok ;

	auto a = m_ui->lineEditRunPeriodicallyInterval->text() ;

	auto b = a.toInt( &ok ) ;

	if( ok ){

		m_settings.runCommandOnIntervalTime( b ) ;
	}else{
		m_settings.runCommandOnIntervalTime( 10 ) ;
	}

	if( utility::platformIsWindows() ){

		m_settings.setWindowsExecutableSearchPath( m_ui->lineEditMountPointPrefix->text() ) ;
	}else{
		m_settings.setDefaultMountPointPrefix( m_ui->lineEditMountPointPrefix->text() ) ;
	}

	this->hide() ;
}

void configOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
