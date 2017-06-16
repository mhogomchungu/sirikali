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

#include "task.h"

class QObject ;
class volumeInfo ;

class mountinfo
{
public:
	QStringList mountedVolumes() ;

	static std::unique_ptr< mountinfo > instance( QObject * parent,
						      bool b,
						      std::function< void() >&& f )
	{
		return std::unique_ptr< mountinfo >( new mountinfo( parent,b,std::move( f ) ) ) ;
	}
	mountinfo( QObject * parent,bool,std::function< void() >&& ) ;
	mountinfo() ;

	std::function< void() > stop() ;

	void announceEvents( bool ) ;
	void eventHappened( void ) ;

	~mountinfo() ;
private:
	void linuxMonitor( void ) ;
	void osxMonitor( void ) ;
	void updateVolume( void ) ;
	void pbUpdate( void ) ;
	void autoMount( const QString& ) ;

	QObject * m_parent ;

	std::function< void() > m_stop ;

	bool m_announceEvents ;
	bool m_linux ;

	QStringList m_oldMountList ;
	QStringList m_newMountList ;
};

#endif // MONITOR_MOUNTINFO_H
