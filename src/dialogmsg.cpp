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

DialogMsg::DialogMsg( QWidget * parent ) : QDialog( parent ),m_ui( new Ui::DialogMsg )
{
	m_ui->setupUi( this ) ;

	if( parent ){

		this->setFont( parent->font() ) ;
	}

	connect( m_ui->pbNo,SIGNAL( clicked() ),this,SLOT( pbNo() ) ) ;
	connect( m_ui->pbYes,SIGNAL( clicked() ),this,SLOT( pbYes() ) ) ;
	connect( m_ui->pbOk,SIGNAL( clicked() ),this,SLOT( pbOK() ) ) ;

	this->installEventFilter( this ) ;
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

	this->exec() ;
}

void DialogMsg::ShowPermissionProblem( const QString& device )
{
	Q_UNUSED( device ) ;	
}

void DialogMsg::ShowPermissionProblem( const QString& msg,const QString& device )
{
	Q_UNUSED( device ) ;
	Q_UNUSED( msg ) ;
}

void DialogMsg::setDimentions( const QString& msg )
{
	int len = msg.size() ;

	if( len <= 30 ){

		this->setFixedSize( 270,90 ) ;

		m_ui->label->setGeometry( 10,10,251,31 ) ;
		m_ui->label->setFixedSize( m_ui->label->size() ) ;
		m_ui->pbOk->setGeometry( 100,50,75,31 ) ;
		m_ui->pbYes->setGeometry( 70,50,71,31 ) ;
		m_ui->pbNo->setGeometry( 140,50,75,31 ) ;

	}else if( len <= 130 ){

		this->setFixedSize( 372,118 ) ;

		m_ui->label->setGeometry( 10,10,351,61 ) ;
		m_ui->label->setFixedSize( m_ui->label->size() ) ;
		m_ui->pbOk->setGeometry( 150,80,75,31 ) ;
		m_ui->pbYes->setGeometry( 120,80,71,31 ) ;
		m_ui->pbNo->setGeometry( 190,80,75,31 ) ;

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
	Q_UNUSED( title ) ;
	Q_UNUSED( m ) ;
}

void DialogMsg::ShowUIInfo( const QString& title,const QString& msg )
{
	m_ui->pbYes->setHidden( true ) ;
	m_ui->pbNo->setHidden( true ) ;
	m_ui->pbOk->setHidden( false ) ;

	m_ui->label->setAlignment( Qt::AlignHCenter|Qt::AlignVCenter ) ;

	this->setFixedSize( 562,338 ) ;

	m_ui->label->setGeometry( 20,10,531,271 ) ;
	m_ui->label->setFixedSize( m_ui->label->size() ) ;
	m_ui->pbOk->setGeometry( 240,290,81,31 ) ;

	this->HideLabels() ;
	this->ShowUI( title,msg ) ;
}

void DialogMsg::ShowUIOK( const QString& title,const QString& msg )
{
	m_ui->pbYes->setHidden( true ) ;
	m_ui->pbNo->setHidden( true ) ;
	m_ui->pbOk->setHidden( false ) ;

	this->HideLabels() ;
	this->setDimentions( msg ) ;
	this->ShowUI( title,msg ) ;
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
	this->SetUpButtons() ;
	this->setDimentions( msg ) ;
	this->ShowUI( title,msg ) ;

	return m_status;
}

int  DialogMsg::ShowUIYesNoDefaultNo( const QString& title,const QString& msg )
{
	this->SetUpButtons() ;
	this->setDimentions( msg ) ;

	m_ui->pbNo->setFocus() ;

	this->ShowUI( title,msg ) ;

	return m_status;
}

bool DialogMsg::ShowUIOKDoNotShowOption( const QString& title,const QString& msg )
{
	Q_UNUSED( title ) ;
	Q_UNUSED( msg ) ;

	return true ;
}

DialogMsg::~DialogMsg()
{
	delete m_ui ;
}
