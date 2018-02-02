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
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QVector>
#include <QSystemTrayIcon>
#include <QAction>
#include <QIcon>
#include <QSettings>
#include <QRect>

#include <functional>
#include <memory>
#include <array>
#include <utility>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "task.hpp"
#include "lxqt_wallet.h"
#include "favorites.h"
#include "plugins.h"
#include "utility2.h"

#include <QObject>
#include <QLabel>

#include <fcntl.h>

#include <iostream>
class QByteArray ;
class QEvent ;

namespace utility
{
	using volumeList = std::vector< std::pair< favorites::entry,QByteArray > > ;
}

namespace utility
{
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
        uint64_t f_blocks ;
        uint64_t f_bavail ;
        uint64_t f_bsize ;
        uint64_t f_bfree ;
	};

	::Task::future< utility::fsInfo >& fileSystemInfo( const QString& ) ;

	bool platformIsLinux() ;
	bool platformIsOSX() ;
	bool platformIsWindows() ;

	void setParent( QWidget * parent,QWidget ** localParent,QDialog * dialog ) ;

	void setWindowOptions( QDialog * ) ;

	utility::walletBackEnd autoMountBackEnd( void ) ;

	void autoMountBackEnd( const utility::walletBackEnd& ) ;

	int startApplication( std::function< int() > ) ;

	bool printVersionOrHelpInfo( const QStringList& ) ;

	wallet getKey( const QString& keyID,LXQt::Wallet::Wallet&,QWidget * = nullptr ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	void runCommandOnMount( const QString& ) ;
	QString runCommandOnMount( void ) ;
	QString fileManager( void ) ;
	void setFileManager( const QString& ) ;

	QString readPassword( bool = true ) ;

	QIcon getIcon() ;

	QStringList directoryList( const QString& e ) ;

	QString executableFullPath( const QString& ) ;

	QString externalPluginExecutable() ;
	void setExternalPluginExecutable( const QString& ) ;

	void setWindowsMountPointOptions( QWidget *,QLineEdit *,QPushButton * ) ;

	bool reUseMountPoint( void ) ;
	void reUseMountPoint( bool ) ;

	QString homeConfigPath( const QString& = QString() ) ;
	QString homePath() ;
	QString mountPath( const QString& path ) ;
	QString mountPath() ;

	QString getExistingDirectory( QWidget *,const QString& caption,const QString& dir ) ;

	bool createFolder( const QString& ) ;

	void scaleGUI( void ) ;

	void setDefaultMountPointPrefix( const QString& path ) ;

	void setSettingsObject( QSettings * ) ;

	void preUnMountCommand( const QString& ) ;
	QString preUnMountCommand( void ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsReadable( const QString&,bool isFolder = true ) ;
	bool pathIsWritable( const QString&,bool isFolder = true ) ;

	bool autoOpenFolderOnMount() ;
	void autoOpenFolderOnMount( bool ) ;

	QString securefsPath() ;
	QString winFSPpath() ;
	int winFSPpollingInterval() ;

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

	bool startMinimized() ;
	void setStartMinimized( bool ) ;

	int checkForUpdateInterval( void ) ;

	bool enableRevealingPasswords( void ) ;

	void enableDebug( bool ) ;
	bool debugEnabled( void ) ;

	void enableFullDebug( bool ) ;
	bool debugFullEnabled( void ) ;

	enum class background_thread{ True,False } ;
	bool enablePolkit( utility::background_thread ) ;

	QProcessEnvironment systemEnvironment() ;

	int networkTimeOut() ;

	QString homePath() ;
	QString userName() ;

	QString configFilePath( QWidget *,const QString& ) ;

	QStringList split( const QString&,char = '\n' ) ;
	QStringList executableSearchPaths( void ) ;
	QString executableSearchPaths( const QString& ) ;

	void polkitFailedWarning( std::function< void() > ) ;
	bool useSiriPolkit( void ) ;
	void quitHelper() ;
	void initGlobals() ;
	QString helperSocketPath() ;
	void clearFavorites( void ) ;
	void addToFavorite( const QStringList& ) ;
	std::vector< favorites::entry > readFavorites( void ) ;
	favorites::entry readFavorite( const QString& ) ;
	void replaceFavorite( const favorites::entry&,const favorites::entry& ) ;
	void readFavorites( QMenu * ) ;
	void removeFavoriteEntry( const favorites::entry& ) ;
	int favoritesEntrySize() ;
	QString getVolumeID( const QString&,bool = false ) ;
	QString localizationLanguage() ;
	QString localizationLanguagePath() ;
	QString socketPath() ;
	void setLocalizationLanguage( const QString& ) ;
	QString walletName( void ) ;
	QString walletName( LXQt::Wallet::BackEnd ) ;
	QString applicationName( void ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

	void setLocalizationLanguage( bool translate,QMenu * m,utility2::translator& ) ;
	void languageMenu( QMenu *,QAction *,utility2::translator& ) ;

	class windowDimensions{
	public:
		static constexpr int size = 8 ;
		windowDimensions( const QStringList& e ) ;
		windowDimensions( const QString& e ) ;
		windowDimensions( const std::array< int,size >& e ) ;
		operator bool() ;
		int columnWidthAt( std::array< int,size >::size_type ) const ;
		QRect geometry() const ;
		QString dimensions() const ;
	private:
		void setDimensions( const QStringList& ) ;
		std::array< int,size > m_array ;
		bool m_ok = false ;
	};

	utility::windowDimensions getWindowDimensions() ;
	void setWindowDimensions( const utility::windowDimensions& ) ;

	::Task::future< bool >& openPath( const QString& path,const QString& opener ) ;

	void openPath( const QString& path,const QString& opener,QWidget *,const QString&,const QString& ) ;
}

namespace utility
{
	template< typename T >
	QByteArray fileContents( const T& e )
	{
		QFile file( e ) ;
		file.open( QIODevice::ReadOnly ) ;
		return file.readAll() ;
	}

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

		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QByteArray& password = QByteArray() )
		{
			return ::Task::run( [ = ](){

				return utility::Task( exe,-1,utility::systemEnvironment(),password ) ;
			} ) ;
		}

		static void exec( const QString& exe,
				  const QProcessEnvironment& env = utility::systemEnvironment(),
				  std::function< void() > f = [](){} )
		{
			::Task::exec( [ = ](){ utility::Task( exe,env,f ) ; } ) ;
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
		Task( const QString& exe,int waitTime = -1,const QProcessEnvironment& env = utility::systemEnvironment(),
		      const QByteArray& password = QByteArray(),std::function< void() > f = [](){},bool e = false )
		{
			this->execute( exe,waitTime,env,password,std::move( f ),e ) ;
		}
		Task( const QString& exe,const QProcessEnvironment& env,std::function< void() > f,bool e = false )
		{
			this->execute( exe,-1,env,QByteArray(),std::move( f ),e ) ;
		}

		enum class channel{ stdOut,stdError } ;
		QStringList splitOutput( char token,channel s = channel::stdOut ) const
		{
			if( s == channel::stdOut ){

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
	private:
		void execute( const QString& exe,int waitTime,const QProcessEnvironment& env,
			      const QByteArray& password,std::function< void() > f,bool e ) ;

		QByteArray m_stdOut ;
		QByteArray m_stdError ;
		int m_exitCode = 255 ;
		int m_exitStatus = 255 ;
		bool m_finished = false ;
	};
}

#endif // MISCFUNCTIONS_H
