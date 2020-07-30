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
#include <QTextEdit>
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
#include "plugins.h"
#include "secrets.h"
#include "utility2.h"
#include "debugwindow.h"

#include <QObject>
#include <QLabel>

#include <fcntl.h>

#include <iostream>
class QByteArray ;
class QEvent ;

namespace utility
{
	using qstringlist_result = utility2::result< QStringList > ;
	using qbytearray_result  = utility2::result< QByteArray > ;
	using qstring_result     = utility2::result< QString > ;
	using bool_result        = utility2::result< bool > ;
	using int_result         = utility2::result< int > ;

	struct debug
	{
		static void showDebugWindow( const QString& ) ;

		static void logErrorWhileStarting( const QString& ) ;

		struct cout{

			template< typename T >
			cout operator<<( T x ){

				std::cout << x << std::endl ;

				return cout() ;
			}
			cout operator<<( const QString& x ){

				std::cout << x.toStdString() << std::endl ;

				return cout() ;
			}
			cout operator<<( const QByteArray& x ){

				std::cout << x.constData() << std::endl ;

				return cout() ;
			}
			cout operator<<( const QStringList& x ){

				for( const auto& it : x ){

					std::cout << it.toStdString() << std::endl ;
				}

				return cout() ;
			}
		};

		struct cerr{

			template< typename T >
			cerr operator<<( T x ){

				std::cerr << x << std::endl ;

				return cerr() ;
			}
			cerr operator<<( const QString& x ){

				std::cerr << x.toStdString() << std::endl ;

				return cerr() ;
			}
			cerr operator<<( const QByteArray& x ){

				std::cerr << x.constData() << std::endl ;

				return cerr() ;
			}
			cerr operator<<( const QStringList& x ){

				for( const auto& it : x ){

					std::cerr << it.toStdString() << std::endl ;
				}

				return cerr() ;
			}
		};

		utility::debug operator<<( int ) ;
		utility::debug operator<<( const char * ) ;
		utility::debug operator<<( const QByteArray& ) ;
		utility::debug operator<<( const QString& ) ;
		utility::debug operator<<( const QStringList& ) ;
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
	bool platformIsNOTWindows() ;

	void setWindowOptions( QDialog * ) ;

	int startApplication( std::function< int() > ) ;

