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
#include <utility>

#include <QString>
#include <QStringList>
#include <QWidget>

#include "favorites.h"
#include "utility.h"

class engines
{
public:
	static QString executableFullPath( const QString& ) ;
	static QStringList executableSearchPaths() ;

	class engine
	{
	protected:
		class commandOptions ;
	public:
		class Wrapper{
		public:
			Wrapper( const engines::engine& e ) :
			        m_engine( std::addressof( e ) )
			{
			}
			Wrapper() :
			        m_engine( std::addressof( engines::instance().getUnKnown() ) )
			{
			}
			const engines::engine& get() const
			{
				return *m_engine ;
			}
			const engines::engine& get()
			{
				return *m_engine ;
			}
			const engines::engine * operator->() const
			{
				return m_engine ;
			}
			const engines::engine * operator->()
			{
				return m_engine ;
			}
		private:
			const engines::engine * m_engine ;
		};

		enum class error{ Success,Failed,Timeout,Continue } ;

		enum class status
		{
			success,
			volumeCreatedSuccessfully,

			backendRequiresPassword,

			cryfsBadPassword,
			encfsBadPassword,
			sshfsBadPassword,
			gocryptfsBadPassword,
			securefsBadPassword,
			ecryptfsBadPassword,
			fscryptBadPassword,

			sshfsNotFound,
			cryfsNotFound,
			encfsNotFound,
			fscryptNotFound,
			securefsNotFound,
			gocryptfsNotFound,
			ecryptfs_simpleNotFound,

			customCommandNotFound,
                        customCommandBadPassword,

			cryfsMigrateFileSystem,
			IllegalPath,

			fscryptPartialVolumeClose,
			failedToLoadWinfsp,
			backEndFailedToMeetMinimumRequirenment,
			failedToStartPolkit,
			failedToUnMount,
			backEndDoesNotSupportCustomConfigPath,
			failedToCreateMountPoint,
			invalidConfigFileName,
			backendFail,
			backendTimedOut,
			unknown
		} ;

		struct options
		{
			options( const favorites::entry& e,
				 const QByteArray& volumeKey = QByteArray() ) ;

			options( const QString& cipher_folder,
				 const QString& plain_folder,
				 const QByteArray& volume_key,
				 const QString& idle_timeout,
				 const QString& config_file_path,
				 bool unlock_in_read_only,
				 bool unlock_in_reverse_mode,
				 const QString& mount_options,
				 const QString& create_options ) ;
			QString cipherFolder ;
			QString plainFolder ;
			QByteArray key ;
			QString idleTimeout ;
			QString configFilePath ;
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
			cmdStatus( engines::engine::status s,
				   const engines::engine&,
				   const QString& e = QString() ) ;
			engines::engine::status status() const ;
			bool operator==( engines::engine::status s ) const ;
			bool operator!=( engines::engine::status s ) const ;
			QString toString() const ;
			QString toMiniString() const ;
			const engines::engine& engine() const ;
			bool success() const ;
		private:
			engines::engine::status m_status = engines::engine::status::backendFail ;
			QString m_message ;
			engines::engine::Wrapper m_engine ;
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

		struct BaseOptions
		{
			int  backendTimeout ;
			bool hasConfigFile ;
			bool setsCipherPath ;
			bool autoMountsOnCreate ;
			bool hasGUICreateOptions ;
			bool supportsMountPathsOnWindows ;
			bool requiresAPassword ;
			bool customBackend ;
			bool requiresPolkit ;
			bool autorefreshOnMountUnMount ;
			bool backendRequireMountPath ;
			bool takesTooLongToUnlock ;

			QByteArray passwordFormat ;
			QString minimumVersion ;
			QString reverseString ;
			QString idleString ;
			QString executableName ;
			QString incorrectPasswordText ;
			QString incorrectPassWordCode ;
			QString configFileArgument ;
			QString unMountCommand ;
			QString windowsUnMountCommand ;
			QString windowsInstallPathRegistryKey ;
			QString windowsInstallPathRegistryValue ;
			QStringList volumePropertiesCommands ;
			QStringList successfulMountedList ;
			QStringList failedToMountList ;
			QStringList names ;
			QStringList fuseNames ;
			QStringList configFileNames ;
			QStringList fileExtensions ;

