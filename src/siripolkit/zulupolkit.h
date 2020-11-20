#ifndef ZULUPOLKIT_H
#define ZULUPOLKIT_H

/*
 *
 *  Copyright ( c ) 2017
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

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class zuluPolkit : public QObject
{
public:
	zuluPolkit( const QStringList& ) ;
	void start() ;
	~zuluPolkit() ;
private:
	void gotConnection() ;
	bool passSanityCheck( const QString& cmd,const QStringList& s ) ;
	QStringList m_arguments ;
	QString readStdin() ;
	QString m_cookie ;
	QLocalServer m_server ;
	QString m_socketPath ;
	QString m_suCmd ;
	QString m_ecryptfs_simpleCmd ;
};

#endif // ZULUPOLKIT_H
