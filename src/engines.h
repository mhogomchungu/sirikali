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

#include "volumeinfo.h"
#include "favorites.h"
#include "utility.h"

class engines
{
public:
	class engine ;

	static QString executableNotEngineFullPath( const QString& ) ;
	static QString executableFullPath( const QString& ) ;
	static QString executableFullPath( const QString&,const engines::engine& ) ;

	static QStringList executableSearchPaths() ;
	static QStringList executableSearchPaths( const engines::engine& ) ;

	class engineVersion{
	public:
		engineVersion() ;
		engineVersion( int major,int minor,int patch ) ;
		engineVersion( const QString& e ) ;
		bool valid() const ;
		bool operator==( const engineVersion& other ) const ;
		bool operator<( const engineVersion& other ) const ;
		/*
		 * a != b equal to !(a == b)
		 * a <= b equal to (a < b) || (a == b)
		 * a >= b equal to !(a < b)
		 * a > b  equal to !(a <= b)
		 */
		bool operator>=( const engineVersion& other ) const
		{
			return !( *this < other ) ;
		}
		bool operator<=( const engineVersion& other ) const
		{
			return ( *this < other ) || ( *this == other ) ;
		}
		bool operator!=( const engineVersion& other ) const
		{
			return !( *this == other ) ;
		}
		bool operator>( const engineVersion& other ) const
		{
			return !( *this <= other ) ;
		}
		QString toString() const ;
	private:
		bool m_valid = false ;
		int m_major = 0 ;
		int m_minor = 0 ;
		int m_patch = 0 ;
	};

	template< typename Type >
	class cache{
	public:
	        cache( std::function< Type() > function ) : m_function( std::move( function ) )
		{
		}
		cache() : m_function( [](){ return Type() ; } )
		{
		}
		const Type& get() const
		{
		        if( m_unset ){

			        m_unset = false ;

				m_variable = utility::unwrap( Task::run( [ this ]{ return m_function() ; } ) ) ;
			}

			return m_variable ;
		}
		virtual ~cache()
		{
		}
	private:
		std::function< Type() > m_function ;
		mutable Type m_variable ;
		mutable bool m_unset = true ;
	};

	class version : public cache< engines::engineVersion >{
	public:
	        enum class Operator{ less,lessOrEqual,equal,notEqual,greater,greaterOrEqual } ;

		version( const QString& e,std::function< engines::engineVersion() > s ) :
			cache( std::move( s ) ),
			m_engineName( e )
		{
		}
		version()
		{
		}
		utility::bool_result compare( const engines::engineVersion& b,
					      engines::version::Operator op ) const
		{
			const auto& a = this->get() ;

			if( a.valid() && b.valid() ){

			        switch( op ) {

				        case engines::version::Operator::less :           return a < b ;
				        case engines::version::Operator::lessOrEqual :    return a <= b ;
				        case engines::version::Operator::equal :          return a == b ;
				        case engines::version::Operator::notEqual :       return a != b ;
				        case engines::version::Operator::greater :        return a > b ;
				        case engines::version::Operator::greaterOrEqual : return a >= b ;
				}
			}

			return {} ;
		}
		QString toString() const
		{
			return this->get().toString() ;
		}
		utility::bool_result greaterOrEqual( int major,int minor,int patch ) const
		{
			return this->compare( { major,minor,patch },engines::version::Operator::greaterOrEqual ) ;
		}
		utility::bool_result greaterOrEqual( const QString& v ) const
		{
		        return this->compare( v,engines::version::Operator::greaterOrEqual ) ;
		}
		virtual void logError() const ;
	private:
		QString m_engineName ;
	};

	class booleanCache : public cache< utility::bool_result >{
	public:
		booleanCache( std::function< utility::bool_result() > s ) :
		        cache( std::move( s ) )
		{
		}
		operator bool() const
		{
		        return this->get().value() ;
		}
	private:
		virtual void silenceWarning() ;
	};

	class versionGreaterOrEqual : public cache< utility::bool_result >{
	public:
	        versionGreaterOrEqual( bool m,const engines::engine& engine,int major,int minor,int patch ) :
		        cache( [ this,&engine,m,major,minor,patch ](){ return this->setCallback( m,engine,major,minor,patch ) ; } )
		{
		}
		versionGreaterOrEqual( bool m,const engines::engine& engine,const QString& e ) :
		        cache( [ this,&engine,m,e ](){ return this->setCallback( m,engine,e ) ; } )
		{
		}
		operator bool() const
		{
		        return this->get().value() ;
		}
	private:
		bool setCallback( bool m,const engines::engine& engine,int major,int minor,int patch ) ;
		bool setCallback( bool m,const engines::engine& engine,const QString& ) ;
		virtual void silenceWarning() ;
	} ;

