/*
 *  Copyright ( c ) 2012-2015
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


#ifndef WALLETCONFIGINPUT_H
#define WALLETCONFIGINPUT_H

#include <QDialog>
#include <QString>

#include <functional>
#include <memory>

#include "utility.h"

class QCloseEvent ;

namespace Ui {
class walletconfiginput;
}

class walletconfiginput : public QDialog
{
	Q_OBJECT
public:
	static walletconfiginput& instance( QWidget * parent,
					    std::function< void( const QString&,const QString&,const QString& ) > p,
					    std::function< void() > q )
	{
		return *( new walletconfiginput( parent,std::move( p ),std::move( q ) ) ) ;
	}

	walletconfiginput( QWidget * parent,
			   std::function< void( const QString&,const QString&,const QString& ) >,
			   std::function< void() > ) ;
	~walletconfiginput() ;
private slots:
	void pbAdd( void ) ;
	void slotCancel( void ) ;
	void pbImageFilePath( void ) ;
	void pbVolumePath( void ) ;
	void setvolumeID( QString ) ;
private:
	void HideUI( void ) ;
	void ShowUI( void ) ;
	void focus( void ) ;
	Ui::walletconfiginput * m_ui ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	std::function< void( const QString&,const QString&,const QString& ) > m_add ;
	std::function< void() > m_cancel ;
};

#endif // KWALLETCONFIGINPUT_H
