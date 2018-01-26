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
#include "task.hpp"
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
			 const QString& volumeKey = QString() ) :

			cipherFolder( e.volumePath ),
			plainFolder( e.mountPointPath ),
			key( volumeKey ),
			idleTimeout( e.idleTimeOut ),
			configFilePath( e.configFilePath ),
			type( QString() ),
			ro( false ),
			mountOptions( e.mountOptions )
		{
		}
		options( const QString& cipher_folder,
			 const QString& plain_folder,
			 const QString& volume_key,
			 const QString& idle_timeout,
			 const QString& config_file_path,
			 const QString& volume_type,
			 bool unlock_in_read_only,
			 const QString& mount_options,
			 const QString& create_options ) :

			cipherFolder( cipher_folder ),
			plainFolder( plain_folder ),
			key( volume_key ),
			idleTimeout( idle_timeout ),
			configFilePath( config_file_path ),
			type( volume_type ),
			ro( unlock_in_read_only ),
			mountOptions( mount_options ),
			createOptions( create_options )
		{
		}

		QString cipherFolder ;
		QString plainFolder ;
		QString key ;
		QString idleTimeout ;
		QString configFilePath ;
		siritask::volumeType type ;
		bool ro ;
		QString mountOptions ;
		QString createOptions ;
	};

	enum class status
	{
		success,
		cryfs,
		cryfsMigrateFileSystem,
		encfs,
		gocryptfs,
		securefs,
		ecryptfs,
		ecryptfsIllegalPath,
		ecrypfsBadExePermissions,
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
		template< typename T = QString >
		cmdStatus( const siritask::cmdStatus& s,const T& e = T() )
		{
			m_status = s.status() ;
			m_exitCode = s.exitCode() ;

			if( e.isEmpty() ){

				this->message( s.msg() ) ;
			}else{
				this->message( e ) ;
			}
		}
		template< typename T = QString >
		cmdStatus( siritask::status s,const T& e = T() ) :
			m_status( s )
		{
			this->message( e ) ;
		}
		template< typename T = QString >
		cmdStatus( int s,const T& e = T() ) :
			m_exitCode( s )
		{
			this->message( e ) ;
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
		cmdStatus& setExitCode( int s )
		{
			m_exitCode = s ;
			return *this ;
		}
		cmdStatus& setStatus( siritask::status s )
		{
			m_status = s ;
			return *this ;
		}
		template< typename T >
		cmdStatus& setMessage( const T& e )
		{
			this->message( e ) ;
			return *this ;
		}
		const QString& msg() const
		{
			return m_message ;
		}
		QString report( const QString& cmd = QString() ) const
		{
			auto s = QString::number( m_exitCode ) ;

			QString e ;

			e += "-------------------------" ;
			e += QString( "\nBackend Generated Output:\nExit Code: %1" ).arg( s ) ;

			if( !m_message.isEmpty() ){

				e += QString( "\nExit String: \"%1\"" ).arg( m_message ) ;
			}

			if( !cmd.isEmpty() ){

				e += "\nCommand Sent To Backend: " + cmd ;
			}

			e+= "\n-------------------------" ;

			return e ;
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
	private:
		template< typename T >
		void message( const T& e )
		{
			m_message = e ;

			while( true ){

				if( m_message.endsWith( '\n' ) ){

					m_message.truncate( m_message.size() - 1 ) ;
				}else{
					break ;
				}
			}
		}

		int m_exitCode = -1 ;
		siritask::status m_status = siritask::status::backendFail ;
		QString m_message ;
	};

	bool deleteMountFolder( const QString& ) ;
	Task::future< bool >& encryptedFolderUnMount( const QString& cipherFolder,
						      const QString& mountPoint,
						      const QString& fileSystem ) ;

	Task::future< cmdStatus >& encryptedFolderMount( const siritask::options&,bool = false ) ;
	Task::future< cmdStatus >& encryptedFolderCreate( const siritask::options& ) ;
}

#endif // ZULUMOUNTTASK_H
