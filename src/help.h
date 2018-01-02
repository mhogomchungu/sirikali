/*
 *
 *  Copyright ( c ) 2015
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


#ifndef HELP_H
#define HELP_H

#include <QDialog>
#include <QString>
#include <functional>
#include <utility>

class QCloseEvent ;
class QEvent ;
class QObject ;

namespace Ui {
class help;
}

class help : public QDialog
{
	Q_OBJECT
public:
        static void instance( QWidget * parent,const QString& path,std::function< void() > function )
	{
                new help( parent,path,std::move( function ) ) ;
	}
        help( QWidget * parent,const QString&,std::function< void() > ) ;
	~help() ;
private slots:
	void pbClose( void ) ;
	void pbOpenPDF( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::help * m_ui ;
	QString m_openPath ;
        std::function< void() > m_function ;
};

#endif // HELP_H
