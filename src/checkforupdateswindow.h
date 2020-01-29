/*
 *
 *  Copyright (c) 2020
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

#ifndef CHECKFORUPDATESWINDOW_H
#define CHECKFORUPDATESWINDOW_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>

#include <utility>
#include <functional>

namespace Ui {
class checkforupdateswindow;
}

class checkforupdateswindow : public QDialog
{
	Q_OBJECT
public:
	using functions = std::pair< std::function< void() >,std::function< void() > > ;

	static checkforupdateswindow& instance( QWidget * parent,functions& function )
	{
		return *( new checkforupdateswindow( parent,function ) ) ;
	}
	explicit checkforupdateswindow( QWidget * parent,functions& ) ;
	void ShowUI( const QString& ) ;
	~checkforupdateswindow() ;
private:
	void closeEvent( QCloseEvent * ) override ;
	void closeUI() ;
	Ui::checkforupdateswindow * m_ui ;
	functions& m_functions ;
};

#endif // CHECKFORUPDATESWINDOW_H
