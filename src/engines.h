/*
 *
 *  Copyright (c) 2018
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

#ifndef BACKEND_H
#define BACKEND_H

#include <vector>
#include <memory>
#include <functional>

#include <QString>
#include <QStringList>
#include <QWidget>

#include "favorites.h"

class engines
{
public:
	static const engines& instance() ;
	static QStringList supported() ;

	class engine
	{
	public:
		enum class status
		{
			success,
			volumeCreatedSuccessfully,

			cryfsBadPassword,
			encfsBadPassword,
			sshfsBadPassword,
			gocryptfsBadPassword,
			securefsBadPassword,
			ecryptfsBadPassword,

			sshfsNotFound,
			cryfsNotFound,
			encfsNotFound,
			securefsNotFound,
			gocryptfsNotFound,
			ecryptfs_simpleNotFound,

			cryfsMigrateFileSystem,

			failedToLoadWinfsp,

			ecryptfsIllegalPath,
			ecrypfsBadExePermissions,

			backEndDoesNotSupportCustomConfigPath,
			failedToCreateMountPoint,
			backendFail,
			unknown
		} ;

		struct options
		{
			options( const favorites::entry& e,
				 const QString& volumeKey = QString() ) ;

			options( const QString& cipher_folder,
				 const QString& plain_folder,
				 const QString& volume_key,
				 const QString& idle_timeout,
				 const QString& config_file_path,
				 const QString& volume_type,
				 bool unlock_in_read_only,
				 bool unlock_in_reverse_mode,
				 const QString& mount_options,
				 const QString& create_options ) ;
			QString cipherFolder ;
			QString plainFolder ;
			QString key ;
			QString idleTimeout ;
			QString configFilePath ;
			QString type ;
			bool ro ;
			bool reverseMode ;
			QString mountOptions ;
			QString createOptions ;
		} ;

		struct cmdArgsList
		{
			const QString& exe ;
			const engines::engine::options& opt ;
			const QString& configFilePath ;
			const QString& cipherFolder ;
			const QString& mountPoint ;
			const bool create ;
		} ;

		class cmdStatus
		{
		public:
			cmdStatus() ;
			cmdStatus( const engine::cmdStatus& s,const QString& e = QString() ) ;
			cmdStatus( engines::engine::status s,int c,const QString& e = QString() ) ;
			cmdStatus( engines::engine::status s,const QString& e = QString() ) ;
			cmdStatus( int s,const QString& e = QString() ) ;
			engines::engine::status status() const ;
			bool operator==( engine::status s ) const ;
			bool operator!=( engine::status s ) const ;
			engines::engine::cmdStatus& setExitCode( int s ) ;
			engines::engine::cmdStatus& setStatus( engines::engine::status s ) ;
			engines::engine::cmdStatus& setMessage( const QString& e ) ;
			const QString& msg() const ;
			QString report( const QString& cmd = QString() ) const ;
			int exitCode() const ;
		private:
			void message( const QString& e ) ;

			int m_exitCode = -1 ;
			engines::engine::status m_status = engines::engine::status::backendFail ;
			QString m_message ;
		} ;

		struct Options
		{
			Options( QStringList s,bool r ) ;
			Options( QStringList s ) ;
			Options( bool r ) ;
			Options() ;
			QStringList options ;
			bool reverseMode ;
			bool success ;
		} ;

		engine( const QString& name ) ;
		QString executableFullPath() const ;
		bool isInstalled() const ;
		bool isNotInstalled() const ;
		bool unknown() const ;
		bool known() const ;
		virtual ~engine() ;
		virtual QString defaultCreateOptions() const = 0 ;
		virtual QString configFileArgument() const = 0 ;
		virtual QString setPassword( const QString& ) const = 0 ;
		virtual QString command( const engines::engine::cmdArgsList& args ) const = 0 ;
		virtual engines::engine::status errorCode( const QString& e,int s ) const = 0 ;
		virtual engines::engine::status notFoundCode() const = 0 ;
		virtual const QStringList& names() const = 0 ;
		virtual const QStringList& fuseNames() const = 0 ;
		virtual QStringList configFileNames() const = 0 ;
		virtual bool setsCipherPath() const = 0 ;
		virtual bool autoMountsOnCreate() const = 0 ;
		virtual bool hasGUICreateOptions() const = 0 ;
		virtual void GUICreateOptionsinstance( QWidget * parent,
						       std::function< void( const Options& ) > )  const = 0 ;
	private:
		QString m_name ;
	} ;

	engines() ;
	const engine& getByName( const engines::engine::options& e ) const ;
	const engine& getByName( const QString& e ) const ;
	const engine& getByFuseName( const QString& e ) const ;
private:
	std::vector< std::unique_ptr< engines::engine > > m_backends ;
};

#endif
