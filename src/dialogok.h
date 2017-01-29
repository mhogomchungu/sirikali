/*
 *
 *  Copyright ( c ) 2016
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

#ifndef DIALOGOK_H
#define DIALOGOK_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>

namespace Ui {
class dialogok;
}

class dialogok : public QDialog
{
	Q_OBJECT
public:
	dialogok( QWidget * parent,QDialog *,bool,bool,const QString& title,const QString& msg ) ;
	~dialogok() ;
	int Show() ;
private slots:
	void ok() ;
	void yes() ;
	void no() ;
private:
	void closeEvent( QCloseEvent * ) ;
	Ui::dialogok * m_ui ;
	QDialog * m_dialog ;
} ;

#endif // DIALOGOK_H
