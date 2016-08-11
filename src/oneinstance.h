
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

#ifndef ONEINSTANCE_H
#define ONEINSTANCE_H

#include <Qt>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QFile>
#include <QCoreApplication>
#include <QMainWindow>
#include <QString>
#include <QMetaObject>
#include <QDir>
#include <functional>
#include <QObject>

class oneinstance : public QObject
{
	Q_OBJECT
public:
	static void instance( QObject * a,const char * b,const char * c,const QString& d,std::function<void( QObject * )> e )
	{
		new oneinstance( a,b,c,d,std::move( e ) ) ;
	}
	explicit oneinstance( QObject *,const char *,const char *,const QString&,std::function<void( QObject * )> ) ;
	~oneinstance() ;
	void setDevice( QString ) ;
signals:
	void raise( void ) ;
	void raiseWithDevice( QString ) ;
private slots:
	void connected( void ) ;
	void gotConnection( void ) ;
	void errorOnConnect( QLocalSocket::LocalSocketError ) ;
	void Exit( QObject * ) ;
private:
	void startInstance( void ) ;
	void killProcess( void ) ;
	QLocalServer * m_localServer = nullptr ;
	QLocalSocket * m_localSocket = nullptr ;
	QString m_serverPath ;
	const char * m_methodName ;
	QString m_device ;
	std::function<void( QObject * )> m_firstInstance ;
};

#endif // ONEINSTANCE_H
