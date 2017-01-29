/*
 *  Copyright ( c ) 2012-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation,either version 2 of the License,or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not,see <http://www.gnu.org/licenses/>.
 */

#include "dialogmsg.h"
#include "ui_dialogmsg.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QCheckBox>

#include "utility.h"
#include "dialogok.h"

DialogMsg::DialogMsg( QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::DialogMsg )
{
	m_ui->setupUi( this ) ;

	this->setFont( parent->font() ) ;

	utility::setParent( parent,&m_parent,this ) ;

	connect( m_ui->pbNo,SIGNAL( clicked() ),this,SLOT( pbNo() ) ) ;
	connect( m_ui->pbYes,SIGNAL( clicked() ),this,SLOT( pbYes() ) ) ;
	connect( m_ui->pbOk,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;

	this->installEventFilter( this ) ;

	utility::setWindowOptions( this ) ;
}

bool DialogMsg::eventFilter( QObject * watched,QEvent * event )
{
	return utility::eventFilter( this,watched,event,[ this ](){ this->HideUI() ; } ) ;
}

void DialogMsg::pbNo()
{
	m_status = ( int ) QMessageBox::No ;
	this->hide() ;
}

void DialogMsg::pbOK()
{
	m_status = ( int ) QMessageBox::Ok ;
	this->hide() ;
}

void DialogMsg::pbYes()
{
	m_status = ( int ) QMessageBox::Yes ;
	this->hide() ;
}

void DialogMsg::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}

void DialogMsg::HideUI()
{
	m_status = -1 ;
	this->hide() ;
}

void DialogMsg::ShowUI( const QString& title,const QString& msg )
{
	m_ui->label->setText( msg ) ;

	this->setWindowTitle( title ) ;

	this->show() ;
	this->raise() ;
	this->activateWindow() ;
	this->exec() ;
}

void DialogMsg::ShowPermissionProblem( const QString& device )
{
	Q_UNUSED( device ) ;
	QString msg = tr( "\n\
\"system volumes\" are volumes that either udev has identify them as such if udev is enabled \
or have an entry in \"/etc/fstab\",\"/etc/crypttab\" or \"/etc/zuluCrypt/system_volumes.list\".\n\
\n\
If you prefer for a volume not to be considered a system volume,start the tool\
from root account and then go to \"menu->options->manage non system partitions\" \
and add the volume to the list and the volume will stop being considered as \"system\".\n\n\
Alternatively,you can add yourself to group \"zulucrypt\" and \"zulumount\" and all restrictions will go away." ) ;

	this->ShowUIInfo( tr( "INFORMATION" ),false,msg ) ;
}

void DialogMsg::ShowPermissionProblem( const QString& msg,const QString& device )
{
	QString msg1 ;

	if( device.startsWith( "/dev/" ) ){

		if( msg == "reading" ){

			msg1 = tr( "Insufficient privilege to access a system device,\nonly root user or members of group zulucrypt can do that" ) ;
		}else{
			msg1 = tr( "Insufficient privilege to access a system device in read/write mode,\nonly root user or members of group zulucrypt-write can do that" ) ;
		}
	}else{
		msg1 = tr( "You do not seem to have proper permissions to access the encrypted file in %1 mode,check file permissions and try again" ).arg( msg ) ;
	}

	this->ShowUIOK( tr( "INFORMATION"),msg1 ) ;
}

void DialogMsg::setDimentions( const QString& msg )
{
	this->setFixedSize( 372,146 ) ;

	m_ui->label->setGeometry( 10,10,351,91 ) ;
	m_ui->label->setFixedSize( m_ui->label->size() ) ;
	m_ui->pbOk->setGeometry( 150,110,75,31 ) ;
	m_ui->pbYes->setGeometry( 120,110,71,31 ) ;
	m_ui->pbNo->setGeometry( 190,110,75,31 ) ;

	return ;

	int len = msg.size() ;

	if( len <= 30 ){

		this->setFixedSize( 270,90 ) ;

		m_ui->label->setGeometry( 10,10,251,31 ) ;
		m_ui->label->setFixedSize( m_ui->label->size() ) ;
		m_ui->pbOk->setGeometry( 100,50,75,31 ) ;
		m_ui->pbYes->setGeometry( 70,50,71,31 ) ;
		m_ui->pbNo->setGeometry( 140,50,75,31 ) ;

	}else if( len <= 130 ){

		this->setFixedSize( 372,138 ) ;

		m_ui->label->setGeometry( 10,10,351,81 ) ;
		m_ui->label->setFixedSize( m_ui->label->size() ) ;
		m_ui->pbOk->setGeometry( 150,100,75,31 ) ;
		m_ui->pbYes->setGeometry( 120,100,71,31 ) ;
		m_ui->pbNo->setGeometry( 190,100,75,31 ) ;

	}else if( len > 130 ){

		this->setFixedSize( 372,146 ) ;

		m_ui->label->setGeometry( 10,10,351,91 ) ;
		m_ui->label->setFixedSize( m_ui->label->size() ) ;
		m_ui->pbOk->setGeometry( 150,110,75,31 ) ;
		m_ui->pbYes->setGeometry( 120,110,71,31 ) ;
		m_ui->pbNo->setGeometry( 190,110,75,31 ) ;
	}
}

