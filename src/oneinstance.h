
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

#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QString>

#include <functional>

class oneinstance : public QObject
{
	Q_OBJECT
public:
	static void instance( QObject * a,
			      const QString& b,
			      const QString& c,
			      std::function< void( const QString& ) > d,
			      std::function< void( int ) > e,
			      std::function< void( const QString& ) > f )
	{
		new oneinstance( a,b,c,std::move( d ),std::move( e ),std::move( f ) ) ;
	}
	oneinstance( QObject *,
		     const QString&,
		     const QString&,
		     std::function< void( const QString& ) >,
		     std::function< void( int ) >,
		     std::function< void( const QString& ) > ) ;
	~oneinstance() ;
private slots:
	void connected( void ) ;
	void gotConnection( void ) ;
	void errorOnConnect( QLocalSocket::LocalSocketError ) ;
private:
	void start( void ) ;
	QLocalServer m_localServer ;
	QLocalSocket m_localSocket ;
	QString m_serverPath ;
	QString m_argument ;
	std::function< void( const QString& ) > m_start ;
	std::function< void( int ) > m_exit ;
	std::function< void( const QString& ) > m_event ;
};

#endif // ONEINSTANCE_H
