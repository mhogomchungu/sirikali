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

#include "utility2.h"
#include "utility.h"
#include "favorites.h"

class volumeInfo
{
public:
	struct mountinfo
	{
		mountinfo()
		{
		}
		mountinfo( const QStringList& e )
		{
			utility2::stringListToStrings( e,
						      volumePath,
						      mountPoint,
						      fileSystem,
						      mode,
						      idleTimeout,
						      mountOptions ) ;
		}
		QStringList minimalList() const
		{
			return { volumePath,mountPoint,
				 fileSystem,mode } ;
		}
		QStringList fullList() const
		{
			return { volumePath,mountPoint,
				 fileSystem,mode,idleTimeout,mountOptions,configPath } ;
		}
		QString volumePath ;
		QString mountPoint ;
		QString fileSystem ;
		QString mode ;
		QString idleTimeout ;
		QString mountOptions ;
		QString configPath ;
	};
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
		m_reverseMode          = e.reverseMode ;
		m_readOnlyMode         = e.readOnlyMode ;

		if( e.configFilePath != "N/A" ){

			m_mountinfo.configPath = e.configFilePath ;
		}

		if( e.idleTimeOut != "N/A" ){

			m_mountinfo.idleTimeout = e.idleTimeOut ;
		}

		if( e.mountOptions != "N/A" ){

			m_mountinfo.mountOptions = e.mountOptions ;
		}
	}
	const favorites::triState& mountReadOnly() const
	{
		return m_readOnlyMode ;
	}
	bool reverseMode() const
	{
		return m_reverseMode ;
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
		return m_mountinfo.configPath ;
	}
	const QString& idleTimeOut() const
	{
		return m_mountinfo.idleTimeout ;
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
	bool m_reverseMode = false ;
	favorites::triState m_readOnlyMode ;
};

#endif // VOLUMEENTRYPROPERTIES_H
