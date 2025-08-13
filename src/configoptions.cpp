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

#include "sirikali.h"

configOptions::configOptions( sirikali * s,secrets& a,QMenu * m ) :
	QDialog( s ),
	m_ui( new Ui::configOptions ),
	m_secrets( a ),
	m_settings( settings::instance() ),
	m_sirikali( *s )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->window()->size() ) ;

	this->installEventFilter( this ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbMountPointPrefix->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbMountPointPrefixDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	m_ui->pbPostMountCommand->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbRunPostMountCommandDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	m_ui->pbPreUnMountCommand->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbRunPreUnMountCommandDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	m_ui->pbSetExternalCommand->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbExternalExecutableDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	m_ui->pbSetFileManager->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbSetFileManagerToDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	m_ui->pbRunPeriodicallyCommand->setIcon( QIcon( ":/icons/folder.png" ) ) ;
	m_ui->pbRunCommandPeriodicallyDefault->setIcon( QIcon( ":/icons/setting_reset.png" ) );

	if( utility::platformIsFlatPak() ){

		m_ui->cbInternallyManageBackEnds->setChecked( true ) ;
		m_ui->cbInternallyManageBackEnds->setEnabled( false ) ;

	}else if( utility::platformIsWindows() ){

		auto m = m_settings.internallyManageBackEnds() ;

		m_ui->cbInternallyManageBackEnds->setChecked( m ) ;
		m_ui->cbInternallyManageBackEnds->setEnabled( true ) ;

	}else if( utility::platformIsLinux() ){

		auto m = m_settings.internallyManageBackEnds() ;

		m_ui->cbInternallyManageBackEnds->setChecked( m ) ;
		m_ui->cbInternallyManageBackEnds->setEnabled( true ) ;
	}else{
		m_ui->cbInternallyManageBackEnds->setChecked( false ) ;
		m_ui->cbInternallyManageBackEnds->setEnabled( false ) ;
	}

	auto theme = m_settings.getThemeType() ;

	if( theme == settings::themeType::dark ){

		m_ui->cbThemes->setCurrentIndex( 1 ) ;

	}else if( theme == settings::themeType::light ){

		m_ui->cbThemes->setCurrentIndex( 2 ) ;
	}else{
		m_ui->cbThemes->setCurrentIndex( 0 ) ;
	}

	auto sig = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui->cbThemes,sig,[ this ]( int m ){

		if( m == 0 ){

			m_settings.setThemeType( settings::themeType::platformDefault ) ;

		}else if( m == 1 ){

			m_settings.setThemeType( settings::themeType::dark ) ;
		}else{
			m_settings.setThemeType( settings::themeType::light ) ;
		}
	} ) ;

	connect( m_ui->pushButton,&QPushButton::clicked,[ this ](){ this->HideUI() ; } ) ;

	connect( m_ui->cbAutoOpenMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.autoOpenFolderOnMount( e ) ;
	} ) ;

	connect( m_ui->cbReUseMountPoint,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.reUseMountPoint( e ) ;
	} ) ;

	connect( m_ui->cbInternallyManageBackEnds,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.setInternallyManageBackEnds( e ) ;

		if( !e ){

			auto mm = engines::instance().supportedEngines() ;

			auto m = engines::defaultBinPath() ;

			for( const auto& it : mm ){

				if( it->updatable( true ) ){

					it->deleteBinPath( m ) ;
				}
			}
		}
	} ) ;

	connect( m_ui->cbStartMinimized,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.setStartMinimized( e ) ;
	} ) ;

	m_ui->cbShowTrayIcon->setChecked( m_settings.showSystemTray() ) ;

	connect( m_ui->cbShowTrayIcon,&QCheckBox::toggled,[ this ]( bool e ){

		m_settings.setShowSystemTray( e ) ;

		m_sirikali.showTrayIcon( e ) ;
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

			if( utility::platformIsWindows() ){

				m_settings.setWindowsExecutableSearchPath( e ) ;
				auto m = m_settings.windowsExecutableSearchPath() ;

				m_ui->lineEditMountPointPrefix->setText( m ) ;
			}else{
				m_ui->lineEditMountPointPrefix->setText( e ) ;

				m_settings.setDefaultMountPointPrefix( e ) ;
			}
		}
	} ) ;

	m_ui->pbSelectLanguage->setMenu( m ) ;

	connect( m,&QMenu::triggered,[ this ]( QAction * ac ){

		m_sirikali.updateLanguage( ac ) ;

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

		if( utility::platformIsWindows() ){

			auto e = QFileDialog::getOpenFileName( this,tr( "Set External Plugin Executable" ),QDir::homePath() ) ;

			if( !e.isEmpty() ){

				m_ui->lineEditExecutableKeySource->setText( e ) ;
			}
		}else{
			auto e = QFileDialog::getOpenFileName( this,tr( "Set Executables Search Path" ),QDir::homePath() ) ;

			if( !e.isEmpty() ){

				m_ui->lineEditExecutableKeySource->setText( e ) ;
			}
		}
	} ) ;

	connect( m_ui->pbSetFileManagerToDefault,&QPushButton::pressed,[ this ](){

		m_settings.setFileManager( QString() ) ;
		m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;
	} ) ;

	connect( m_ui->pbExternalExecutableDefault,&QPushButton::pressed,[ this ](){

		if( utility::platformIsWindows() ){

			m_settings.setExternalPluginExecutable( QString() ) ;

			m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;
		}else{
			auto m = settings::instance().defaultExecutableSearchPath() ;

			m_ui->lineEditExecutableKeySource->setText( m ) ;
		}
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
	}else{
		m_ui->label_3->setText( tr( "Set Executables Search Path" ) ) ;
	}

	if( utility::platformIsFlatPak() ){

		m_ui->cbInternallyManageBackEnds->setText( tr( "Manage Securefs, Gocryptfs, Cryfs And Cryptomator" ) ) ;

	}else if( utility::platformIsWindows() ){

		m_ui->cbInternallyManageBackEnds->setText( tr( "Manage Securefs, Cppcryptfs And Cryptomator" ) ) ;

	}else if( utility::platformIsLinux() ){

		m_ui->cbInternallyManageBackEnds->setText( tr( "Manage Securefs And Gocryptfs" ) ) ;
	}else{
		m_ui->cbInternallyManageBackEnds->setText( tr( "Manage Securefs" ) ) ;
	}
}

