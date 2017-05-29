/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <QString>
#include <QStringList>
#include <QEvent>
#include <QProcess>
#include <QThreadPool>
#include <QRunnable>
#include <QMetaObject>
#include <QDebug>
#include <QWidget>
#include <QDialog>
#include <QEventLoop>
#include <QTimer>
#include <QMenu>
#include <QVector>
#include <QSystemTrayIcon>
#include <QAction>
#include <QIcon>
#include <QSettings>
#include <functional>
#include <memory>
#include <array>
#include <utility>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "task.h"
#include "lxqt_wallet.h"
#include "favorites.h"
#include "plugins.h"
#include "translator.h"

#include <QObject>
#include <QLabel>

#include <poll.h>
#include <fcntl.h>

#include <iostream>

class QByteArray ;
class QEvent ;

namespace utility
{
	static inline int getUID()
	{
		return getuid() ;
	}

	QString homePath() ;

	template< typename T >
	void changePathOwner( const T& f )
	{
		int uid = utility::getUID() ;
		int fd = f.handle() ;

		if( uid != -1 && fd != -1 ){

			if( fchown( fd,uid,uid ) ){;}
		}
	}

	static inline void changePathOwner( const char * path )
	{
		int uid = utility::getUID() ;

		if( uid != -1 ){

			if( chown( path,uid,uid ) ){;}
		}
	}

	static inline void changePathOwner( const QString& path )
	{
		utility::changePathOwner( path.toLatin1().constData() ) ;
	}

	template< typename T >
	void changePathPermissions( const T& f,int mode = 0666 )
	{
		if( fchmod( f.handle(),mode ) ){;}
	}

	static inline void changePathPermissions( const QString& f,int mode = 0666 )
	{
		if( chmod( f.toLatin1().constData(),mode ) ){;}
	}

	class debug
	{
	public:
		debug( bool s = true ) : m_stdout( s )
		{
		}

		template< typename T >
		utility::debug operator<<( const T& e )
		{
			if( m_stdout ){

				std::cout << e << std::endl ;
			}else{
				std::cerr << e << std::endl ;
			}

			return utility::debug( m_stdout ) ;
		}

		utility::debug operator<<( const QByteArray& e )
		{
			if( m_stdout ){

				std::cout << e.constData() << std::endl ;
			}else{
				std::cerr << e.constData() << std::endl ;
			}

			return utility::debug( m_stdout ) ;
		}

		utility::debug operator<<( const QString& e )
		{
			if( m_stdout ){

				std::cout << e.toLatin1().constData() << std::endl ;
			}else{
				std::cerr << e.toLatin1().constData() << std::endl ;
			}

			return utility::debug( m_stdout ) ;
		}

		utility::debug operator<<( const QStringList& e )
		{
			if( m_stdout ){

				for( const auto& it : e ){

					std::cout << it.toLatin1().constData() << std::endl ;
				}
			}else{
				for( const auto& it : e ){

					std::cerr << it.toLatin1().constData() << std::endl ;
				}
			}

			return utility::debug( m_stdout ) ;
		}
	private:
		bool m_stdout ;
	};
}

namespace utility
{
	struct wallet
	{
		bool opened ;
		bool notConfigured ;
		QString key ;
	};

	class walletBackEnd
	{
	public:
		walletBackEnd( LXQt::Wallet::BackEnd s ) : m_valid( true ),m_storage( s )
		{
		}
		walletBackEnd() : m_valid( false )
		{
		}
		bool operator==( LXQt::Wallet::BackEnd s ) const
		{
			return m_valid && m_storage == s ;
		}
		bool operator==( const utility::walletBackEnd& other ) const
		{
			if( this->m_valid && other.m_valid ){

				return this->m_storage == other.m_storage ;
			}else{
				return false ;
			}
		}
		bool isValid() const
		{
			return m_valid ;
		}
		bool isInvalid() const
		{
			return !this->isValid() ;
		}
		LXQt::Wallet::BackEnd bk() const
		{
			return m_storage ;
		}
	private:
		bool m_valid ;
		LXQt::Wallet::BackEnd m_storage ;
	};

	struct fsInfo
	{
		bool valid ;
		u_int64_t f_blocks ;
		u_int64_t f_bavail ;
		u_int64_t f_bsize ;
		u_int64_t f_bfree ;
	};

	Task::future< utility::fsInfo >& fileSystemInfo( const QString& ) ;

	bool platformIsLinux() ;
	bool platformIsOSX() ;

