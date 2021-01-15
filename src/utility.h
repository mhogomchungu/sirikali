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
#include <QDateTime>
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
#include <type_traits>

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
#include "version.h"

#include <QObject>
#include <QLabel>

#include <fcntl.h>

#include <iostream>
class QByteArray ;
class QEvent ;

namespace utility
{
	class miscOptions{
	public:
		static miscOptions& instance()
		{
			static miscOptions s ;
			return s ;
		}
		const QByteArray& getCookie() const
		{
			return m_cookie ;
		}
		const QString& getSocketPath() const
		{
			return m_polkit_socket_path ;
		}
		bool usePolkit() const
		{
			return m_use_polkit ;
		}
		bool debugEnabled() const
		{
			return m_enable_debug ;
		}
		bool runningOnGUIThread() const
		{
			return m_main_gui_thread == QThread::currentThread() ;
		}
		bool runningOnBackGroundThread() const
		{
			return m_main_gui_thread != QThread::currentThread() ;
		}
		bool starting() const
		{
			return m_starting ;
		}
		QThread * getMainGUIThread() const
		{
			return m_main_gui_thread ;
		}
		QWidget * getMainQtWidget() const
		{
			return m_mainQWidget ;
		}
		utility2::result< debugWindow * > getDebugWindow() const
		{
			if( m_debugWindow ){

				return m_debugWindow ;
			}else{
				return {} ;
			}
		}
		void doneStarting()
		{
			m_starting = false ;
		}
		const QString& getStartingLogs() const
		{
			return m_starting_error_logs ;
		}
		miscOptions& appendLog( const QString& e )
		{
			m_starting_error_logs += e ;
			return *this ;
		}
		miscOptions& setCookie( const QByteArray& e )
		{
			m_cookie = e ;
			return *this ;
		}
		miscOptions& setUsePolkit( bool e )
		{
			m_use_polkit = e ;
			return *this ;
		}
		miscOptions& setDebugWindow( debugWindow * w )
		{
			m_debugWindow = w ;
			return *this ;
		}
		miscOptions& setPolkitPath( const QString& w )
		{
			m_polkit_socket_path = w ;
			return *this ;
		}
		miscOptions& setEnableDebug( bool w )
		{
			m_enable_debug = w ;
			return *this ;
		}
		miscOptions& setMainQtWidget( QWidget * w )
		{
			m_mainQWidget = w ;
			return *this ;
		}
		miscOptions& setCurrentThreadAsMain()
		{
			m_main_gui_thread = QThread::currentThread() ;
			return *this ;
		}
	private:
		QString m_polkit_socket_path ;
		QString m_starting_error_logs ;

		bool m_starting = true ;
		bool m_use_polkit = false ;
		bool m_enable_debug = false ;

		QByteArray m_cookie ;

		debugWindow * m_debugWindow = nullptr ;
		QThread * m_main_gui_thread = nullptr ;
		QWidget * m_mainQWidget = nullptr ;
	};

	class logger{
	public:
		static QString starLine() ;
		logger() ;
		std::function< void( const QString& ) > function() ;
		logger& showText( const QString& cmd,const QStringList& args ) ;
		logger& showText( const QString& e ) ;
		logger& showLine() ;
		logger& appendLog( const QString& e ) ;
		logger& showText( const ::Task::process::result& m ) ;
		logger& showTextWithLines( const QString& b ) ;
		logger& showDebugWindow() ;
		logger& enableDebug() ;
	private:
		utility::miscOptions& m_miscOptions ;
	};

	template< typename T >
	typename std::add_const<T>::type& asConst( T& t )
	{
		return t ;
	}

	template< typename T >
	void asConst( const T&& ) = delete ;

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

	QString SiriKaliVersion() ;

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

	void runInUiThread( std::function< void() > function ) ;

	void waitForOneSecond( void ) ;

	void wait( int ) ;

	bool waitForFinished( QProcess&,int timeOut = 5 ) ;

