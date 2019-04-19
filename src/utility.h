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
#include "favorites.h"
#include "plugins.h"
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
	using volumeList = std::vector< std::pair< favorites::entry,QByteArray > > ;

	template< typename T >
	class result
	{
	public:
		result()
		{
		}
		result( T e ) : m_valid( true ),m_value( std::move( e ) )
		{
		}
		T * operator->()
		{
			return &m_value ;
		}
		const T * operator->() const
		{
			return &m_value ;
		}
		T& operator*()
		{
			return m_value ;
		}
		const T& operator*() const
		{
			return m_value ;
		}
		operator bool()
		{
			return m_valid ;
		}
		bool has_value() const
		{
			return m_valid ;
		}
		T& value()
		{
			return m_value ;
		}
		const T& value() const
		{
			return m_value ;
		}
		void set( T value )
		{
			m_value = std::move( value ) ;
			m_valid = true ;
		}
	private:
		bool m_valid = false ;
		T m_value ;
	} ;

	class globalEnvironment
	{
	public:
		static globalEnvironment& instance() ;
		const QProcessEnvironment& get() const ;
		void insert( const QString&,const QString& ) ;
		void remove( const QString& ) ;
		globalEnvironment() ;
	private:
		QProcessEnvironment m_environment ;
	} ;
}

namespace utility
{
	struct debug
	{
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

	struct wallet
	{
		bool opened ;
		bool notConfigured ;
		QString key ;
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

	void setWindowOptions( QDialog * ) ;

	int startApplication( std::function< int() > ) ;

	bool printVersionOrHelpInfo( const QStringList& ) ;

	wallet getKey( const QString& keyID,LXQt::Wallet::Wallet&,QWidget * = nullptr ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	QString readPassword( bool = true ) ;

	QIcon getIcon() ;

	QStringList directoryList( const QString& e ) ;

	QString executableFullPath( const QString& ) ;

	QString freeWindowsDriveLetter() ;
	bool isDriveLetter( const QString& ) ;

	void setWindowsMountPointOptions( QWidget *,QTextEdit *,QPushButton * ) ;

	void setWindowsMountPointOptions( QWidget *,QLineEdit *,QPushButton * ) ;

	QString homeConfigPath( const QString& = QString() ) ;

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

	void waitForOneSecond( void ) ;

	void wait( int ) ;

	void waitForFinished( QProcess& ) ;

	template< typename Future >
	static inline auto unwrap( Future& x )
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

	void setDefaultMountPointPrefix( const QString& path ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsReadable( const QString&,bool isFolder = true ) ;
	bool pathIsWritable( const QString&,bool isFolder = true ) ;

	QString securefsPath() ;
	QString winFSPpath() ;

	void enableDebug( bool ) ;
	bool debugEnabled( void ) ;

	void enableFullDebug( bool ) ;
	bool debugFullEnabled( void ) ;

	const QProcessEnvironment& systemEnvironment() ;

	QString userName() ;

	QString configFilePath( QWidget *,const QString& ) ;

	QStringList split( const QString&,char = '\n' ) ;
	QStringList executableSearchPaths( void ) ;
	QString executableSearchPaths( const QString& ) ;

	void logCommandOutPut( const ::Task::process::result&,const QString& ) ;
	void logCommandOutPut( const QString& ) ;

	void setDebugWindow( debugWindow * ) ;
	void polkitFailedWarning( std::function< void() > ) ;
	bool useSiriPolkit( void ) ;
	void quitHelper() ;
	void initGlobals() ;
	QString helperSocketPath() ;

	QString wrap_su( const QString& ) ;

	QString getVolumeID( const QString&,bool = false ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

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

	::Task::future< utility::result< QString > >& backEndInstalledVersion( const QString& backend ) ;

	::Task::future< utility::result< bool > >& backendIsLessThan( const QString& backend,
								      const QString& version ) ;

	::Task::future< utility::result< bool > >& backendIsGreaterOrEqualTo( const QString& backend,
									      const QString& version ) ;

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
		static QString makePath( QString e )
		{
			e.replace( "\"","\"\"\"" ) ;

			return "\"" + e + "\"" ;
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
