/*
 *
 *  Copyright (c) 2017
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

#ifndef SECUREFSCREATEOPTIONS_H
#define SECUREFSCREATEOPTIONS_H

#include <functional>
#include <utility>

#include <QCloseEvent>
#include <QString>
#include <QDialog>

namespace Ui {
class securefscreateoptions;
}

class securefscreateoptions : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,std::function< void( const QString& ) > function )
	{
		new securefscreateoptions( parent,std::move( function ) ) ;
	}
	explicit securefscreateoptions( QWidget * parent,std::function< void( const QString& ) > ) ;
	~securefscreateoptions() ;
private slots:
	void pbOK() ;
	void pbCancel() ;
private:
	void HideUI() ;
	void closeEvent( QCloseEvent * ) ;
	Ui::securefscreateoptions * m_ui ;
	std::function< void( const QString& ) > m_function ;
};

#endif // SECUREFSCREATEOPTIONS_H
