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

#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>

#include <QMenu>

class QCloseEvent ;
class QAction ;
class QTableWidgetItem ;
class QTableWidget ;

#include "sirikali.h"
#include "volumeinfo.h"
#include "utility.h"
#include "siritask.h"
#include "can_build_pwquality.h"
#include "secrets.h"

#include <functional>
#include <memory>

#if BUILD_PWQUALITY
class keystrength
{
public:
	keystrength() : m_handle( pwquality_default_settings() )
	{
	}
	~keystrength()
	{
		pwquality_free_settings( m_handle ) ;
	}
	int quality( const QString& e )
	{
		return pwquality_check( m_handle,e.toLatin1().constData(),
					nullptr,nullptr,nullptr ) ;
	}
	operator bool()
	{
		return true ;
	}
private:
	pwquality_settings_t * m_handle ;
};
#else
class keystrength
{
public:
	keystrength()
	{
	}
	~keystrength()
	{
	}
	int quality( const QString& e )
	{
		Q_UNUSED( e ) ;
		return 0 ;
	}
	operator bool()
	{
		return false ;
	}
private:
};
#endif

namespace Ui {
class keyDialog;
}

class keyDialog : public QDialog
{
	Q_OBJECT
public:
	static QString keyFileError() ;

	static keyDialog& instance( QWidget * parent,
				    QTableWidget * table,
				    secrets& s,
				    const volumeInfo& v,
				    std::function< void() > cancel,
				    std::function< void( const QString& ) > success,
				    const QString& exe = QString(),const QByteArray& key = QByteArray() )
	{
		return *( new keyDialog( parent,table,s,v,std::move( cancel ),std::move( success ),exe,key ) ) ;
	}
	keyDialog( QWidget * parent,
		   QTableWidget *,
		   secrets&,
		   const volumeInfo&,
		   std::function< void() >,
		   std::function< void( const QString& ) >,
		   const QString&,
		   const QByteArray& ) ;
	~keyDialog() ;
signals:
	void mounted( QString ) ;
	void cryptoOpen( QString ) ;
private slots:
	void cbVisibleKeyStateChanged( int ) ;
	void textChanged( QString ) ;
	void passWordTextChanged( QString ) ;
	void cbActicated( QString ) ;
	void pbOptions( void ) ;
	void pbkeyOption( void ) ;
	void pbMountPointPath( void ) ;
	void pbFolderPath( void ) ;
	void pbOK( void ) ;
	void plugIn( void ) ;
	void key( void ) ;
	void keyFile( void ) ;
	void HMACKeyFile( void ) ;
	void pbOpen( void ) ;
	void pbCancel( void ) ;
	void KeyFile( void ) ;
	void cbMountReadOnlyStateChanged( int ) ;
	void encryptedFolderMount( void ) ;
	void encryptedFolderCreate( void ) ;
private :	
	void ShowUI( void ) ;
	void HideUI( void ) ;

	void showErrorMessage( const siritask::cmdStatus& ) ;
	void showErrorMessage( const QString& ) ;

	void setUIVisible( bool ) ;
	void keyAndKeyFile( void ) ;
	void openVolume( void ) ;
	void enableAll( void ) ;
	void disableAll( void ) ;
	void windowSetTitle( const QString& = QString() ) ;
	void closeEvent( QCloseEvent * ) ;
	bool completed( const siritask::cmdStatus& ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;

	Ui::keyDialog * m_ui ;

	QByteArray m_key ;

	QString m_path ;
	QString m_deviceOffSet ;
	QString m_options ;
	QString m_configFile ;
	QString m_exe ;

	QStringList m_keyFiles ;

	QTableWidget * m_table ;

	bool m_working ;
	bool m_create ;
	bool m_reUseMountPoint ;

	secrets& m_secrets ;

	keystrength m_keyStrength ;

	typedef enum{ Key = 0,keyfile = 1,hmacKeyFile = 2,keyKeyFile = 3,Plugin = 4 } keyType ;

	keyType m_keyType ;

	QWidget * m_parentWidget ;

	std::function< void() > m_cancel ;
	std::function< void( const QString& ) > m_success ;
};

#endif // KEYDIALOG_H