			engines::engine::status notFoundCode ;
		} ;

		struct args
		{
			args() ;
			args( const cmdArgsList&,const engines::engine::commandOptions&,
			      const QString& cmd = QString() ) ;
			QString cmd ;
			QString cipherPath ;
			QString mountPath ;
			QString fuseOptions ;
			QString exeOptions ;
			QString mode ;
			QString subtype ;
		} ;

		QString executableFullPath() const ;

		bool isInstalled() const ;
		bool isNotInstalled() const ;
		bool unknown() const ;
		bool known() const ;
		bool setsCipherPath() const ;
		bool autoMountsOnCreate() const ;
		bool hasGUICreateOptions() const ;
		bool hasConfigFile() const ;
		bool supportsMountPathsOnWindows() const ;
		bool requiresAPassword() const ;
		bool customBackend() const ;
		bool autorefreshOnMountUnMount() const ;
		bool backendRequireMountPath() const ;

		bool versionIsLessOrEqualTo( const QString& ) const ;
		bool versionGreaterOrEqualTo( const QString& ) const ;

		engines::engine::status notFoundCode() const ;

		int backendTimeout() const ;
		const QStringList& names() const ;
		const QStringList& fuseNames() const ;
		const QStringList& configFileNames() const ;
		const QStringList& fileExtensions() const ;
		const QStringList& volumePropertiesCommands() const ;

		const QString& minimumVersion() const ;
		const QString& reverseString() const ;
		const QString& idleString() const ;
		const QString& executableName() const ;
		const QString& name() const ;
		const QString& configFileName() const ;
		const QString& incorrectPasswordText() const ;
		const QString& incorrectPasswordCode() const ;
		const QString& unMountCommand() const ;
		const QString& windowsUnMountCommand() const ;
		const QString& windowsInstallPathRegistryKey() const ;
		const QString& windowsInstallPathRegistryValue() const ;

		engine::engine::error errorCode( const QString& ) const ;

		QString setConfigFilePath( const QString& ) const ;
		QByteArray setPassword( const QByteArray& ) const ;

		virtual ~engine() ;

		virtual void updateVolumeList( const engines::engine::options& ) const ;

		virtual QStringList mountInfo( const QStringList& ) const ;

		virtual Task::future< QString >& volumeProperties( const QString& cipherFolder,
								   const QString& mountPoint ) const ;

		virtual engines::engine::status unmount( const QString& cipherFolder,
							 const QString& mountPoint,
							 int maxCount ) const ;

		virtual bool takesTooLongToUnlock() const ;

		virtual engine::engine::status passMinimumVersion() const ;

		virtual const engines::engine& proveEngine( const QString& cipherPath ) const ;

		virtual void updateOptions( engines::engine::options& ) const ;

		virtual const QProcessEnvironment& getProcessEnvironment() const ;
		virtual bool requiresPolkit() const ;
		virtual const QString& installedVersionString() const = 0 ;
		virtual args command( const QByteArray& password,const engines::engine::cmdArgsList& args ) const = 0 ;
		virtual engines::engine::status errorCode( const QString& e,int s ) const = 0 ;
		using function = std::function< void( const Options& ) > ;
		virtual void GUICreateOptionsinstance( QWidget * parent,function ) const = 0 ;
	protected:
		virtual QString sanitizeVersionString( const QString& ) const ;
		QString baseInstalledVersionString( const QString& versionArgument,
						    bool readFromStdOut,
                                                    int argumentNumber,
                                                    int argumentLine ) const ;
		class commandOptions{
		public:
			enum class separator{ space,equal_sign } ;

