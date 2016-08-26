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


#ifndef MONITOR_MOUNTINFO_H
#define MONITOR_MOUNTINFO_H

#include <QThread>
#include <QString>
#include <QStringList>

#include <functional>
#include <memory>

class QObject ;
class volumeInfo ;

class mountinfo : public QThread
{
	Q_OBJECT
public:
	static QStringList mountedVolumes() ;

	static mountinfo& instance( QObject * parent,bool b,std::function< void() >&& f )
	{
		return *( new mountinfo( parent,b,std::move( f ) ) ) ;
	}
	mountinfo( QObject * parent,bool,std::function< void() >&& ) ;
	std::function< void() > stop() ;
	void announceEvents( bool ) ;
	~mountinfo() ;
signals:
	void gotEvent( void ) ;
	void gotEvent( QString ) ;
private slots:
	void threadStopped( void ) ;
private:
	void run( void ) ;
	void failedToStart( void ) ;
	QThread * m_baba ;
	QThread * m_mtoto ;
	QObject * m_babu ;
	mountinfo * m_main ;
	bool m_running ;
	std::function< void() > m_stop ;
	bool m_announceEvents ;
};

#endif // MONITOR_MOUNTINFO_H
