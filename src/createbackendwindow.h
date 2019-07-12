/*
 *
 *  Copyright ( c ) 2019
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

#ifndef CREATEBACKENDWINDOW_H
#define CREATEBACKENDWINDOW_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class createBackendWIndow;
}

class createBackendWIndow : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * m )
	{
		new createBackendWIndow( m ) ;
	}
	explicit createBackendWIndow( QWidget * parent ) ;
	~createBackendWIndow() ;
private:
	void save() ;
	void Hide() ;
	void closeEvent( QCloseEvent * ) ;
	Ui::createBackendWIndow * m_ui ;
};

#endif // CREATEBACKENDWINDOW_H
