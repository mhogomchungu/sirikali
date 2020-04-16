/*
 *
 *  Copyright ( c ) 2019
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

#include "createbackendwindow.h"
#include "ui_createbackendwindow.h"

#include "json_parser.hpp"
#include "utility.h"
#include "dialogmsg.h"

#include <string>
#include <vector>

#include <QFile>

createBackendWIndow::createBackendWIndow( QWidget * parent ) :
	QDialog( parent ),
	m_ui( new Ui::createBackendWIndow )
{
	m_ui->setupUi( this ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbCreate,&QPushButton::clicked,[ this ](){ this->save() ; } ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){ this->Hide() ; } ) ;

	this->show() ;
}

createBackendWIndow::~createBackendWIndow()
{
	delete m_ui ;
}

void createBackendWIndow::Hide()
{
	this->hide() ;
	this->deleteLater() ;
}

void createBackendWIndow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->Hide() ;
}

void createBackendWIndow::save()
{
	auto pluginname = m_ui->lineEditPluginName->text() ;

	if( pluginname.isEmpty() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Invalid Input Encountered" ) ) ;
	}

	QString path ;

	if( utility::platformIsWindows() ){

		path = QDir::homePath() + "/Desktop/" + pluginname + ".json" ;
	}else{
		path = QDir::homePath() + "/" + pluginname + ".json" ;
	}

	if( utility::pathExists( path ) ){

		auto m = tr( "The Config File Will Be Created At \"%1\" But The Path Is Already Taken" ) ;

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),m.arg( path ) ) ;
	}

	auto executable = m_ui->lineEditExecutableName->text() ;

	if( executable.isEmpty() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Executable Field Can Not Be Empty" ) ) ;
	}

	auto configFileNames = m_ui->lineEditConfigFileNames->text() ;

	//if( configFileNames.isEmpty() ){

	//	return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "ConfigFileNames Field Can Not Be Empty" ) ) ;
	//}

	auto fusenames = m_ui->lineEditFuseNames->text() ;

	if( fusenames.isEmpty() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "FuseNames Field Can Not Be Empty" ) ) ;
	}

	auto names = m_ui->lineEditNames->text() ;

	if( names.isEmpty() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "BackendNames Field Can Not Be Empty" ) ) ;
	}

	auto password = m_ui->lineEditWrongPasswordText->text() ;

	if( password.isEmpty() && m_ui->cbRequiresAPassword->isChecked() ){

		return DialogMsg( this ).ShowUIOK( tr( "ERROR" ),tr( "Wrong Password Field Can Not Be Empty" ) ) ;
	}

	SirikaliJson config ;

	auto _addList = [ & ]( const QString& e ){

		if( e.isEmpty() ){

			return QStringList() ;
		}else{
			return utility::split( e,',' ) ;
		}
	} ;

	auto _log_error = []( const QString& msg,const QString& path ){

		auto a = "\nFailed to parse file for writing: " + path ;

		utility::debug::logErrorWhileStarting( msg + a ) ;
	} ;

	try {
		config[ "versionNumber" ]               = "1.1" ;

		//start of version 1.0
		config[ "createControlStructure" ]      = "%{createOptions} %{cipherFolder} %{mountPoint}" ;
		config[ "mountControlStructure" ]       = "%{mountOptions} %{cipherFolder} %{mountPoint} %{fuseOpts}" ;
		config[ "idleString" ]                  = "" ;
		config[ "reverseString" ]               = "" ;
		config[ "unMountCommand" ]              = "" ;
		config[ "windowsUnMountCommand" ]       = "" ;
		config[ "passwordFormat" ]              = "%{password}" ;
		config[ "windowsInstallPathRegistryKey" ]   = "" ;
		config[ "windowsInstallPathRegistryValue" ] = "" ;
		config[ "executableName" ]              = executable ;
		config[ "configFileNames" ]             = _addList( configFileNames ) ;
		config[ "fuseNames" ]                   = _addList( fusenames ) ;
		config[ "names" ]                       = _addList( names ) ;
		config[ "fileExtensions" ]              = QStringList() ;
		config[ "volumePropertiesCommands" ]    = QStringList() ;
		config[ "failedToMountTextList" ]       = _addList( m_ui->lineEditFailedToMountText->text() ) ;
		config[ "successfullyMountedList" ]     = _addList( m_ui->lineEditSuccessfullyMountedText->text() ) ;
		config[ "configFileArgument" ]          = m_ui->lineEditConfigFileArgument->text() ;
		config[ "wrongPasswordText" ]           = password ;
		config[ "wrongPasswordErrorCode" ]      = m_ui->lineEditWrongPasswordErrorCode->text() ;
		config[ "requiresAPassword" ]           = m_ui->cbRequiresAPassword->isChecked() ;
		config[ "supportsMountPointPaths" ]     = m_ui->cbSupportsMountPointPaths->isChecked() ;
		config[ "autoMountsOnVolumeCreation" ]  = m_ui->cbAutoMountsOnVolumeCreation->isChecked() ;

		//start of version 1.1
		config[ "backendRequireMountPath" ]     = true ;
		config[ "autorefreshOnMountUnMount" ]   = true ;
		config[ "backendTimeout" ]              = 0 ;
		config[ "takesTooLongToUnlock" ]        = false ;

		config.toFile( path ) ;

	}catch( const SirikaliJson::exception& e ){

		_log_error( e.what(),path ) ;

	}catch( const std::exception& e ){

		_log_error( e.what(),path ) ;

	}catch( ... ){

		_log_error( "Unknown error has occured",path ) ;
	}

	this->Hide() ;
}