	void setParent( QWidget * parent,QWidget ** localParent,QDialog * dialog ) ;

	void setWindowOptions( QDialog * ) ;

	utility::walletBackEnd autoMountBackEnd( void ) ;

	void autoMountBackEnd( const utility::walletBackEnd& ) ;

	int startApplication( const char * appName,std::function<int()> ) ;

	wallet getKey( const QString& keyID,LXQt::Wallet::Wallet& ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	QString runCommandOnMount( void ) ;
	QString fileManager( void ) ;
	void setFileManager( const QString& ) ;

	QString readPassword( bool = true ) ;

	QIcon getIcon() ;

	QStringList directoryList( const QString& e ) ;

	QString executableFullPath( const QString& ) ;

	QString externalPluginExecutable() ;
	void setExternalPluginExecutable( const QString& ) ;

	bool reUseMountPoint( void ) ;
	void reUseMountPoint( bool ) ;

	bool enablePolkitSupport( void ) ;
	void enablePolkitSupport( bool ) ;

	QString homeConfigPath( const QString& = QString() ) ;
	QString homePath() ;
	QString mountPath( const QString& path ) ;

	bool createFolder( const QString& ) ;

	void scaleGUI( void ) ;

	void setDefaultMountPointPrefix( const QString& path ) ;

	void setSettingsObject( QSettings * ) ;
	void setTranlatorObject( translator * s ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsReadable( const QString&,bool isFolder = true ) ;
	bool pathIsWritable( const QString&,bool isFolder = true ) ;

	bool autoOpenFolderOnMount() ;
	void autoOpenFolderOnMount( bool ) ;

	bool autoCheck() ;
	void autoCheck( bool ) ;

	bool readOnlyWarning() ;
	void readOnlyWarning( bool ) ;

	bool doNotShowReadOnlyWarning() ;
	void doNotShowReadOnlyWarning( bool ) ;

	bool autoMountFavoritesOnStartUp() ;
	void autoMountFavoritesOnStartUp( bool ) ;

	bool showMountDialogWhenAutoMounting() ;
	void showMountDialogWhenAutoMounting( bool ) ;

	bool setOpenVolumeReadOnly( QWidget * parent,bool check ) ;
	bool getOpenVolumeReadOnlyOption() ;

	bool autoMountFavoritesOnAvailable() ;
	void autoMountFavoritesOnAvailable( bool ) ;

	int checkForUpdateInterval( void ) ;

	bool enableRevealingPasswords( void ) ;

	bool runningInMixedMode() ;
	bool notRunningInMixedMode() ;

	QProcessEnvironment systemEnvironment() ;

	QString homePath() ;
	QString userName() ;

	QStringList split( const QString&,char = '\n' ) ;
	QStringList executableSearchPaths( void ) ;
	QString executableSearchPaths( const QString& ) ;

	bool useZuluPolkit( void ) ;
	void startHelperExecutable( QWidget *,const QString&,const char *,const char * ) ;
	void quitHelper() ;
	QString helperSocketPath() ;
	void clearFavorites( void ) ;
	void addToFavorite( const QStringList& ) ;
	QVector< favorites::entry > readFavorites( void ) ;
	void replaceFavorite( const favorites::entry&,const favorites::entry& ) ;
	void readFavorites( QMenu *,bool,const QString&,const QString& ) ;
	void removeFavoriteEntry( const favorites::entry& ) ;
	int favoritesEntrySize() ;
	QString getVolumeID( const QString&,bool = false ) ;
	QString localizationLanguage() ;
	QString localizationLanguagePath() ;
	void setLocalizationLanguage( const QString& ) ;
	QString walletName( void ) ;
	QString walletName( LXQt::Wallet::BackEnd ) ;
	QString applicationName( void ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

	void setLocalizationLanguage( bool translate,QMenu * m,translator& ) ;
	void languageMenu( QMenu *,QAction *,translator& ) ;

	using array_t = std::array< int,8 > ;

	utility::array_t getWindowDimensions() ;
	void setWindowDimensions( const std::initializer_list<int>& ) ;

	int pluginKey( QWidget *,QDialog *,QByteArray *,plugins::plugin ) ;

	QFont getFont( QWidget * ) ;
	void saveFont( const QFont& ) ;

	::Task::future< bool >& openPath( const QString& path,const QString& opener ) ;

	void openPath( const QString& path,const QString& opener,QWidget *,const QString&,const QString& ) ;
}

namespace utility
{
	bool pathExists( const QString& ) ;

	template< typename ... F >
	bool atLeastOnePathExists( const F& ... f ){

		for( const auto& it : { f ... } ){

			if( utility::pathExists( it ) ){

				return true ;
			}
		}

		return false ;
	}

	template< typename E,typename ... F >
	bool containsAtleastOne( const E& e,const F& ... f )
	{
		for( const auto& it : { f ... } ){

			if( e.contains( it ) ){

				return true ;
			}
		}

		return false ;
	}

	template< typename E,typename ... F >
	bool startsWithAtLeastOne( const E& e,const F& ... f )
	{
		for( const auto& it : { f ... } ){

			if( e.startsWith( it ) ){

				return true ;
			}
		}

		return false ;
	}

        template< typename E,typename ... F >
        bool endsWithAtLeastOne( const E& e,const F& ... f )
        {
                for( const auto& it : { f ... } ){

                        if( e.endsWith( it ) ){

                                return true ;
                        }
                }

                return false ;
        }

	template< typename E,typename ... F >
	bool equalsAtleastOne( const E& e,const F& ... f )
	{
		for( const auto& it : { f ... } ){

			if( e == it ){

				return true ;
			}
		}

		return false ;
	}
}

namespace utility
{
	class Task
	{
	public :
		static ::Task::future< utility::Task >& run( const QString& exe,bool e ) ;

		static ::Task::future< utility::Task >& run( const QString& exe,int,bool e ) ;

		static ::Task::future< utility::Task >& run( const QString& exe )
		{
			return ::Task::run< utility::Task >( [ exe ](){ return utility::Task( exe ) ; } ) ;
		}
		static void exec( const QString& exe,
				  const QProcessEnvironment& env = QProcessEnvironment(),
				  std::function< void() > f = [](){} )
		{
			::Task::run< utility::Task >( [ = ](){ return utility::Task( exe,env,f ) ; } ).start() ;
		}
		static void wait( int s )
		{
			sleep( s ) ;
		}
		static void waitForOneSecond( void )
		{
			sleep( 1 ) ;
		}
		static void waitForTwoSeconds( void )
		{
			sleep( 2 ) ;
		}
		static void suspendForOneSecond( void )
		{
			utility::Task::suspend( 1 ) ;
		}
		static void suspend( int s )
		{
			QTimer t ;

			QEventLoop l ;

			QObject::connect( &t,SIGNAL( timeout() ),&l,SLOT( quit() ) ) ;

			t.start( 1000 * s ) ;

			l.exec() ;
		}
		static QString makePath( QString e )
		{
			e.replace( "\"","\"\"\"" ) ;

			return "\"" + e + "\"" ;
		}
		Task()
		{
		}
		Task( const QString& exe,int waitTime = -1,const QProcessEnvironment& env = QProcessEnvironment(),
		      const QByteArray& password = QByteArray(),std::function< void() > f = [](){},bool e = true )
		{
			this->execute( exe,waitTime,env,password,std::move( f ),e ) ;
		}
		Task( const QString& exe,const QProcessEnvironment& env,std::function< void() > f,bool e = true )
		{
			this->execute( exe,-1,env,QByteArray(),std::move( f ),e ) ;
		}
		QStringList splitOutput( char token,bool stdOutput = true ) const
		{
			if( stdOutput ){

				return utility::split( m_stdOut,token ) ;
			}else{
				return utility::split( m_stdError,token ) ;
			}
		}
		void stdOut( const QByteArray& r )
		{
			m_stdOut = r ;
		}
		void stdError( const QByteArray& r )
		{
			m_stdError = r ;
		}
		const QByteArray& stdOut() const
		{
			return m_stdOut ;
		}
		const QByteArray& stdError() const
		{
			return m_stdError ;
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		int exitStatus() const
		{
			return m_exitStatus ;
		}
		bool success() const
		{
			return m_exitCode == 0 && m_exitStatus == QProcess::NormalExit && m_finished == true ;
		}
		bool failed() const
		{
			return !this->success() ;
		}
		bool finished() const
		{
			return m_finished ;
		}
		bool ok() const
		{
			return this->splitOutput( '\n' ).size() > 12 ;
		}
	private:
		void execute( const QString& exe,int waitTime,const QProcessEnvironment& env,
			      const QByteArray& password,const std::function< void() >& f,bool e ) ;

		QByteArray m_stdOut ;
		QByteArray m_stdError ;
		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};
}

#endif // MISCFUNCTIONS_H
