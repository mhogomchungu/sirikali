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

#ifndef ZULUMOUNTTASK_H
#define ZULUMOUNTTASK_H

#include "volumeinfo.h"
#include "task.h"
#include "utility.h"

#include <QVector>
#include <QString>
#include <QStringList>

namespace siritask
{
	class volumeType
	{
	public:
		volumeType( const QString& type ) : m_type( type )
		{
		}
		volumeType( const char * type ) : m_type( type )
		{
		}
		volumeType& operator=( const char * e )
		{
			m_type = e ;
			return *this ;
		}
		const QString& name() const
		{
			return m_type ;
		}
		QString executableFullPath() const
		{
			return utility::executableFullPath( m_type ) ;
		}
		bool operator==( const char * type ) const
		{
			return m_type == type ;
		}
		template< typename ... T >
		bool isOneOf( const T& ... t ) const
		{
			return utility::equalsAtleastOne( m_type,t ... ) ;
		}
	private:
		QString m_type ;
	};

	struct options
	{
		QString cipherFolder ;
		QString plainFolder ;
		QString key ;
		QString mOpt ;
		QString configFilePath ;
		siritask::volumeType type ;
		bool ro ;
		std::function< void( const QString& ) > openFolder ;
	};

	enum class status
	{
		success,
		cryfs,
		encfs,
		gocryptfs,
		securefs,
		gocryptfsNotFound,
		cryfsNotFound,
		encfsNotFound,
		securefsNotFound,
		unknown,
		failedToCreateMountPoint,
		backendFail
	};

	bool deleteMountFolder( const QString& ) ;
	Task::future< QVector< volumeInfo > >& updateVolumeList( void ) ;
	Task::future< bool >& encryptedFolderUnMount( const QString& mountPoint ) ;
	Task::future< siritask::status >& encryptedFolderMount( const options&,bool = false ) ;
	Task::future< siritask::status >& encryptedFolderCreate( const options& ) ;
}

#endif // ZULUMOUNTTASK_H