	class exeFullPath : public cache< QString >{
	public:
	        exeFullPath( std::function< QString() > s ) : cache( std::move( s ) )
		{
		}
	private:
		virtual void silenceWarning() ;
	};

	class engine
	{
	protected:
		class commandOptions ;
	public:
		struct unMount{

			const QString& cipherFolder ;
			const QString& mountPoint ;
			const engines::engine& engine ;
			int numberOfAttempts ;
		} ;

		struct exe_args{
		        exe_args( QString e,QStringList s ) :
			        exe( std::move( e ) ),args( std::move( s ) )
			{
			}
			exe_args()
			{
			}
			QString exe ;
			QStringList args ;
		};

		struct exe_args_const{
			exe_args_const( const exe_args& e ) : exe( e.exe ),args( e.args )
			{
			}
			exe_args_const( const QString& e,const QStringList& s ) :
				exe( e ),args( s )
			{
			}
			const QString& exe ;
			const QStringList& args ;
		};

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
			badPassword,

			ecryptfs_simpleNotFound,
			engineExecutableNotFound,
			javaNotFound,

			cryfsMigrateFileSystem,
			cryfsReplaceFileSystem,
			cryfsVersionTooOldToMigrateVolume,

			backEndDoesNotSupportCustomConfigPath,
			notSupportedMountPointFolderPath,
			mountPointFolderNotEmpty,
			IllegalPath,

		        backendFailedToMeetSiriKaliMinimumVersion,
			backEndFailedToMeetMinimumRequirenment,

			fscryptPartialVolumeClose,
			failedToLoadWinfsp,
			fscryptKeyFileRequired,

			failedToStartPolkit,
			failedToUnMount,
			failedToCreateMountPoint,

			invalidConfigFileName,
			backendFail,
			backendTimedOut,
			unknown
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

		struct booleanOptions{

			booleanOptions() = default ;

			booleanOptions( const booleanOptions& other ) = default ;

			booleanOptions( bool ro,bool rm,bool ar,bool au ) :
				unlockInReadOnly( ro ),
				unlockInReverseMode( rm ),
				allowReplacedFileSystem( ar ),
				allowUpgradeFileSystem( au )
			{
			}
			booleanOptions& operator=( const booleanOptions& other ) = default ;
			bool unlockInReadOnly = false ;
			bool unlockInReverseMode = false ;
			bool allowReplacedFileSystem = true ;
			bool allowUpgradeFileSystem = false ;
		} ;

		struct createGUIOptions{

			struct createOptions
			{
				createOptions( const QString& createOpts,
					       const QString& configFile,
					       const QString& keyFile,
					       const engines::engine::booleanOptions& r ) ;

				createOptions( const QString& createOpts,
					       const QString& configFile,
					       const QString& keyFile ) ;

				createOptions( const engines::engine::booleanOptions& r ) ;

				createOptions() ;

				QString idleTimeOut ;
				QString configFile ;
				QString keyFile ;
				QStringList createOpts ;
				engines::engine::booleanOptions opts ;
				bool success = true ;
			} ;

			createGUIOptions( QWidget * w,
					  const engines::engine::createGUIOptions::createOptions& s,
					  std::function< void( const createGUIOptions::createOptions& ) > f ) :
				parent( w ),
				cOpts( s ),
				fCreateOptions( std::move( f ) )
			{
			}
			QWidget * parent ;
			const engines::engine::createGUIOptions::createOptions& cOpts ;
			std::function< void( const createGUIOptions::createOptions& ) > fCreateOptions ;
		};

		struct mountGUIOptions{

			struct mountOptions{

				mountOptions( const favorites::entry& ) ;

				mountOptions( const QString& idleTimeOut,
					      const QString& configFile,
					      const QString& mountOpes,
					      const QString& keyFile,
				              const QString& identityFile,
				              const QString& identityAgent,
					      const engines::engine::booleanOptions& opts ) ;
				mountOptions() ;
				QString idleTimeOut ;
				QString configFile ;
				QString keyFile ;
				QString identityFile ;
				QString identityAgent ;
				QStringList mountOpts ;
				engines::engine::booleanOptions opts ;
				bool success = true ;
			} ;

