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
#include "favorites.h"
#include <QVector>
#include <QString>
#include <QStringList>

namespace siritask
{
	class volumeType
	{
	public:
		volumeType()
		{
		}
		template< typename T >
		volumeType( const T& type ) : m_type( type )
		{
		}
		template< typename T >
		volumeType& operator=( const T& e )
		{
			m_type = e ;
			return *this ;
		}
		const QString& name() const
		{
			return m_type ;
		}
		template< typename T >
		bool startsWith( const T& e ) const
		{
			return m_type.startsWith( e ) ;
		}
		QString executableFullPath() const
		{
			return utility::executableFullPath( m_type ) ;
		}
		template< typename T >
		bool operator==( const T& type ) const
		{
			return m_type == type ;
		}
		template< typename T >
		bool operator!=( const T& type ) const
		{
			return m_type != type ;
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
		using function_t = std::function< void( const QString& ) > ;

		options( const favorites::entry& e,
			 const QString& volumeKey = QString(),
			 function_t folder_opener = []( const QString& e ){ Q_UNUSED( e ) ; } ) :

			cipherFolder( e.volumePath ),
			plainFolder( e.mountPointPath ),
			key( volumeKey ),
			mOpt( e.idleTimeOut ),
			configFilePath( e.configFilePath ),
			type( QString() ),
			ro( false ),
			openFolder( folder_opener )
		{
		}
		options( const QString& cipher_folder,
			 const QString& plain_folder,
			 const QString& volume_key,
			 const QString& mount_options,
			 const QString& config_file_path,
			 const QString& volume_type,
			 bool unlock_in_read_only,
			 function_t folder_opener = []( const QString& e ){ Q_UNUSED( e ) ; } ) :

			cipherFolder( cipher_folder ),
			plainFolder( plain_folder ),
			key( volume_key ),
			mOpt( mount_options ),
			configFilePath( config_file_path ),
			type( volume_type ),
			ro( unlock_in_read_only ),
			openFolder( folder_opener )
		{
		}

		QString cipherFolder ;
		QString plainFolder ;
		QString key ;
		QString mOpt ;
		QString configFilePath ;
		siritask::volumeType type ;
		bool ro ;
		function_t openFolder ;
	};

	enum class status
	{
		success,
		cryfs,
		encfs,
		gocryptfs,
		securefs,
		ecryptfs,
		gocryptfsNotFound,
		cryfsNotFound,
		encfsNotFound,
		securefsNotFound,
		ecryptfs_simpleNotFound,
		unknown,
		failedToCreateMountPoint,
		backendFail
	};

	class cmdStatus
	{
	public:
		cmdStatus()
		{
		}
		cmdStatus( const siritask::cmdStatus& s,const QString& e = QString() )
		{
			m_status = s.status() ;

			if( e.isEmpty() ){

				m_message = s.msg() ;
			}else{
				m_message = e ;
			}
		}
		cmdStatus( siritask::status s,const QString& e = QString() ) :
			m_status( s ),m_message( e )
		{
		}
		siritask::status status() const
		{
			return m_status ;
		}
		bool operator==( siritask::status s ) const
		{
			return m_status == s ;
		}
		bool operator!=( siritask::status s ) const
		{
			return m_status != s ;
		}
		const QString& msg() const
		{
			return m_message ;
		}
	private:
		siritask::status m_status ;
		QString m_message ;
	};

	bool deleteMountFolder( const QString& ) ;
	Task::future< QVector< volumeInfo > >& updateVolumeList( void ) ;
	Task::future< bool >& encryptedFolderUnMount( const QString& cipherFolder,
						      const QString& mountPoint,
						      const QString& fileSystem ) ;

	Task::future< cmdStatus >& encryptedFolderMount( const options&,bool = false ) ;
	Task::future< cmdStatus >& encryptedFolderCreate( const options& ) ;
}

#endif // ZULUMOUNTTASK_H