	bool printVersionOrHelpInfo( const QStringList& ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	QByteArray readPassword( bool = true ) ;

	QByteArray convertPassword( const QString& ) ;
	QString convertPassword( const QByteArray& ) ;

	enum class iconType{ trayIcon,general } ;

	QIcon getIcon( iconType ) ;

	QStringList directoryList( const QString& e ) ;

	QString freeWindowsDriveLetter() ;
	bool isDriveLetter( const QString& ) ;
	bool startsWithDriveLetter( const QString& ) ;

	void setWindowsMountPointOptions( QWidget *,QTextEdit *,QPushButton * ) ;

	void setWindowsMountPointOptions( QWidget *,QLineEdit *,QPushButton * ) ;

	QString homeConfigPath( const QString& = QString() ) ;

	QString getExistingFile( QWidget *,const QString& caption,const QString& dir ) ;

	QString getExistingDirectory( QWidget *,const QString& caption,const QString& dir ) ;

	bool enablePolkit( void ) ;

	bool createFolder( const QString& ) ;
	bool removeFolder( const QString&,int attempts = 1 ) ;
	bool folderIsEmpty( const QString& ) ;
	bool folderNotEmpty( const QString& ) ;

	void scaleGUI( void ) ;

	void setGUIThread( void ) ;

	bool runningOnGUIThread( void ) ;

	bool runningOnBackGroundThread( void ) ;

	void runInUiThread( std::function< void() > function ) ;

	void waitForOneSecond( void ) ;

	void wait( int ) ;

	bool waitForFinished( QProcess&,int timeOut = 5 ) ;

	void setMainQWidget( QWidget * ) ;
	QWidget * mainQWidget() ;

	template< typename T >
	static inline auto unwrap( Task::future< T >& x )
	{
		if( utility::runningOnGUIThread() ){

			return x.await() ;
		}else{
			return x.get() ;
		}
	}

	class hideQWidget{
	public:
		hideQWidget( QWidget * w ) : m_widget( w )
		{
		}
		void hide()
		{
			m_widget->hide() ;
		}
		void show()
		{
			m_widget->show() ;
		}
		~hideQWidget()
		{
			this->show() ;
		}
	private:
		QWidget * m_widget ;
	};

	static inline void Timer( int interval,std::function< bool( int ) > function )
	{
		class Timer{
		public:
			Timer( int interval,std::function< bool( int ) > function ) :
				m_function( std::move( function ) )
			{
				auto timer = new QTimer() ;

				QObject::connect( timer,&QTimer::timeout,[ timer,this ](){

					m_counter++ ;

					if( m_function( m_counter ) ){

						timer->stop() ;

						timer->deleteLater() ;

						delete this ;
					}
				} ) ;

				timer->start( interval ) ;
			}
		private:
			int m_counter = 0 ;
			std::function< bool( int ) > m_function ;
		} ;

		new Timer( interval,std::move( function ) ) ;
	}

	void setDefaultMountPointPrefix( const QString& path ) ;

	qbytearray_result yubiKey( const QByteArray& challenge ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsFile( const QString& ) ;
	bool pathIsReadable( const QString&,bool isFolder = true ) ;
	bool pathIsWritable( const QString&,bool isFolder = true ) ;

	QString securefsPath() ;
	QString winFSPpath() ;

	void enableDebug( bool ) ;
	bool debugEnabled( void ) ;

	const QProcessEnvironment& systemEnvironment() ;

	QString userName() ;

	QString configFilePath( QWidget *,const QString& ) ;

	QString policyString() ;
	QString commentString() ;

	QStringList split( const QString&,const QString& ) ;
	QStringList split( const QString&,char = '\n' ) ;
	QStringList executableSearchPaths( void ) ;
	QString executableSearchPaths( const QString& ) ;

	QString removeLast( const QString&,int lastChars ) ;
	QString removeFirstAndLast( const QString&,int firstChars,int lastChars ) ;
	QString removeLastPathComponent( const QString& path,char separator = '/' ) ;

	void logCommandOutPut( const ::Task::process::result&,const QString&,const QStringList& ) ;
	void logCommandOutPut( const QString& ) ;

	std::function< void( const QString& ) > jsonLogger() ;

	void setDebugWindow( debugWindow * ) ;
	void polkitFailedWarning( std::function< void() > ) ;
	bool useSiriPolkit( void ) ;
	void quitHelper() ;
	void initGlobals() ;
	QString helperSocketPath() ;

	QString getVolumeID( const QString&,bool = false ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

	void applicationStarted() ;
	bool earlyBoot() ;

	QString removeOption( const QStringList&,const QString& option ) ;
	QString removeOption( const QString& commaSeparatedString,const QString& option ) ;

	template< typename E,typename F,typename ... G >
	QString removeOption( const E& e,const F& f,G&& ... g )
	{
		return removeOption( removeOption( e,f ),std::forward< G >( g ) ... ) ;
	}

	struct SocketPaths{
		QString folderPath ;
		QString socketFullPath ;
	} ;

	SocketPaths socketPath() ;

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
	bool pathNotExists( const QString& ) ;

	template< typename ... F >
	bool atLeastOnePathExists( const F& ... f ){

		for( const auto& it : { f ... } ){

			if( utility::pathExists( it ) ){

				return true ;
			}
		}

		return false ;
	}

	template< typename E >
	bool containsAtleastOne( const E& e )
	{
		Q_UNUSED( e )
		return false ;
	}

	template< typename E,typename F,typename ... G >
	bool containsAtleastOne( const E& e,F&& f,G&& ... g )
	{
		if( e.contains( f ) ){

			return true ;
		}else{
			return containsAtleastOne( e,std::forward< G >( g ) ... ) ;
		}
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
		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QStringList& list,
							     bool e ) ;

		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QStringList& list,
							     int,
							     bool e ) ;

		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QStringList& list,
							     const QByteArray& password = QByteArray() )
		{
			return ::Task::run( [ = ](){

				return utility::Task( exe,list,-1,utility::systemEnvironment(),password ) ;
			} ) ;
		}

		static void exec( const QString& exe,
				  const QStringList& list,
				  const QProcessEnvironment& env = utility::systemEnvironment(),
				  std::function< void() > f = [](){} )
		{
			::Task::exec( [ = ](){ utility::Task( exe,list,env,f ) ; } ) ;
		}
		static void wait( int s )
		{
			sleep( static_cast< unsigned int >( s ) ) ;
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
		Task()
		{
		}
		Task( const ::Task::process::result& e )
		{
			m_stdError = e.std_error() ;
			m_stdOut   = e.std_out() ;
			m_exitCode = e.exit_code() ;
			m_exitStatus = e.exit_status() ;
			m_finished = e.finished() ;
		}
		Task( const QString& exe,
		      const QStringList& list = QStringList(),
		      int waitTime = -1,
		      const QProcessEnvironment& env = utility::systemEnvironment(),
		      const QByteArray& password = QByteArray(),
		      std::function< void() > f = [](){},
		      bool use_polkit = false,
		      bool runs_in_background = true )
		{
			this->execute( exe,
				       list,
				       waitTime,
				       env,
				       password,
				       std::move( f ),
				       use_polkit,
				       runs_in_background ) ;
		}
		Task( const QString& exe,
		      const QStringList& list,
		      const QProcessEnvironment& env,
		      std::function< void() > f,
		      bool use_polkit = false )
		{
			this->execute( exe,
				       list,
				       -1,
				       env,
				       QByteArray(),
				       std::move( f ),
				       use_polkit,
				       true ) ;
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
		::Task::process::result toProcessResult()
		{
			return { m_stdOut,m_stdError,m_exitCode,m_exitStatus,m_finished } ;
		}
	private:
		void execute( const QString& exe,
			      const QStringList& list,
			      int waitTime,
			      const QProcessEnvironment& env,
			      const QByteArray& password,
			      std::function< void() > f,
			      bool e,
			      bool runs_in_background ) ;

		QByteArray m_stdOut ;
		QByteArray m_stdError ;
		int m_exitCode = 255 ;
		int m_exitStatus = 255 ;
		bool m_finished = false ;
	};

	using task_result = utility2::result< utility::Task > ;
}

#endif // MISCFUNCTIONS_H
