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

#include <QObject>
#include <QLabel>

#include <poll.h>
#include <fcntl.h>

class QByteArray ;
class QEvent ;

namespace utility
{
	class selectMenuOption : public QObject
	{
		Q_OBJECT
	public:
		using function_t = std::function< void( const QString& e ) > ;

		selectMenuOption( QMenu * m,bool e,
				  function_t && f = []( const QString& e ){ Q_UNUSED( e ) } ) :
		m_menu( m ),m_function( f )
		{
			if( e ){

				this->setParent( m ) ;
			}
		}
	public slots :
		void selectOption( const QString& f )
		{
			for( const auto& it : m_menu->actions() ){

				QString e = it->text() ;

				e.remove( "&" ) ;

				it->setChecked( f == e ) ;
			}

			m_function( f ) ;
		}
		void selectOption( QAction * ac )
		{
			auto e = ac->text() ;

			e.remove( "&" ) ;

			this->selectOption( e ) ;
		}
	private:
		QMenu * m_menu ;
		std::function< void( const QString& ) > m_function ;
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

	int startApplication( const char * appName,std::function<int()> ) ;

	wallet getKey( const QString& keyID,LXQt::Wallet::Wallet& ) ;

	QString cmdArgumentValue( const QStringList&,const QString& arg,const QString& defaulT = QString() ) ;

	QIcon getIcon() ;

	LXQt::Wallet::BackEnd autoMountBackEnd() ;

	QStringList directoryList( const QString& e ) ;

	QString executableFullPath( const QString& ) ;

	bool reUseMountPoint( void ) ;
	void reUseMountPoint( bool ) ;

	QString homeConfigPath( const QString& = QString() ) ;
	QString homePath() ;
	QString mountPath( const QString& path ) ;

	void setDefaultMountPointPrefix( const QString& path ) ;

	void setSettingsObject( QSettings * ) ;

	QString mountPathPostFix( const QString& path ) ;
	QString mountPathPostFix( const QString& prefix,const QString& path ) ;

	bool pathIsReadable( const QString& ) ;

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

	bool setOpenVolumeReadOnly( QWidget * parent,bool check ) ;
	bool getOpenVolumeReadOnlyOption() ;

	QStringList split( const QString&,char = '\n' ) ;
	void addToFavorite( const QString& dev,const QString& m_point ) ;

	QStringList readFavorites( void ) ;
	void readFavorites( QMenu *,bool = false ) ;
	void removeFavoriteEntry( const QString& ) ;
	QString getVolumeID( const QString&,bool = false ) ;
	QString localizationLanguage() ;
	QString localizationLanguagePath() ;
	void setLocalizationLanguage( const QString& ) ;
	QString walletName( void ) ;
	QString applicationName( void ) ;
	bool eventFilter( QObject * gui,QObject * watched,QEvent * event,std::function< void() > ) ;
	void licenseInfo( QWidget * ) ;

	void setLocalizationLanguage( bool translate,QMenu * m ) ;
	void languageMenu( QWidget *,QMenu *,QAction * ) ;

	using array_t = std::array< int,8 > ;

	utility::array_t getWindowDimensions() ;
	void setWindowDimensions( const std::initializer_list<int>& ) ;

	int pluginKey( QWidget *,QString *,const QString& ) ;

	QFont getFont( QWidget * ) ;
	void saveFont( const QFont& ) ;

	::Task::future< bool >& openPath( const QString& path,const QString& opener,const QString& env = QString() ) ;

	void openPath( const QString& path,const QString& opener,const QString& env,QWidget *,const QString&,const QString& ) ;
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
		static ::Task::future< utility::Task >& run( const QString& exe )
		{
			return ::Task::run< utility::Task >( [ exe ](){ return utility::Task( exe ) ; } ) ;
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
		      const QByteArray& password = QByteArray(),std::function< void() > f = [](){} )
		{
			this->execute( exe,waitTime,env,password,std::move( f ) ) ;
		}
		Task( const QString& exe,const QProcessEnvironment& env,std::function< void() > f )
		{
			this->execute( exe,-1,env,QByteArray(),std::move( f ) ) ;
		}
		QStringList splitOutput( char token,bool stdOutput = true ) const
		{
			if( stdOutput ){

				return utility::split( m_data,token ) ;
			}else{
				return utility::split( m_stdError,token ) ;
			}
		}
		void output( const QByteArray& r )
		{
			m_data = r ;
		}
		const QByteArray& output() const
		{
			return m_data ;
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
			      const QByteArray& password,std::function< void() >&& f )
		{
			class Process : public QProcess{
			public:
				Process( std::function< void() >&& f ) : m_function( std::move( f ) )
				{
				}
			protected:
				void setupChildProcess()
				{
					m_function() ;
				}
			private:
				std::function< void() > m_function ;
			} p( std::move( f ) ) ;

			p.setProcessEnvironment( env ) ;

			p.start( exe ) ;

			if( !password.isEmpty() ){

				p.waitForStarted() ;

				p.write( password + '\n' ) ;

				p.closeWriteChannel() ;
			}

			m_finished   = p.waitForFinished( waitTime ) ;
			m_exitCode   = p.exitCode() ;
			m_exitStatus = p.exitStatus() ;
			m_data       = p.readAll() ;
			m_stdError   = p.readAllStandardError() ;
		}

		QByteArray m_data ;
		QByteArray m_stdError ;
		int m_exitCode ;
		int m_exitStatus ;
		bool m_finished ;
	};
}

#endif // MISCFUNCTIONS_H
