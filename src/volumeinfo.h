/*
 *
 *  Copyright (c) 2014-2015
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
#ifndef VOLUMEENTRYPROPERTIES_H
#define VOLUMEENTRYPROPERTIES_H

#include <QString>
#include <QStringList>

#include <initializer_list>

#include "favorites.h"
#include "utility.h"

class volumeInfo
{
public:
	static bool volumeIsSupported( const QString& e )
	{
		return utility::containsAtleastOne( e," fuse.cryfs ",
		                                    " fuse.encfs ",
		                                    " fuse.gocryptfs ",
		                                    " fuse.securefs ",
		                                    " ecryptfs " ) ;
	}
	volumeInfo( const favorites::entry& e )
	{
		this->setValues( { e.volumePath,e.mountPointPath,"","",
		                   e.configFilePath,e.idleTimeOut } ) ;
	}
	volumeInfo( const QStringList& l = QStringList() )
	{
		this->setValues( l ) ;
	}
	volumeInfo( const std::initializer_list<QString>& l )
	{
		this->setValues( l ) ;
	}
	const QString& volumePath() const
	{
		return m_volumePath ;
	}
	const QString& mountPoint() const
	{
		return m_mountPoint ;
	}
	const QString& configFilePath() const
	{
		return m_configPath ;
	}
	const QString& idleTimeOut() const
	{
		return m_idleTime ;
	}
	bool isValid() const
	{
		return !this->isNotValid() ;
	}
	bool isNotValid() const
	{
		return m_volumePath.isEmpty() ;
	}
	QStringList entryList() const
	{
		return { m_volumePath,m_mountPoint,m_fileSystem,m_mode } ;
	}
private:
	void setValues( const QStringList& l )
	{
		auto s = l.size() ;

		if( s == 1 ){

			m_volumePath = l.at( 0 ) ;

		}else if( s == 2 ){

			m_volumePath = l.at( 0 ) ;
			m_mountPoint = l.at( 1 ) ;

		}else if( s == 3 ){

			m_volumePath = l.at( 0 ) ;
			m_mountPoint = l.at( 1 ) ;
			m_fileSystem = l.at( 2 ) ;

		}else if( s == 4 ){

			m_volumePath = l.at( 0 ) ;
			m_mountPoint = l.at( 1 ) ;
			m_fileSystem = l.at( 2 ) ;
			m_mode       = l.at( 3 ) ;

		}else if( s == 5 ){

			m_volumePath = l.at( 0 ) ;
			m_mountPoint = l.at( 1 ) ;
			m_fileSystem = l.at( 2 ) ;
			m_mode       = l.at( 3 ) ;
			m_configPath = l.at( 4 ) ;

		}else if( s >= 6 ){

			m_volumePath = l.at( 0 ) ;
			m_mountPoint = l.at( 1 ) ;
			m_fileSystem = l.at( 2 ) ;
			m_mode       = l.at( 3 ) ;
			m_configPath = l.at( 4 ) ;
			m_idleTime   = l.at( 5 ) ;
		}

		if( m_configPath == "N/A" ){

			m_configPath.clear() ;
		}

		if( m_idleTime == "N/A" ){

			m_idleTime.clear() ;
		}
	}

	QString m_volumePath ;
	QString m_mountPoint ;
	QString m_fileSystem ;
	QString m_mode ;
	QString m_configPath ;
	QString m_idleTime ;
};

#endif // VOLUMEENTRYPROPERTIES_H