			mountGUIOptions( QWidget * w,
					 bool c,
					 const engines::engine::mountGUIOptions::mountOptions& ll,
					 std::function< void( const mountGUIOptions::mountOptions& ) > f ) :
				parent( w ),
				create( c ),
				mOpts( ll ),
				fMountOptions( std::move( f ) )
			{
			}
			QWidget * parent ;
			bool create ;
			const engines::engine::mountGUIOptions::mountOptions& mOpts ;
			std::function< void( const mountGUIOptions::mountOptions& ) > fMountOptions ;
		} ;

		using mOpts = engines::engine::mountGUIOptions::mountOptions ;
		using cOpts = engines::engine::createGUIOptions::createOptions ;

		using fcreate = std::function< void( const engines::engine::cOpts& ) > ;
		using fmount = std::function< void( const engines::engine::mOpts& ) > ;

		struct cmdArgsList
		{
			cmdArgsList( const favorites::entry& e,
				     const QByteArray& volumeKey = QByteArray() ) ;

			cmdArgsList( const QString& cipher_folder,
				     const QString& plain_folder,
				     const QByteArray& volume_key,
				     const engines::engine::createGUIOptions::createOptions& ) ;

			cmdArgsList( const QString& cipher_folder,
				     const QString& plain_folder,
				     const QByteArray& volume_key,
				     const engines::engine::mountGUIOptions::mountOptions& ) ;

			QString cipherFolder ;
			QString mountPoint ;
			QByteArray key ;
			QString idleTimeout ;
			QString configFilePath ;
			QString identityFile ;
			QString identityAgent ;
			QStringList mountOptions ;
			QStringList createOptions ;
			QString keyFile ;
			QString fuseOptionsSeparator ;
			booleanOptions boolOptions ;
		} ;

		struct BaseOptions
		{
			struct vInfo{

				QString versionArgument ;
				bool readFromStdOut ;
				int argumentNumber ;
				int argumentLine ;
			};

			std::vector< vInfo > versionInfo ;
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
			bool backendRunsInBackGround ;
			bool acceptsSubType ;
			bool acceptsVolName ;
			bool likeSsh ;
			bool autoCreatesMountPoint ;
			bool autoDeletesMountPoint ;
			bool usesOnlyMountPoint ;
			bool usesFuseArgumentSwitch ;

			QByteArray passwordFormat ;
			QString displayName ;
			QString releaseURL ;
			QString versionMinimum ;
			QString sirikaliMinimumVersion ;
			QString reverseString ;
			QString idleString ;
			QString incorrectPasswordText ;
			QString incorrectPassWordCode ;
			QString configFileArgument ;
			QString keyFileArgument ;
			QString windowsInstallPathRegistryKey ;
			QString windowsInstallPathRegistryValue ;
			QString windowsExecutableFolderPath ;
			QString mountControlStructure ;
			QString createControlStructure ;
			QString defaultFavoritesMountOptions ;

			QStringList executableNames ;
			QStringList windowsUnMountCommand ;
			QStringList unMountCommand ;
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
			args( const cmdArgsList&,
			      const engines::engine::commandOptions&,
			      const QString& cmd,
			      const QStringList& ) ;
			QString cmd ;
			QString cipherPath ;
			QString mountPath ;
			QString mode ;
			QString subtype ;
			QStringList cmd_args ;
			QStringList fuseOptions ;
		} ;

		static QString encodeMountPath( const QString& e ) ;

		static void encodeSpecialCharacters( QString& ) ;
		static void decodeSpecialCharacters( QString& ) ;

		static QString decodeSpecialCharactersConst( const QString& ) ;

		static const QString& javaFullPath() ;
		static const QString& fuserMountPath() ;

		bool isInstalled() const ;
		bool isNotInstalled() const ;
		bool unknown() const ;
		bool known() const ;
		bool setsCipherPath() const ;
		bool autoMountsOnCreate() const ;
		bool hasGUICreateOptions() const ;
		bool hasConfigFile() const ;
		bool supportsMountPathsOnWindows() const ;
		bool customBackend() const ;
		bool autorefreshOnMountUnMount() const ;
		bool backendRequireMountPath() const ;
		bool runsInBackGround() const ;
		bool runsInForeGround() const ;
		bool acceptsSubType() const ;
		bool acceptsVolName() const ;
		bool likeSsh() const ;
		bool autoCreatesMountPoint() const ;
		bool autoDeletesMountPoint() const ;
		bool requiresAPassword() const ;
		bool requiresNoPassword() const ;
		bool usesOnlyMountPoint() const ;
		bool needsJava() const ;
		bool usesFuseArgumentSwitch() const ;

		engines::engine::status notFoundCode() const ;

		int backendTimeout() const ;

