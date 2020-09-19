/*
 *
 *  Copyright ( c ) 2015
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


#include "plugin.h"
#include "plugins.h"
#include "crypto.h"
#include "ui_plugin.h"
#include "utility.h"
#include "dialogmsg.h"
#include "settings.h"

#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QIcon>

#include <memory>

plugin::plugin( QWidget * parent,
		QDialog * dialog,
		plugins::plugin plugin,
		const QString& e,
		std::function< void( const QByteArray& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::plugin ),
	m_function( std::move( function ) ),
	m_pluginType( plugin ),
	m_dialog( dialog )
{
	m_ui->setupUi( this ) ;

	m_ui->label->setText( e ) ;

	settings::instance().setParent( parent,&m_parentWidget,this ) ;

	this->setFixedSize( this->size() ) ;
	this->setFont( parent->font() ) ;

	connect( m_ui->pbSetKey,SIGNAL( clicked() ),this,SLOT( pbSetKey() ) ) ;
	connect( m_ui->pbCancel,SIGNAL( clicked() ),this,SLOT( pbClose() ) ) ;
	connect( m_ui->pbKeyFile,SIGNAL( clicked() ),this,SLOT( pbSelectKeyFile() ) ) ;

	m_ui->pbKeyFile->setIcon( QIcon( ":/file.png" ) ) ;

	m_ui->lineEdit->setFocus() ;

	utility::setWindowOptions( this ) ;

	this->ShowUI() ;
}

bool plugin::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void plugin::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}

plugin::~plugin()
{
	delete m_ui ;
}

void plugin::ShowUI()
{
	this->show() ;
	this->raise() ;
	this->activateWindow() ;
}

void plugin::HideUI()
{
	m_function( m_key ) ;

	this->hide() ;
	m_dialog->show() ;
	m_dialog->activateWindow() ;
	this->deleteLater() ;
}

void plugin::pbSetKey()
{
	auto passphrase = m_ui->lineEdit->text().toLatin1() ;
	auto keyFile    = m_ui->lineEdit_2->text() ;

	if( keyFile.isEmpty() && m_pluginType == plugins::plugin::hmac_key ){

		return DialogMsg( m_parentWidget,this ).ShowUIOK( tr( "ERROR" ),tr( "KeyFile Not Set" ) ) ;
	}

	this->disableAll() ;

	Task::run( [ this,passphrase,keyFile ](){

		if( m_pluginType == plugins::plugin::hmac_key ){

			return crypto::hmac_key( keyFile,passphrase ) ;

		}else if( m_pluginType == plugins::plugin::externalExecutable ){

			auto exe = settings::instance().externalPluginExecutable() ;

			if( exe.isEmpty() ){

				return QByteArray() ;
			}else{
				auto env = utility::systemEnvironment() ;

				env.insert( "PATH",utility::executableSearchPaths( env.value( "PATH" ) ) ) ;

				return utility::Task( exe,{ keyFile },20000,env,passphrase ).stdOut() ;
			}
		}else{
			return QByteArray() ;
		}

	} ).then( [ this ]( QByteArray e ){

		m_key = std::move( e ) ;

		if( m_key.isEmpty() ){

			DialogMsg( m_parentWidget,this ).ShowUIOK( tr( "ERROR" ),tr( "Failed To Generate Key" ) ) ;

			this->enableAll() ;
		}else{
			this->HideUI() ;
		}
	} ) ;
}

void plugin::pbSelectKeyFile()
{
	auto a = settings::instance().homePath() ;
	m_ui->lineEdit_2->setText( QFileDialog::getOpenFileName( this,tr( "KeyFile" ),a,nullptr ) ) ;
}

void plugin::pbClose()
{
	this->HideUI() ;
}

void plugin::enableAll()
{
	m_ui->pbCancel->setEnabled( true ) ;
	m_ui->pbKeyFile->setEnabled( true ) ;
	m_ui->pbSetKey->setEnabled( true ) ;
	m_ui->groupBox->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label_3->setEnabled( true ) ;
	m_ui->lineEdit->setEnabled( true ) ;
	m_ui->lineEdit_2->setEnabled( true ) ;
}

void plugin::disableAll()
{
	m_ui->pbCancel->setEnabled( false ) ;
	m_ui->pbKeyFile->setEnabled( false ) ;
	m_ui->pbSetKey->setEnabled( false ) ;
	m_ui->groupBox->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label_3->setEnabled( false ) ;
	m_ui->lineEdit->setEnabled( false ) ;
	m_ui->lineEdit_2->setEnabled( false ) ;
}
