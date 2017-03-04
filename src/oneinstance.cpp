
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

#include "oneinstance.h"
#include <QDebug>
#include "utility.h"
#include <memory>
#include <utility>

oneinstance::oneinstance( QObject * parent,const char * socketPath,const char * methodName,
			  const QString& device,std::function<void( QObject * )> start ) : m_firstInstance( std::move( start ) )
{
	m_device = device ;

	this->setParent( parent ) ;

	m_serverPath = utility::homePath() + "/.SiriKali/.tmp/" ;

	m_methodName = methodName ;

	QDir d ;

	d.mkpath( m_serverPath ) ;

	m_serverPath += socketPath ;

	if( QFile::exists( m_serverPath ) ){

		m_localSocket = new QLocalSocket() ;

		connect( m_localSocket,SIGNAL( destroyed( QObject * ) ),this,SLOT( Exit( QObject * ) ) ) ;
		connect( m_localSocket,SIGNAL( connected() ),this,SLOT( connected() ) ) ;
		connect( m_localSocket,SIGNAL( error( QLocalSocket::LocalSocketError ) ),
			 this,SLOT( errorOnConnect( QLocalSocket::LocalSocketError ) ) ) ;

		m_localSocket->connectToServer( m_serverPath ) ;
	}else{
		this->startInstance() ;
	}
}

void oneinstance::startInstance()
{
	m_firstInstance( this ) ;

	QMetaObject::invokeMethod( this->parent(),m_methodName,Qt::QueuedConnection ) ;

	m_localServer = new QLocalServer() ;

	connect( m_localServer,SIGNAL( newConnection() ),this,SLOT( gotConnection() ) ) ;

	m_localServer->listen( QString( m_serverPath ) ) ;
}

void oneinstance::Exit( QObject * e )
{
	Q_UNUSED( e ) ;
	//QCoreApplication::exit( 200 ) ;
	exit( 0 ) ;
}

void oneinstance::setDevice( QString device )
{
	m_device = device ;
}

void oneinstance::killProcess()
{
	QMetaObject::invokeMethod( this,"Exit",Qt::QueuedConnection ) ;
}

void oneinstance::gotConnection()
{
	std::unique_ptr<QLocalSocket> s( m_localServer->nextPendingConnection() ) ;

	s->waitForReadyRead() ;

	auto data = s->readAll() ;

	if( data.isEmpty() ){

		emit raise() ;
	}else{
		emit raiseWithDevice( data ) ;
	}
}

void oneinstance::errorOnConnect( QLocalSocket::LocalSocketError e )
{
	Q_UNUSED( e ) ;
	qDebug() << tr( "Previous instance seem to have crashed,trying to clean up before starting" ) ;
	QFile::remove( m_serverPath ) ;
	this->startInstance() ;
}

void oneinstance::connected()
{
	qDebug() << tr( "There seem to be another instance running,exiting this one" ) ;

	if( !m_device.isEmpty() ){

		m_localSocket->write( m_device.toLatin1() ) ;
		m_localSocket->waitForBytesWritten() ;
	}

	m_localSocket->close() ;

	m_localSocket->deleteLater() ;
}

oneinstance::~oneinstance()
{
	if( m_localServer ){
		m_localServer->close() ;
		delete m_localServer ;
		QFile::remove( m_serverPath ) ;
	}
}