		const QStringList& names() const ;
		const QStringList& fuseNames() const ;
		const QStringList& configFileNames() const ;
		const QStringList& fileExtensions() const ;
		const QStringList& volumePropertiesCommands() const ;

		const engines::version& installedVersion() const ;

		const QString& defaultFavoritesMountOptions() const ;

		const QString& executableFullPath() const ;

		const QString& minimumVersion() const ;
		const QString& sirikaliMinimumVersion() const ;
		const QString& reverseString() const ;
		const QString& idleString() const ;
		const QString& releaseURL() const ;
		const QString& executableName() const ;
		const QString& name() const ;
		const QString& displayName() const ;
		const QString& uiName() const ;
		const QString& configFileName() const ;
		const QString& keyFileArgument() const ;
		const QString& mountControlStructure() const ;
		const QString& createControlStructure() const ;
		const QString& incorrectPasswordText() const ;
		const QString& incorrectPasswordCode() const ;
		const QString& configFileArgument() const ;
		const QString& windowsInstallPathRegistryKey() const ;
		const QString& windowsInstallPathRegistryValue() const ;
		const QString& windowsExecutableFolderPath() const ;

		QByteArray setPassword( const QByteArray& ) const ;

		engine( BaseOptions ) ;

		virtual ~engine() ;

		class terminate_process{
		public:
		        terminate_process( QProcess& exe,const QString& mountPath ) :
				m_exe( &exe ),m_mountPath( mountPath )
			{
			}
			terminate_process( const QString& mountPath ) :
				m_exe( nullptr ),m_mountPath( mountPath )
			{
			}
			qint64 PID() const
			{
				if( m_exe ){

					return m_exe->processId() ;
				}else{
					utility::debug() << "Warning, accessing invalid entry in terminate_process::PID" ;
					return 0 ;
				}
			}
			const QString& mountPath() const
			{
				 return m_mountPath ;
			}
			void terminate() const
			{
				if( m_exe ){

					m_exe->terminate() ;
				}else{
					utility::debug() << "Warning, accessing invalid entry in terminate_process::terminate" ;
				}
			}
			bool waitForFinished() const
			{
				if( m_exe ){

					return utility::waitForFinished( *m_exe ) ;
				}else{
					utility::debug() << "Warning, accessing invalid entry in terminate_process::waitForFinished" ;
					return false ;
				}
			}
		private:
			QProcess * m_exe ;
			const QString& m_mountPath ;
		};

		struct terminate_result{

			Task::process::result result ;
			QString exe ;
			QStringList args ;
		} ;

		engines::engine::exe_args unMountCommand( const engines::engine::terminate_process& e ) const ;

		virtual terminate_result terminateProcess( const terminate_process& ) const ;

		virtual terminate_result terminateProcess( const QString& mountPath ) const ;

		virtual engine::engine::error errorCode( const QString& ) const ;

		virtual void updateVolumeList( const engines::engine::cmdArgsList& ) const ;

		virtual volumeInfo::List mountInfo( const volumeInfo::List& ) const ;

		virtual Task::future< QString >& volumeProperties( const QString& cipherFolder,
								   const QString& mountPoint ) const ;

		virtual engines::engine::status unmount( const engines::engine::unMount& ) const ;

		virtual bool requiresAPassword( const engines::engine::cmdArgsList& ) const ;

		virtual bool takesTooLongToUnlock() const ;

		virtual engine::engine::status passAllRequirenments( const engines::engine::cmdArgsList& ) const ;

		struct ownsCipherFolder{
			bool yes ;
			QString cipherPath ;
			QString configPath ;
		} ;

		virtual ownsCipherFolder ownsCipherPath( const QString& cipherPath,
		                                         const QString& configPath ) const ;

		virtual void updateOptions( engines::engine::commandOptions& opts,
					    const engines::engine::cmdArgsList& args,
					    bool creating ) const ;

		virtual void updateOptions( engines::engine::cmdArgsList&,bool creating ) const ;

		virtual void updateOptions( QStringList&,
					    const engines::engine::cmdArgsList&,
					    bool creating ) const ;

		virtual const QProcessEnvironment& getProcessEnvironment() const ;


		virtual utility2::LOGLEVEL allowLogging( const QStringList& ) const ;

		virtual bool requiresPolkit() const ;

		virtual bool createMountPath( const QString& ) const ;

		virtual bool createCipherPath( const QString& ) const ;

		virtual bool deleteFolder( const QString&,int = 1 ) const ;

		virtual args command( const QByteArray& password,
				      const engines::engine::cmdArgsList& args,
				      bool create ) const ;

