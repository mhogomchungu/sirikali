/*
 *
 *  Copyright (c) 2015
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

#ifndef READONLYWARNING_H
#define READONLYWARNING_H

#include <QDialog>
#include <QString>

class QWidget ;
class QCloseEvent ;

namespace Ui {
class readOnlyWarning ;
}

class readOnlyWarning : public QDialog
{
	Q_OBJECT
public:
	static bool showWarning( QWidget * parent = 0,bool checked = false )
	{
		auto w = new readOnlyWarning( parent,checked ) ;
		w->ShowUI() ;
		return checked ;
	}
	static bool getOpenVolumeReadOnlyOption() ;
	explicit readOnlyWarning( QWidget * parent = 0,bool checked = false ) ;
	~readOnlyWarning() ;
	void ShowUI( void ) ;
	void HideUI( void ) ;
private slots:
	void pbOK( void ) ;
	void checkBoxChecked( bool ) ;
private:
	void setReadOnlyOption( bool ) ;
	bool showUIwarning( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::readOnlyWarning * m_ui ;
	bool m_checked ;
};

#endif // READONLYWARNING_H
