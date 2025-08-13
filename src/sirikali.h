/*
 *
 *  Copyright (c) 2012-2015
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QSystemTrayIcon>
#include <QVector>
#include <QSettings>
#include <QApplication>

#include "volumeinfo.h"
#include "utility.h"
#include "utility2.h"
#include "secrets.h"
#include "mountinfo.h"
#include "lxqt_wallet.h"
#include "keydialog.h"
#include "checkforupdates.h"
#include "configoptions.h"
#include "debugwindow.h"
#include "settings.h"
#include "systemsignalhandler.h"
#include "keydialog.h"
#include "tablewidget.h"

#include <vector>

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class mountinfo ;

namespace Ui {
class sirikali ;
}

class miniSiriKali : public QObject
{
	Q_OBJECT
public:
	miniSiriKali( std::function< int( secrets& ) > function ) ;
private slots :
	void start() ;
private:
	virtual void silenceWarning() ;
	std::function< int( secrets& ) > m_function ;
	secrets m_secrets ;
} ;

class sirikali : public QWidget
{
	Q_OBJECT
public:
	static int run( const QStringList&,int argc,char * argv[] ) ;
	static int unlockVolume( const QStringList&,secrets& ) ;

	sirikali( const QStringList&,QApplication& ) ;
	void start() ;
	void showTrayIcon( bool ) ;
	void updateLanguage( QAction * ac ) ;
	void enableAllAndSetFileManager() ;
	void enableAll() ;
	void disableAll() ;
	~sirikali() ;
private:
	void genericVolumeProperties() ;
	void unlockVolume( bool ) ;
	void startGUI( const QString& ) ;
	void startGUIx( const QString& ) ;
	void autoMount( const QString& ) ;
	void updateList() ;
	void emergencyShutDown() ;
	void slotOpenParentFolder() ;
	void slotOpenSharedFolder() ;
	void addEntryToTable( const QStringList& ) ;
	void addEntryToTable( const volumeInfo& ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites() ;
	void openMountPointPath( const QString& ) ;
	void openWith() ;
	void slotOpenFolder() ;
	void volumeProperties() ;
	void addToFavorites() ;
	void defaultButton() ;
	void favoriteClicked() ;
	void hideWindow() ;
	void createbackendwindow() ;
	void showDebugWindow() ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void setUpApp( const QString& ) ;
	void unMountAll() ;
	bool unMountAllBeforeQuitting() ;
	void autoMountFavoritesOnAvailable( QString ) ;

	void favoriteClickedA( QAction * ) ;
	void updateCheck() ;
	void closeApplication( int = 0,const QString& = QString() ) ;
	void closeApp() ;
	void FAQ() ;
	void licenseInfo() ;
	void configurationOptions() ;
	void unMountAllAndQuit() ;
	void pbUmount() ;
	void pbUpdate() ;
	void itemClicked( QTableWidgetItem * ) ;

	void trayClicked( QSystemTrayIcon::ActivationReason ) ;
	void trayClickedq() ;
	void createVolume( QAction * ) ;

	void setCreateMenu() ;
	void mountAll() ;
	void mountFavorite( const QString& ) ;

	void showTrayIcon() ;

	void mountMultipleVolumes( keyDialog::volumeList ) ;

	keyDialog::volumeList autoMount( keyDialog::volumeList l,bool ) ;

	void autoMount( keyDialog::volumeList&,
			keyDialog::entry&&,
			bool,
			bool ) ;

	QString resolveFavoriteMountPoint( const QString& ) ;

	QFont getSystemVolumeFont() ;

	bool attachParent() ;

	void updateFavoritesInContextMenu() ;
	void updateAgainFavoritesInContextMenu() ;
	void runIntervalCustomCommand( const QString& ) ;
	void runIntervalCustomCommand( const QString&,const QStringList&,const QByteArray& ) ;
	void updateVolumeList( const mountinfo::List&,bool enableAll = true ) ;
	void openMountPoint( const QString& ) ;
	void setLocalizationLanguage( bool ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void updateList( const volumeInfo& ) ;
	void setUpAppMenu() ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont() ;
	void setUpShortCuts() ;
	void showMainWindow() ;
	void raiseWindow( const QString& = QString() ) ;
	void autoUnlockVolumes( const mountinfo::List&,bool autoSetAutoMount = false ) ;
	void autoUnlockVolumes( favorites::volumeList,
				bool autoOpenFolderOnMount = false,
				bool skipUnknown = false,
				bool autoSetAutoMount = false ) ;

	template< typename ... Args >
	void autoUnlockAutoMount( favorites::volEntry&& vEntry,Args&& ... args )
	{
		favorites::volumeList s ;
		s.emplace_back( std::move( vEntry ) ) ;

		this->autoUnlockAutoMount( std::move( s ),
					   std::forward< Args >( args ) ... ) ;
	}

	template< typename ... Args >
	void autoUnlockAutoMount( favorites::volumeList&& volList,Args&& ... args )
	{
		this->autoUnlockVolumes( std::move( volList ),std::forward< Args >( args ) ... ) ;
	}

	void autoUnlockVolumesR( secrets::wallet&,
				 keyDialog::volumeList& ss,
				 bool autoOpenFolderOnMount,
				 bool autoSetAutoMount ) ;
	struct mountedEntry
	{
		const QString& cipherPath ;
		const QString& mountPoint ;
		const QString& volumeType ;
	};

	template< typename Function >
	void processMountedVolumes( QTableWidget * table,Function function )
	{
		const auto cipherFolders = tablewidget::columnEntries( table,0 ) ;
		const auto mountPoints   = tablewidget::columnEntries( table,1 ) ;
		const auto fileSystems   = tablewidget::columnEntries( table,2 ) ;

		for( auto r = cipherFolders.size() - 1 ; r >= 0 ; r-- ){

			const auto& a = cipherFolders.at( r ) ;
			const auto& b = mountPoints.at( r ) ;
			const auto& c = fileSystems.at( r ) ;

			function( { a,b,c } ) ;
		}
	}

	using cbVoid = void( sirikali::* )() ;
	using cbQAct = void( sirikali::* )( QAction * ) ;

	QAction * addAction( const QString& e,const char * z,cbVoid s )
	{
		auto ac = new QAction( e,this ) ;

		m_actionPair.emplace_back( ac,z ) ;

		ac->setFont( this->font() ) ;

		connect( ac,&QAction::triggered,this,s ) ;

		return ac ;
	}

	void addAction( QMenu& m,const QString& txt,sirikali::cbVoid s )
	{
		connect( m.addAction( txt ),&QAction::triggered,this,s ) ;
	}

	QAction * addAction( std::initializer_list< QKeySequence > s,sirikali::cbVoid m )
	{
		auto ac = new QAction( this ) ;

		ac->setShortcuts( s ) ;

		connect( ac,&QAction::triggered,this,m ) ;

		return ac ;
	}

	QMenu * addMenu( QMenu * m,const QString& r,const char * z,cbQAct t,cbVoid s )
	{
		auto e = m->addMenu( r ) ;

		m_menuPair.emplace_back( e,z ) ;

		e->setFont( this->font() ) ;

		connect( e,&QMenu::triggered,this,t ) ;

		connect( e,&QMenu::aboutToShow,this,s ) ;

		return e ;
	}

	engines::engine::cmdStatus unMountVolume( const sirikali::mountedEntry&,bool = false ) ;

	QApplication& m_app ;

	Ui::sirikali * m_ui = nullptr ;

	debugWindow m_debugWindow ;

	secrets m_secrets ;

	settings::translator m_translator ;

	QMenu * m_hidden_volume_menu = nullptr ;
	QMenu * m_not_hidden_volume_menu = nullptr ;
	QMenu * m_key_manager_menu = nullptr ;
	QMenu * m_main_menu = nullptr ;
	QMenu * m_context_menu = nullptr ;

	QMenu m_language_menu ;

	std::vector< std::pair< QAction *,const char * > > m_actionPair ;
	std::vector< std::pair< QMenu *,const char * > > m_menuPair ;

	QAction * m_unMountAll = nullptr ;
	QAction * m_change_password_action = nullptr ;

	bool m_startHidden ;
	bool m_autoOpenFolderOnMount ;
	bool m_emergencyShuttingDown = false ;
	bool m_initWallet = true ;
	QString m_sharedFolderPath ;
	QString m_folderOpener ;

	int m_exitStatus = 0 ;

	QSystemTrayIcon m_trayIcon ;

	mountinfo m_mountInfo ;

	checkUpdates m_checkUpdates ;

	configOptions m_configOptions ;

	systemSignalHandler m_signalHandler ;

	const QStringList m_argumentList ;

	class allowEnableAll{

	public:
		operator bool() const
		{
			return m_allowEnableAll ;
		}
		void setTrue( int token = -1 )
		{
			/*
			 * Only the person who knows the token can uset the option.
			 * Normal users set token of -1, super users set higher number tokens.
			 */
			if( token == m_token ){

				m_token = -1 ;
				m_allowEnableAll = true ;
			}
		}
		void setFalse( int token = -1 )
		{
			if( token != -1 && m_token != -1 ){

				utility::debug() << "Warning: Two users with super token just collided" ;

				return ;
			}
			/*
			 * Option can be changed only if token is unset or set by normal user.
			 */
			if( m_token == -1 ){

				m_token = token ;
				m_allowEnableAll = false ;
			}
		}
	private:
		int m_token = -1 ;
		bool m_allowEnableAll = true ;
	} m_allowEnableAll ;
};

class starter : public QObject
{
	Q_OBJECT
public:
	starter( const QStringList& args,QApplication& app ) : m_args( args ),m_app( app )
	{
		auto m = Qt::QueuedConnection ;

		connect( this,&starter::getStarted,this,&starter::start,m ) ;

		emit this->getStarted() ;
	}
	~starter()
	{
	}
	int exec()
	{
		return m_app.exec() ;
	}
signals:
	void getStarted() ;
private:
	void start() ;
	const QStringList& m_args ;
	QApplication& m_app ;
	std::unique_ptr< sirikali > m_sirikali ;
};

#endif // MAINWINDOW_H
