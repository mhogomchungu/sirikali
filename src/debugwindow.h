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

#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class debugWindow;
}

class debugWindow : public QWidget
{
	Q_OBJECT
public:
        explicit debugWindow( QWidget * parent = nullptr ) ;
	~debugWindow();
        void Show() ;
        void Hide() ;
        void closeEvent( QCloseEvent * ) ;
signals:
	void UpdateOutPut( const QString&,bool ) ;
private slots:
	void UpdateOutPutSlot( const QString&,bool ) ;
private:
        Ui::debugWindow * m_ui ;
};

#endif // DEBUGWINDOW_H
