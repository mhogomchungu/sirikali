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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMenu>
#include <QString>
#include <QWidget>
#include <QDialog>
#include <QSettings>
#include <QRect>
#include <QStringList>
#include <QDir>
#include <QtGlobal>
#include <QTranslator>

#include "lxqt_wallet.h"
#include "favorites.h"
#include "engines.h"
#include <vector>
#include <array>
#include <functional>

class settings
{
public:
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
		bool operator==( const settings::walletBackEnd& other ) const
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

	class windowDimensions
	{
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

	class flatpakRuntimeOptions
	{
	public:
		flatpakRuntimeOptions() ;

		const QString& globalBinPath() const
		{
			return m_globalBinPath ;
		}
		const QString& architecture() const
		{
			return m_architecture ;
		}
		const QString& commitId() const
		{
			return m_commitId ;
		}
		const QString& localBinPath() const
		{
			return m_localBinPath ;
		}
		const QString& runtimePath() const
		{
			return m_runtimePath ;
		}
	private:
		QString flatpkakInfoFile() ;
		QSettings m_settings ;
		QString m_globalBinPath ;
		QString m_architecture ;
		QString m_commitId ;
		QString m_runtimePath ;
		QString m_localBinPath ;
	} ;

	class translator
	{
	public:
		translator() ;
		void setLanguage( const QByteArray& e ) ;
		~translator() ;
		const QString& UIName( const QString& name ) ;
		const QString& name( const QString& UIName ) ;
		QString translate( const QString& internalName ) ;
		const char * UINameUnTranslated( const QString& name ) ;
	private:
		struct entry{
			entry( const QString&,const char *,const QString& ) ;
			QString UINameTranslated ;
			const char * UINameUnTranslated ;
			QString internalName ;
		} ;
		void clear() ;
		QTranslator * m_translator = nullptr ;
		std::vector< entry > m_languages ;
	} ;

	static settings& instance()
	{
		static settings s ;
		return s ;
	}

	static bool portableVersion() ;
	static QString portableVersionConfigPath() ;

	const flatpakRuntimeOptions& flatpakIntance() ;

	settings::windowDimensions getWindowDimensions() ;
	void setWindowDimensions( const settings::windowDimensions& ) ;
	settings() ;

