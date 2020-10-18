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

	utility::logger logger ;

	SirikaliJson config( logger.function() ) ;

	auto _addList = [ & ]( const QString& e ){

		if( e.isEmpty() ){

			return QStringList() ;
		}else{
			return utility::split( e,',' ) ;
		}
	} ;

	QStringList emptyStringList ;

	config[ "createControlStructure" ]      = m_ui->lineEditCreateControlStructure->text() ;
	config[ "mountControlStructure" ]       = m_ui->lineEditMountControlStructure->text() ;
	config[ "idleString" ]                  = "--idle %{timeout}" ;
	config[ "reverseString" ]               = "" ;
	config[ "keyFileArgument" ]             = "--keyfile %{keyfile}" ;
	config[ "unMountCommand" ]              = emptyStringList ;
	config[ "windowsUnMountCommand" ]       = emptyStringList ;
	config[ "fileExtensions" ]              = emptyStringList ;
	config[ "unMountCommand" ]              = emptyStringList ;
	config[ "volumePropertiesCommands" ]    = emptyStringList ;
	config[ "passwordFormat" ]              = "%{password}" ;
	config[ "defaultFavoritesMountOptions" ]    = "" ;
	config[ "windowsInstallPathRegistryKey" ]   = "" ;
	config[ "windowsInstallPathRegistryValue" ] = "" ;
	config[ "windowsExecutableFolderPath" ]     = "" ;
	config[ "displayName" ]                     = "" ;
	config[ "sirikaliMinimumVersion" ]          = "" ;
	config[ "windowsSupportsMountPointPaths" ]  = m_ui->cbSupportsMountPointPaths->isChecked() ;
	config[ "configFileNames" ]                   = _addList( configFileNames ) ;
	config[ "fuseNames" ]                         = _addList( fusenames ) ;
	config[ "names" ]                             = _addList( names ) ;
	config[ "stringsToCheckForFailedMount" ]      = _addList( m_ui->lineEditFailedToMountText->text() ) ;
	config[ "stringsToCheckForSuccessfulMount" ]  = _addList( m_ui->lineEditSuccessfullyMountedText->text() ) ;
	config[ "configFileArgument" ]          = m_ui->lineEditConfigFileArgument->text() ;
	config[ "wrongPasswordText" ]           = password ;
	config[ "wrongPasswordErrorCode" ]      = m_ui->lineEditWrongPasswordErrorCode->text() ;
	config[ "requiresAPassword" ]           = m_ui->cbRequiresAPassword->isChecked() ;
	config[ "autoMountsOnVolumeCreation" ]  = m_ui->cbAutoMountsOnVolumeCreation->isChecked() ;
	config[ "executableName" ]              = executable ;
	config[ "backendRequireMountPath" ]     = true ;
	config[ "autorefreshOnMountUnMount" ]   = true ;
	config[ "backendTimeout" ]              = 0 ;
	config[ "takesTooLongToUnlock" ]        = false ;
	config[ "runsInBackGround" ]            = true ;
	config[ "setsCipherPath" ]              = true ;
	config[ "acceptsSubType" ]              = true ;
	config[ "acceptsVolName" ]              = true ;
	config[ "likeSsh" ]                     = false ;
	config[ "autoCreatesMountPoint" ]       = false ;
	config[ "autoDeletesMountPoint" ]       = false ;
	config[ "usesOnlyMountPoint" ]          = false ;
	config[ "versionArgumentString" ]       = "" ;
	config[ "versionMinimum" ]              = "" ;
	config[ "versionOutputStdOut" ]         = true ;
	config[ "versionStringTextPosition" ]   = std::vector< int >( { 0,0 } ) ;

	if( config.passed() ){

		config.toFile( path ) ;
	}

	this->Hide() ;
}
