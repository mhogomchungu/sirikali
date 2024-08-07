/*
 *
 *  Copyright ( c ) 2017
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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QDialog>
#include <QCloseEvent>
#include <QString>

namespace Ui {
class fileManager;
}

class fileManager : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * w,QString& e,bool s )
	{
		new fileManager( w,e,s ) ;
	}
	explicit fileManager( QWidget * parent,QString&,bool ) ;
	~fileManager();
private:
	void set() ;
	void closeEvent( QCloseEvent * ) ;
	Ui::fileManager * m_ui ;
	QString& m_fileManager ;
	bool m_setFileManager ;
};

#endif // FILEMANAGER_H