	bool flatpakUpdated() ;
	bool showSystemTray() ;
	void setShowSystemTray( bool ) ;
	bool showCipherFolderAndMountPathInFavoritesList() ;
	int pollForUpdatesInterval() ;
	int sshfsBackendTimeout() ;
	void setWindowsExecutableSearchPath( const QString& ) ;
	void setExecutableSearchPath( const QString& ) ;
	QString windowsExecutableSearchPath() ;
	QString executableSearchPath() ;
	QString defaultExecutableSearchPath() ;
	QByteArray windowsKeysStorageData() ;
	void windowsKeysStorageData( const QByteArray& ) ;
	QString externalPluginExecutable() ;
	QString ykchalrespArguments() ;
	QString portSeparator() ;
	bool yubikeyRemoveNewLine() ;
	void setExternalPluginExecutable( const QString& ) ;
	bool enableRevealingPasswords() ;
	void scaleGUI() ;
	void setParent( QWidget * parent,QWidget ** localParent,QDialog * dialog ) ;
	QString fileManager() ;
	QStringList openWith() ;
	QString readPassword( bool addNewLine ) ;
	QString mountPath() ;
	QString mountPath( const QString& ) ;
	QString ConfigLocation() ;
	QString environmentalVariableVolumeKey() ;
	bool enableHighDpiScaling() ;
	void enableHighDpiScaling( bool ) ;
	void showDebugWindowOnStartup( bool ) ;
	bool showDebugWindowOnStartup() ;
	QByteArray enabledHighDpiScalingFactor() ;
	void enabledHighDpiScalingFactor( const QString& ) ;
	void removeKey( const QString& ) ;
	void allowExternalToolsToReadPasswords( bool ) ;
	bool allowExternalToolsToReadPasswords() ;
	void setDefaultMountPointPrefix( const QString& ) ;
	bool startMinimized() ;
	bool passWordIsUTF8Encoded() ;
	void setStartMinimized( bool ) ;
	void setFileManager( const QString& ) ;
	QString preUnMountCommand() ;
	void preUnMountCommand( const QString& ) ;
	void runCommandOnMount( const QString& ) ;
	QString runCommandOnMount() ;
	QString runCommandOnInterval() ;
	void runCommandOnInterval( const QString& ) ;
	int runCommandOnIntervalTime() ;
	void runCommandOnIntervalTime( int ) ;
	bool reUseMountPoint() ;
	void reUseMountPoint( bool ) ;
	bool autoOpenFolderOnMount() ;
	void autoOpenFolderOnMount( bool ) ;
	bool autoCheck() ;
	void autoCheck( bool ) ;
	bool readOnlyWarning() ;
	void readOnlyWarning( bool ) ;
	bool doNotShowReadOnlyWarning() ;
	void doNotShowReadOnlyWarning( bool ) ;
	bool autoMountFavoritesOnStartUp() ;
	enum class themeType{ platformDefault,dark,light } ;
	themeType getThemeType() ;
	void setThemeType( themeType ) ;
	void autoMountFavoritesOnStartUp( bool ) ;
	void autoMountFavoritesOnAvailable( bool ) ;
	bool autoMountFavoritesOnAvailable() ;
	void showFavoritesInContextMenu( bool ) ;
	bool showFavoritesInContextMenu() ;
	int networkTimeOut() ;
	bool showMountDialogWhenAutoMounting() ;
	bool showUnlockedVolumesFromAllUsers() ;
	int checkForUpdateInterval() ;
	int windowsPbkdf2Interations() ;
	bool ecryptfsAllowNotEncryptingFileNames() ;
	QString homePath() ;
	QString windowsMountPointPath() ;
	bool windowsUseMountPointPath( const engines::engine& ) ;
	void clearFavorites() ;
	void showMountDialogWhenAutoMounting( bool ) ;
	int favoritesEntrySize() ;
	void setLocalizationLanguage( const QString& language ) ;
	QString localizationLanguage( );
	QString walletName( LXQt::Wallet::BackEnd ) ;
	settings::walletBackEnd autoMountBackEnd() ;
	void autoMountBackEnd( const settings::walletBackEnd& ) ;
	QSettings& backend();
	bool setOpenVolumeReadOnly( QWidget * parent,bool checked ) ;
	bool getOpenVolumeReadOnlyOption() ;
	bool readFavorites( QMenu * m ) ;
	QString localizationLanguagePath() ;
	void languageMenu( QMenu * m,QAction * ac,settings::translator& ) ;
	void setLocalizationLanguage( bool translate,QMenu * m,settings::translator& ) ;
	QString walletName() ;
	QString applicationName() ;
	int readPasswordMaximumLength() ;
	bool unMountVolumesOnLogout() ;
	QStringList mountMonitorFolderPaths() ;
	QStringList supportedFileSystemsOnMountPaths() ;
	QString gvfsFuseMonitorPath() ;
	int mountMonitorFolderPollingInterval() ;
	int delayBeforeAutoMountAtStartup() ;
	bool internallyManageBackEnds() ;
	void setInternallyManageBackEnds( bool ) ;
private:
	bool cipherPathRepeats( const std::vector< favorites::entry >& ) ;
	bool showCipherPathAndMountPath( bool ) ;
	bool enableEntry( const QString& e ) ;
	QAction * addAaction( QMenu * m,const QString& e,const QString& s,const QString& ) ;
	std::unique_ptr< QSettings > m_settingsP ;
	QSettings& m_settings ;
	bool m_portableVersion ;
	flatpakRuntimeOptions m_flatpakRuntimeOptions ;
};

#endif //SETTINGS_H