void configOptions::ShowUI()
{
	m_ui->cbAutoOpenMountPoint->setChecked( m_settings.autoOpenFolderOnMount() ) ;

	m_ui->cbReUseMountPoint->setChecked( m_settings.reUseMountPoint() ) ;

	m_ui->cbStartMinimized->setChecked( m_settings.startMinimized() ) ;

	m_ui->lineEditFileManager->setText( m_settings.fileManager() ) ;

	if( utility::platformIsWindows() ){

		m_ui->lineEditExecutableKeySource->setText( m_settings.externalPluginExecutable() ) ;
	}else{
		m_ui->lineEditExecutableKeySource->setText( m_settings.executableSearchPath() ) ;
	}

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
	m_sirikali.enableAllAndSetFileManager() ;

	m_settings.setFileManager( m_ui->lineEditFileManager->text() ) ;
	m_settings.preUnMountCommand( m_ui->lineEditBeforesUnMount->text() ) ;
	m_settings.runCommandOnMount( m_ui->lineEditAfterMountCommand->text() ) ;
	m_settings.runCommandOnInterval( m_ui->lineEditRunPeriodically->text() ) ;
	m_settings.enableHighDpiScaling( m_ui->cbHiDPI->isChecked() ) ;

	if( utility::platformIsWindows() ){

		m_settings.setExternalPluginExecutable( m_ui->lineEditExecutableKeySource->text() ) ;
	}else{
		m_settings.setExecutableSearchPath( m_ui->lineEditExecutableKeySource->text() ) ;
	}

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

	this->hide() ;
}

void configOptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

bool configOptions::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}