void DialogMsg::ShowLabels()
{
	m_ui->labelType->setHidden( false ) ;
	m_ui->labelCipher->setHidden( false ) ;
	m_ui->labelKeySize->setHidden( false ) ;
	m_ui->labelDevice->setHidden( false ) ;
	m_ui->labelLoop->setHidden( false ) ;
	m_ui->labelOffset->setHidden( false ) ;
	m_ui->labelSize->setHidden( false ) ;
	m_ui->labelMode->setHidden( false ) ;
	m_ui->labelFs->setHidden( false ) ;
	m_ui->labelSize_2->setHidden( false ) ;
	m_ui->labelUsed->setHidden( false ) ;
	m_ui->labelUnused->setHidden( false ) ;
	m_ui->labelUsedPerc->setHidden( false ) ;
	m_ui->labelActiveSlots->setHidden( false ) ;
}

void DialogMsg::HideLabels()
{
	m_ui->labelActiveSlots->setHidden( true ) ;
	m_ui->labelType->setHidden( true ) ;
	m_ui->labelCipher->setHidden( true ) ;
	m_ui->labelKeySize->setHidden( true ) ;
	m_ui->labelDevice->setHidden( true ) ;
	m_ui->labelLoop->setHidden( true ) ;
	m_ui->labelOffset->setHidden( true ) ;
	m_ui->labelSize->setHidden( true ) ;
	m_ui->labelMode->setHidden( true ) ;
	m_ui->labelFs->setHidden( true ) ;
	m_ui->labelSize_2->setHidden( true ) ;
	m_ui->labelUsed->setHidden( true ) ;
	m_ui->labelUnused->setHidden( true ) ;
	m_ui->labelUsedPerc->setHidden( true ) ;
}

void DialogMsg::ShowUIVolumeProperties( const QString& title,const QString& m )
{
	m_ui->pbYes->setHidden( true ) ;
	m_ui->pbNo->setHidden( true ) ;
	m_ui->pbOk->setHidden( false ) ;

	m_ui->label->setHidden( true ) ;

	QString msg = m ;
	msg.remove( "   " ) ;

	auto stl = msg.split( "\n" ) ;

	if( stl.size() >= 14 ){

		this->setFixedSize( this->size() ) ;

		auto _trancate_long_path = []( const QString& e )->QString{

			const int len    = 40 ;
			const int length = e.length() ;

			if( length <= len ){

				return e ;
			}else{
				return e.mid( 0,18 ) + "...." + e.mid( length - 18 ) ;
			}
		} ;

		auto _replace = [ & ]( int position,const char * txt,const QString& translated ){

			auto e = stl.at( position ) ;

			e.replace( txt,translated ) ;

			return e ;
		} ;

		m_ui->labelType->setText( _replace( 0,"type:",tr( "type:" ) ) ) ;
		m_ui->labelCipher->setText( _replace( 1,"cipher:",tr( "cipher:" ) ) ) ;
		m_ui->labelKeySize->setText( _replace( 2,"keysize:",tr( "keysize:" ) ) ) ;
		m_ui->labelOffset->setText( _replace( 3,"offset:",tr( "offset:" ) ) ) ;
		m_ui->labelDevice->setText( _trancate_long_path( _replace( 4,"device:",tr( "device:" ) ) ) ) ;
		m_ui->labelLoop->setText( _trancate_long_path( _replace( 5,"loop:",tr( "loop:" ) ) ) ) ;
		m_ui->labelSize->setText( _replace( 6,"mode:",tr( "mode:" ) ) ) ;
		m_ui->labelMode->setText( _replace( 7,"active slots:",tr( "active slots:" ) ) ) ;
		m_ui->labelActiveSlots->setText( _replace( 8,"file system:",tr( "file system:" ) ) ) ;
		m_ui->labelFs->setText( _replace( 9,"total space:",tr( "total space:" ) ) ) ;
		m_ui->labelSize_2->setText( _replace( 10,"used space:",tr( "used space:" ) ) ) ;
		m_ui->labelUsed->setText( _replace( 11,"free space:",tr( "free space:" ) ) ) ;
		m_ui->labelUnused->setText( _replace( 12,"used%:",tr( "used%:" ) ) ) ;
		m_ui->labelUsedPerc->setText( _replace( 13,"UUID:",tr( "UUID:" ) ) ) ;

		//QString m = stl.at( 15 ) ;
		//m.replace( QString( "mount point2" ),QString( "public mount point" ) ) ;
		//m_ui->labelUsedPerc->setText( m ) ;

		this->ShowUI( title,msg ) ;
	}
}

