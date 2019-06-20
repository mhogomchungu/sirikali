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

#ifndef WALLETCONFIG_H
#define WALLETCONFIG_H

#include <QDialog>
#include <QString>
#include <QMap>

#include "lxqt_wallet.h"
#include "utility.h"
#include "secrets.h"

#include <functional>
#include <utility>

class QCloseEvent ;
class QWidget ;
class QTableWidgetItem ;
class walletconfiginput ;

namespace Ui {
class walletconfig ;
}

class walletconfig : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,
			      secrets::wallet&& wallet,
			      std::function< void() > e = [](){}, QString folderPath = QString() )
	{
		new walletconfig( parent,std::move( wallet ),std::move( e ),folderPath ) ;
	}
	explicit walletconfig( QWidget * parent,secrets::wallet&&,std::function< void() >, QString folderPath ) ;
	~walletconfig() ;
	void HideUI( void ) ;
	static bool addKey( secrets::wallet&,const QString& id,
			    const QString& key,const QString& comment ) ;
	static void deleteKey( secrets::wallet&,const QString& id ) ;
private slots:
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * item ) ;
	void pbDelete( void ) ;
	void pbClose( void ) ;
	void pbAdd( void ) ;
private:
	void accessWallet( void ) ;
	void itemClicked_0( QTableWidgetItem * item ) ;
	void enableAll( void ) ;
	void disableAll( void ) ;
	Ui::walletconfig * m_ui ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;

	secrets::wallet m_wallet ;
	int m_row ;
	QString m_volumeID ;
	QString m_comment ;
	QString m_key ;
	QWidget * m_parentWidget ;
	std::function< void() > m_function ;
};

#endif // KWALLETCONFIG_H