			class Options{
			public:
				Options( QString& m,QString s ) :
					m_options( m ),m_separator( std::move( s ) )
				{
				}
				bool doesNotContain( const QString& key ) const
				{
					return !this->contains( key ) ;
				}
				bool contains( const QString& key ) const
				{
					return m_options.contains( key ) ;
				}
				void addPair( const QString& key,const QString& value,separator s )
				{
					if( s == separator::space ){

						this->add( key + " " + value ) ;
					}else{
						this->add( key + "=" + value ) ;
					}
				}
				template< typename E >
				void add( E&& e )
				{
					while( m_options.endsWith( "\n" ) ){

						m_options = utility::removeLast( m_options,1 ) ;
					}
					if( m_options.isEmpty() ){

						m_options = std::forward< E >( e ) ;
					}else{
						m_options += m_separator + std::forward< E >( e ) ;
					}
				}
				template< typename E,typename ... T >
				void add( E&& e,T&& ... m )
				{
					this->add( std::forward< E >( e ) ) ;
					this->add( std::forward< T >( m ) ... ) ;
				}
				const QString& get() const
				{
					return m_options ;
				}
				QString extractStartsWith( const QString& e )
				{
					auto tmp = m_options ;
					m_options.clear() ;

					QString m ;

					auto s = m_separator[ 0 ].toLatin1() ;

					for( const auto& it : utility::split( tmp,s ) ){

						if( it.startsWith( e ) ){

							m = it ;
						}else{
							this->add( it ) ;
						}
					}

					return m ;
				}
			private:
				QString& m_options ;
				QString m_separator ;
			};

			class fuseOptions : public Options
			{
			public:
				fuseOptions( QString& m ) : Options( m,"," )
				{
				}
				void addPair( const QString& key,const QString& value,
					      separator s = separator::equal_sign )
				{
					Options::addPair( key,value,s ) ;
				}
			} ;

			class exeOptions : public Options
			{
			public:
				exeOptions( QString& m ) : Options( m," " )
				{
				}
				void addPair( const QString& key,const QString& value,
					      separator s = separator::space )
				{
					Options::addPair( key,value,s ) ;
				}
			} ;

			commandOptions( const engines::engine::cmdArgsList& e,
					const QString& f,
					const QString& g = QString() ) ;

			const QString& constExeOptions() const
			{
				return m_exeOptions ;
			}
			const QString& constFuseOpts() const
			{
				return m_fuseOptions ;
			}
			const QString& subType() const
			{
				return m_subtype ;
			}
			const QString& mode() const
			{
				return m_mode ;
			}
			exeOptions exeOptions()
			{
				return m_exeOptions ;
			}
			fuseOptions fuseOpts()
			{
				return m_fuseOptions ;
			}
		private:
			QString m_exeOptions ;
			QString m_fuseOptions ;
			QString m_subtype ;
			QString m_mode ;
		};

		engine( BaseOptions ) ;
	private:
		BaseOptions m_Options ;
		QProcessEnvironment m_processEnvironment ;
	} ;

	class version{
	public:
		version( std::function< QString() > function ) : m_function( std::move( function ) )
		{
		}
		version() : m_function( [](){ return QString() ; } )
		{
		}
		const QString& get() const
		{
			if( m_version.isEmpty() ){

				m_version = m_function() ;
			}

			return m_version ;
		}
	private:
		std::function< QString() > m_function ;
		mutable QString m_version ;
	};

	class exeFullPath{
	public:
		exeFullPath( const engines::engine& e ) : m_engine( e )
		{
		}
		const QString& get() const
		{
			if( m_path.isEmpty() ){

				m_path = m_engine->executableFullPath() ;
			}

			return m_path ;
		}
	private:
		const engines::engine::Wrapper m_engine ;
		mutable QString m_path ;
	};

	engines() ;
	static const engines& instance() ;
	bool atLeastOneDealsWithFiles() const ;
	QStringList mountInfo( const QStringList& ) const ;
	QStringList enginesWithNoConfigFile() const ;
	QStringList enginesWithConfigFile() const ;
	const std::vector< engines::engine::Wrapper >& supportedEngines() const ;
	const engine& getUnKnown() const ;
	const engine& getByName( const QString& e ) const ;
	const engine& getByFuseName( const QString& e ) const ;
	const engine& getByFileExtension( const QString& e ) const ;
	std::pair< const engines::engine&,QString >
	getByConfigFileNames( std::function< bool( const QString& ) > function ) const ;

private:
	std::vector< std::unique_ptr< engines::engine > > m_backends ;
	std::vector< engines::engine::Wrapper > m_backendWrappers ;
};

#endif
