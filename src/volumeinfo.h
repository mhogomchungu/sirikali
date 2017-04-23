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
	struct mountinfo
	{
		mountinfo()
		{
		}
		mountinfo( const QStringList& l )
		{
			const auto s = l.size() ;

			if( s == 1 ){

				volumePath = l.at( 0 ) ;

			}else if( s == 2 ){

				volumePath = l.at( 0 ) ;
				mountPoint = l.at( 1 ) ;

			}else if( s == 3 ){

				volumePath = l.at( 0 ) ;
				mountPoint = l.at( 1 ) ;
				fileSystem = l.at( 2 ) ;

			}else if( s == 4 ){

				volumePath = l.at( 0 ) ;
				mountPoint = l.at( 1 ) ;
				fileSystem = l.at( 2 ) ;
				mode       = l.at( 3 ) ;

			}else if( s == 5 ){

				volumePath = l.at( 0 ) ;
				mountPoint = l.at( 1 ) ;
				fileSystem = l.at( 2 ) ;
				mode       = l.at( 3 ) ;
				idleTimeout = l.at( 4 ) ;

			}else if( s == 6 ){

				volumePath = l.at( 0 ) ;
				mountPoint = l.at( 1 ) ;
				fileSystem = l.at( 2 ) ;
				mode       = l.at( 3 ) ;
				idleTimeout = l.at( 4 ) ;
				mountOptions = l.at( 5 ) ;
			}
		}
		QStringList minimalList() const
		{
			return { volumePath,mountPoint,
				 fileSystem,mode } ;
		}
		QStringList fullList() const
		{
			return { volumePath,mountPoint,
				 fileSystem,mode,idleTimeout,mountOptions } ;
		}
		QString volumePath ;
		QString mountPoint ;
		QString fileSystem ;
		QString mode ;
		QString idleTimeout ;
		QString mountOptions ;
	};

	static bool supported( const QString& e )
	{
		return utility::containsAtleastOne( e," fuse.cryfs ",
		                                    " fuse.encfs ",
		                                    " fuse.gocryptfs ",
		                                    " fuse.securefs ",
		                                    " ecryptfs " ) ;
	}
	volumeInfo()
	{
	}
	volumeInfo( const std::initializer_list< QString >& e ) : m_mountinfo( e )
	{
	}
	volumeInfo( const QStringList& e ) : m_mountinfo( e )
	{
	}
	volumeInfo( const volumeInfo::mountinfo& e ) : m_mountinfo( e )
	{
	}
	volumeInfo( const favorites::entry& e )
	{
		m_mountinfo.volumePath = e.volumePath ;
		m_mountinfo.mountPoint = e.mountPointPath ;

		if( e.configFilePath != "N/A" ){

			m_configPath = e.configFilePath ;
		}

		if( e.idleTimeOut != "N/A" ){

			m_idleTime = e.idleTimeOut ;
		}

		if( e.mountOptions != "N/A" ){

			m_mountinfo.mountOptions = e.mountOptions ;
		}
	}
	const QString& volumePath() const
	{
		return m_mountinfo.volumePath ;
	}
	const QString& mountPoint() const
	{
		return m_mountinfo.mountPoint ;
	}
	const QString& fileSystem() const
	{
		return m_mountinfo.fileSystem ;
	}
	const QString& configFilePath() const
	{
		return m_configPath ;
	}
	const QString& idleTimeOut() const
	{
		return m_idleTime ;
	}
	const QString& mountOptions() const
	{
		return m_mountinfo.mountOptions ;
	}
	const volumeInfo::mountinfo& mountInfo() const
	{
		return m_mountinfo ;
	}
	void printVolumeInfo() const
	{
		std::cout << "\"" +
		             m_mountinfo.volumePath.toStdString() +
		             "\" \"" + m_mountinfo.mountPoint.toStdString() +
		             "\" \"" + m_mountinfo.fileSystem.toStdString() +
		             "\"" << std::endl ;
	}
	bool isValid() const
	{
		return !this->isNotValid() ;
	}
	bool isNotValid() const
	{
		return this->volumePath().isEmpty() ;
	}
private:
	volumeInfo::mountinfo m_mountinfo ;
	QString m_configPath ;
	QString m_idleTime ;
};

#endif // VOLUMEENTRYPROPERTIES_H
