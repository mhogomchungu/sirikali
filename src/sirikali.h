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

#include "volumeinfo.h"
#include "utility.h"
#include "secrets.h"

#include "lxqt_wallet.h"

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class mountinfo ;

namespace Ui {
class sirikali ;
}

class sirikali : public QWidget
{
	Q_OBJECT
public:
	explicit sirikali( QWidget * parent = 0 ) ;
	~sirikali() ;
	void Show( void ) ;
public slots:
	void raiseWindow( void ) ;
	void raiseWindow( QString ) ;
private slots:
	void autoUpdateCheck( void ) ;
	void ecryptfsProperties( void ) ;
	void securefsproperties( void ) ;
	void autoMountKeyStorage( void ) ;
	void autoMountKeySource( QAction * ) ;
	void showMountDialogWhenAutoMounting( bool ) ;
	void autoMountWhenAvailable( bool ) ;
	void setDefaultMountPointPrefix( void ) ;
	void properties( void ) ;
	void autoCheckUpdates( bool ) ;
	void reuseMountPoint( bool ) ;
	void autoMountFavoritesOnStartUp( bool ) ;
	void unlockVolume( const QString&,const QString&,const QString&,
			   const QString&,const QString&,bool ) ;
	void aboutToShowMenu( void ) ;
	void changeInternalWalletPassWord( void ) ;
	void closeApplication( void ) ;
	void unlockVolume( void ) ;
	void startGUI( void ) ;
	void showMoungDialog( const volumeInfo& ) ;
	void showMoungDialog( const QString&,const QString& = QString() ) ;
	void mount( const volumeInfo&,const QString& = QString(),const QByteArray& = QByteArray() ) ;
	void defaultButton( void ) ;
	void itemClicked( QTableWidgetItem * ) ;
	void pbUpdate( void ) ;
	void createVolume( QAction * = nullptr ) ;
	void slotMount( void ) ;
	void unMountAll( void ) ;
	void pbUmount( void ) ;
	void slotTrayClicked( QSystemTrayIcon::ActivationReason = QSystemTrayIcon::Trigger ) ;
	void slotCurrentItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void enableAll( void ) ;
	void enableAll_1( void ) ;
	void slotOpenFolder( void ) ;
	void slotOpenSharedFolder( void ) ;
	void addEntryToTable( const QStringList& ) ;
	void addEntryToTable( const volumeInfo& ) ;
	void autoOpenFolderOnMount( bool ) ;
	void removeEntryFromTable( QString ) ;
	void showFavorites( void ) ;
	void favoriteClicked( QAction * ) ;
	void keyManagerClicked( QAction * ) ;
	void openMountPointPath( QString ) ;
	void licenseInfo( void ) ;
	void updateCheck( void ) ;
	void languageMenu( QAction * ac ) ;
	void autoMountFavoritesOnAvailable( QString ) ;
private:
	QString resolveFavoriteMountPoint( const QString& ) ;

	QFont getSystemVolumeFont( void ) ;

	bool autoOpenFolderOnMount( void ) ;

	void updateVolumeList( const QVector< volumeInfo >& ) ;
	void openMountPoint( const QString& ) ;
	void setLocalizationLanguage( bool ) ;

	void dragEnterEvent( QDragEnterEvent * ) ;
	void dropEvent( QDropEvent * ) ;
	void showContextMenu( QTableWidgetItem *,bool ) ;
	void startAutoMonitor( void ) ;
	void updateList( const volumeInfo& ) ;
	void setUpAppMenu( void ) ;
	void disableAll( void ) ;
	void closeEvent( QCloseEvent * e ) ;
	void setUpFont( void ) ;
	void setUpShortCuts( void ) ;
	void setUpApp( const QString& ) ;
	void autoUnlockVolumes( void ) ;
	QVector< favorites::entry > autoUnlockVolumes( const QVector< favorites::entry >& ) ;

	Ui::sirikali * m_ui = nullptr ;

	secrets m_secrets ;

	QMenu * m_favorite_menu = nullptr ;
	QMenu * m_hidden_volume_menu = nullptr ;
	QMenu * m_not_hidden_volume_menu = nullptr ;
	QMenu * m_key_manager_menu = nullptr ;
	QMenu * m_language_menu = nullptr ;
	QMenu * m_autoMountKeyStorage = nullptr ;

	QVector< std::pair< QAction *,const char * > > m_actionPair ;
	QVector< std::pair< QMenu *,const char * > > m_menuPair ;

	QAction * m_unMountAll = nullptr ;
	QAction * m_change_password_action = nullptr ;

	bool m_startHidden ;
	bool m_autoOpenFolderOnMount ;
	bool m_removeAllVolumes = false ;

	QString m_env ;
	QString m_sharedFolderPath ;
	QString m_folderOpener ;

	QSettings m_settings ;

	QSystemTrayIcon m_trayIcon ;

	mountinfo& m_mountInfo ;
};

#endif // MAINWINDOW_H