void DialogMsg::ShowUIInfo( const QString& title,bool centered,const QString& msg )
{
	m_ui->pbYes->setHidden( true ) ;
	m_ui->pbNo->setHidden( true ) ;
	m_ui->pbOk->setHidden( false ) ;

	if( centered ){

		m_ui->label->setAlignment( Qt::AlignHCenter|Qt::AlignVCenter ) ;
	}else{
		m_ui->label->setAlignment( Qt::AlignLeft|Qt::AlignVCenter ) ;
	}

	this->setFixedSize( 562,338 ) ;

	m_ui->label->setGeometry( 20,10,531,271 ) ;
	m_ui->label->setFixedSize( m_ui->label->size() ) ;
	m_ui->pbOk->setGeometry( 240,290,81,31 ) ;

	this->HideLabels() ;
	this->ShowUI( title,msg ) ;
}

void DialogMsg::ShowUIOK( const QString& title,const QString& msg )
{
	dialogok( m_parent,false,false,title,msg ).Show() ;

	//m_ui->pbYes->setHidden( true ) ;
	//m_ui->pbNo->setHidden( true ) ;
	//m_ui->pbOk->setHidden( false ) ;

	//this->HideLabels() ;
	//this->setDimentions( msg ) ;
	//this->ShowUI( title,msg ) ;
}

void DialogMsg::SetUpButtons()
{
	m_ui->pbYes->setHidden( false ) ;
	m_ui->pbNo->setHidden( false ) ;
	m_ui->pbOk->setHidden( true ) ;

	this->HideLabels() ;
}

int DialogMsg::ShowUIYesNo( const QString& title,const QString& msg )
{
	return dialogok( m_parent,true,false,title,msg ).Show() ;

	//this->SetUpButtons() ;
	//this->setDimentions( msg ) ;
	//this->ShowUI( title,msg ) ;

	//return m_status;
}

int  DialogMsg::ShowUIYesNoDefaultNo( const QString& title,const QString& msg )
{
	return dialogok( m_parent,true,true,title,msg ).Show() ;

	//this->SetUpButtons() ;
	//this->setDimentions( msg ) ;

	//m_ui->pbNo->setFocus() ;

	//this->ShowUI( title,msg ) ;

	//return m_status ;
}

bool DialogMsg::ShowUIOKDoNotShowOption( const QString& title,const QString& msg )
{
	QCheckBox checkBox( tr( "Do not show this dialog again" ),this ) ;

	this->setFixedSize( 270,110 ) ;

	checkBox.setGeometry( 30,40,251,31 ) ;

	m_ui->label->setGeometry( 10,10,251,31 ) ;
	m_ui->label->setFixedSize( m_ui->label->size() ) ;
	m_ui->pbOk->setGeometry( 100,70,75,31 ) ;

	m_ui->pbYes->setHidden( true ) ;
	m_ui->pbNo->setHidden( true ) ;
	m_ui->pbOk->setHidden( false ) ;

	this->HideLabels() ;

	this->ShowUI( title,msg ) ;

	return checkBox.isChecked() ;
}

DialogMsg::~DialogMsg()
{
	m_parent->raise() ;
	m_parent->activateWindow() ;
	delete m_ui ;
}