	template< typename T >
	static inline auto unwrap( Task::future< T >& x )
	{
		if( utility::miscOptions::instance().runningOnGUIThread() ){

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

	/*
	 * Function must take an int and must return bool
	 */
	template< typename Function,Task::detail::has_bool_return_type<Function,int > = 0 >
	static inline void Timer( int interval,Function&& function )
	{
		class Timer{
		public:
			Timer( int interval,Function&& function ) :
				m_function( std::forward< Function >( function ) )
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
			Function m_function ;
		} ;

		new Timer( interval,std::forward< Function >( function ) ) ;
	}

	/*
	 * Function must takes no argument and will be called once when the interval pass
	 */
	template< typename Function,Task::detail::has_no_argument< Function > = 0 >
	static inline void Timer( int interval,Function&& function )
	{
		utility::Timer( interval,[ function = std::forward< Function >( function ) ]( int s ){

			Q_UNUSED( s )

			function() ;

			return true ;
		} ) ;
	}

	void setDefaultMountPointPrefix( const QString& path ) ;

	qbytearray_result yubiKey( const QByteArray& challenge ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsFile( const QString& ) ;
	bool pathIsReadable( const QString&,bool isFolder = true ) ;
	bool pathIsWritable( const QString&,bool isFolder = true ) ;

	QString userName() ;
	QString userIDAsString() ;
	int userID() ;

	const QProcessEnvironment& systemEnvironment() ;

	QString likeSshaddPortNumber( const QString& path,const QString& port ) ;
	QString likeSshRemovePortNumber( const QString& path ) ;

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

	void quitHelper() ;
	void initGlobals() ;

	QString getVolumeID( const QString&,bool = false ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

	void applicationStarted() ;

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
		class moreOptions
		{
		public:
			moreOptions( bool usePolkit = false ) : m_usePolkit( usePolkit )
			{
			}
			moreOptions( bool usePolkit,bool likeSsh,utility2::LOGLEVEL logLevel ) :
				m_usePolkit( usePolkit ),
				m_likeSsh( likeSsh ),
				m_logLevel( logLevel )
			{
			}
			bool usePolkit() const
			{
				return m_usePolkit ;
			}
			const utility::bool_result& likeSsh() const
			{
				return m_likeSsh ;
			}
			const utility2::result< utility2::LOGLEVEL >& allowLogging() const
			{
				return m_logLevel ;
			}
		private:
			bool m_usePolkit = false ;
			utility::bool_result m_likeSsh ;
			utility2::result< utility2::LOGLEVEL > m_logLevel ;
		};
		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QStringList& list,
							     const utility::Task::moreOptions& moreOpts ) ;

		static ::Task::future< utility::Task >& run( const QString& exe,
							     const QStringList& list,
							     int,
							     const utility::Task::moreOptions& moreOpts ) ;

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
		      const utility::Task::moreOptions& moreOpts = utility::Task::moreOptions() )
		{
			this->execute( exe,
				       list,
				       waitTime,
				       env,
				       password,
				       std::move( f ),
				       moreOpts ) ;
		}
		Task( const QString& exe,
		      const QStringList& list,
		      const QProcessEnvironment& env,
		      std::function< void() > f,
		      const utility::Task::moreOptions& moreOpts = utility::Task::moreOptions() )
		{
			this->execute( exe,
				       list,
				       -1,
				       env,
				       QByteArray(),
				       std::move( f ),
				       moreOpts ) ;
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
			      const utility::Task::moreOptions& boolOpts ) ;

		QByteArray m_stdOut ;
		QByteArray m_stdError ;
		int m_exitCode = 255 ;
		int m_exitStatus = 255 ;
		bool m_finished = false ;
	};

	using task_result = utility2::result< utility::Task > ;
}

namespace utility
{
class unlockIntervalReporter
{
public:
	static utility2::raii< unlockIntervalReporter > object( bool s )
	{
		return unlockIntervalReporter( s ) ;
	}
	unlockIntervalReporter( utility::bool_result s ) :
		m_likeSsh( std::move( s ) ),
		m_origTime( QDateTime::currentSecsSinceEpoch() )
	{
	}
	void operator()() const
	{
		this->report() ;
	}
	void report() const ;
private:
	utility::bool_result m_likeSsh ;
	qint64 m_origTime ;
};
}
#endif // MISCFUNCTIONS_H