		virtual engines::engine::status errorCode( const QString& e,int s ) const ;

		virtual void GUICreateOptions( const createGUIOptions& ) const ;

		virtual void GUIMountOptions( const mountGUIOptions& ) const ;
	protected:
		bool unmountVolume( const engine::engine::exe_args_const& exe,bool usePolkit ) const ;

		class commandOptions{
		public:
			class fuseOptions ;

			class Options{
			public:
				Options( QStringList& m ) : m_options( m )
				{
				}
				template< typename E >
				void add( E&& e )
				{
					m_options.append( e ) ;
				}
				template< typename E,typename ... T >
				void add( E&& e,T&& ... m )
				{
					this->add( std::forward< E >( e ) ) ;
					this->add( std::forward< T >( m ) ... ) ;
				}
				template< typename E >
				void addFront( E&& e )
				{
					m_options.insert( 0,std::forward< E >( e ) ) ;
				}
				bool doesNotContain( const QString& key ) const
				{
					return !this->contains( key ) ;
				}
				bool contains( const QString& key ) const
				{
					for( const auto& it : m_options ){

						if( it.contains( key,Qt::CaseInsensitive ) ){

							return true ;
						}
					}

					return false ;
				}
				const QStringList& get() const
				{
					return m_options ;
				}
				utility2::result< QString > extractStartsWith( const QString& e )
				{
					for( int i = 0 ; i < m_options.size() ; i++ ){

						if( m_options[ i ].startsWith( e ) ){

							return m_options.takeAt( i ) ;
						}
					}

					return {} ;
				}
				utility2::result_ref< QString& > optionStartsWith( const QString& e )
				{
					for( int i = 0 ; i < m_options.size() ; i++ ){

						if( m_options[ i ].startsWith( e ) ){

							return m_options[ i ] ;
						}
					}

					return {} ;
				}
			private:
				QStringList& m_options ;
			};

			class fuseOptions : public Options
			{
			public:
				fuseOptions( QStringList& m ) : Options( m )
				{
				}
			} ;

			class exeOptions : public Options
			{
			public:
				exeOptions( QStringList& m ) : Options( m )
				{
				}
			} ;

			commandOptions() ;
			commandOptions( bool creating,
			                const engines::engine& engine,
			                const engines::engine::cmdArgsList& e ) ;

			const QStringList& constExeOptions() const
			{
				return m_exeOptions ;
			}
			const QStringList& constFuseOpts() const
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
			QStringList m_exeOptions ;
			QStringList m_fuseOptions ;
			QString m_subtype ;
			QString m_mode ;
		};
	private:
		const BaseOptions m_Options ;
		const QProcessEnvironment m_processEnvironment ;
		const engines::exeFullPath m_exeFullPath ;
		const engines::version m_version ;
		static engines::exeFullPath m_exeJavaFullPath ;
		static engines::exeFullPath m_exeFuserMount ;
	} ;

	engines() ;
	static const engines& instance() ;
	bool atLeastOneDealsWithFiles() const ;
	volumeInfo::List mountInfo( const volumeInfo::List& ) const ;
	QStringList enginesWithNoConfigFile() const ;
	QStringList enginesWithConfigFile() const ;
	const std::vector< engines::engine::Wrapper >& supportedEngines() const ;

	class engineWithPaths{
	public:
	        engineWithPaths() ;
		engineWithPaths( const QString& engine ) ;
		engineWithPaths( const QString& cipherPath,const QString& configFilePath ) ;
		engineWithPaths( const engines::engine&,engines::engine::ownsCipherFolder ) ;
		engineWithPaths( const engines::engine& e,
		                 const QString& cipherPath,
		                 const QString& configFilePath ) ;
		const engines::engine& get() const
		{
		        return m_engine.get() ;
		}
		const engines::engine * operator->() const
		{
		        return m_engine.operator->() ;
		}
		const QString& configFilePath() const
		{
		        return m_configPath ;
		}
		const QString& cipherFolder() const
		{
		        return m_cipherPath ;
		}
	private:
		engines::engine::Wrapper m_engine ;
		QString m_cipherPath ;
		QString m_configPath ;
	} ;

	engines::engineWithPaths getByPaths( const QString& cipherPath,
	                                     const QString& configPath = QString() ) const ;
	const engine& getUnKnown() const ;
	const engine& getByName( const QString& e ) const ;
	const engine& getByFsName( const QString& e ) const ;
private:
	std::vector< std::unique_ptr< engines::engine > > m_backends ;
	std::vector< engines::engine::Wrapper > m_backendWrappers ;
};

#endif
