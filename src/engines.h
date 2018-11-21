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
			cmdStatus( engines::engine::status s,int c,const QString& e = QString() ) ;
			cmdStatus( engines::engine::status s,const QString& e = QString() ) ;
			cmdStatus( int s,const QString& e = QString() ) ;
			engines::engine::status status() const ;
			bool operator==( engines::engine::status s ) const ;
			bool operator!=( engines::engine::status s ) const ;
			QString toString() const ;
			QString toMiniString() const ;
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

		QString executableFullPath() const ;

		bool isInstalled() const ;
		bool isNotInstalled() const ;
		bool unknown() const ;
		bool known() const ;
		bool setsCipherPath() const ;
		bool autoMountsOnCreate() const ;
		bool hasGUICreateOptions() const ;

		engines::engine::status notFoundCode() const ;

		const QStringList& names() const ;
		const QStringList& fuseNames() const ;
		const QStringList& configFileNames() const ;

		const QString& name() const ;
		const QString& configFileName() const ;

		QString setConfigFilePath( const QString& ) const;

		virtual ~engine() ;

		virtual QString setPassword( const QString& ) const = 0 ;
		virtual QString command( const engines::engine::cmdArgsList& args ) const = 0 ;
		virtual engines::engine::status errorCode( const QString& e,int s ) const = 0 ;

		using function = std::function< void( const Options& ) > ;
		virtual void GUICreateOptionsinstance( QWidget * parent,function )  const = 0 ;
	protected:
		struct BaseOptions
		{
			bool setsCipherPath ;
			bool autoMountsOnCreate ;
			bool hasGUICreateOptions ;
			QString configFileArgument ;
			QStringList names ;
			QStringList fuseNames ;
			QStringList configFileNames ;

			engines::engine::status notFoundCode ;
		} ;

		engine( BaseOptions ) ;
	private:
		BaseOptions m_Options ;
	} ;

	engines() ;
	const engine& getByName( const engines::engine::options& e ) const ;
	const engine& getByName( const QString& e ) const ;
	const engine& getByFuseName( const QString& e ) const ;
private:
	std::vector< std::unique_ptr< engines::engine > > m_backends ;
};

#endif
