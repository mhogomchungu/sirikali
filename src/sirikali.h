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
	miniSiriKali( std::function< int( secrets& ) > function ) :
		m_function( std::move( function ) ),
		m_secrets( nullptr )
	{
	}
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

	sirikali( const QStringList& ) ;
	~sirikali() ;
private slots:
	void addToFavorites( void ) ;
	void createbackendwindow( void ) ;
	void showDebugWindow( void ) ;
	void configurationOptions( void ) ;
	void FAQ( void ) ;
	void hideWindow( void ) ;
	void setUpApp( const QString& ) ;
	void start() ;
	void autoUpdateCheck( void ) ;
	void volumeProperties() ;
	void genericVolumeProperties( void ) ;
	void closeApplication( int = 0,const QString& = QString() ) ;
	void unlockVolume( bool ) ;
	void startGUI( const QString& ) ;
	void startGUI( const QString&,bool ) ;
	void autoMount( const QString& ) ;
	void defaultButton( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void updateList( void ) ;
	void createVolume( QAction * = nullptr ) ;
	void unMountAll( void ) ;
	void emergencyShutDown( void ) ;
	void unMountAllAndQuit( void ) ;
	void pbUmount( void ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason = QSystemTrayIcon::Trigger ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void slotOpenFolder( void ) ;
	void slotOpenParentFolder( void ) ;
	void slotOpenSharedFolder( void ) ;
	void addEntryToTable( const QStringList& ) ;
	void addEntryToTable( const volumeInfo& ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites( void ) ;
	void favoriteClicked( QAction * ) ;
	void favoriteClicked() ;
	void openMountPointPath( const QString& ) ;
	void licenseInfo( void ) ;
	void updateCheck( void ) ;
	void autoMountFavoritesOnAvailable( QString ) ;
private:
	void mountAll() ;
	void mountFavorite( const QString& ) ;

	configOptions::functions configOption() ;

	void showTrayIcon() ;

	void mountMultipleVolumes( keyDialog::volumeList ) ;

	keyDialog::volumeList autoMount( keyDialog::volumeList l,bool ) ;

	void autoMount( keyDialog::volumeList&,
			keyDialog::entry&&,
			bool,
			bool ) ;

	QString resolveFavoriteMountPoint( const QString& ) ;

	QFont getSystemVolumeFont( void ) ;

	void updateFavoritesInContextMenu( void ) ;
	void runIntervalCustomCommand( const QString& ) ;
	void updateVolumeList( const mountinfo::List&,bool enableAll = true ) ;
	void openMountPoint( const QString& ) ;
	void setLocalizationLanguage( bool ) ;
	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void updateList( const volumeInfo& ) ;
	void setUpAppMenu( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void showMainWindow( void ) ;
	void raiseWindow( const QString& = QString() ) ;
	void autoUnlockVolumes( const mountinfo::List&,bool autoSetAutoMount = false ) ;
	keyDialog::volumeList autoUnlockVolumes( favorites::volumeList,
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
		auto mm = this->autoUnlockVolumes( std::move( volList ),
						   std::forward< Args >( args ) ... ) ;

		this->mountMultipleVolumes( std::move( mm ) ) ;
	}

	struct mountedEntry{
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

	engines::engine::cmdStatus unMountVolume( const sirikali::mountedEntry&,bool = false ) ;

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

#endif // MAINWINDOW_H
