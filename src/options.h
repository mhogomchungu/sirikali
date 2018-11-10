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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QAction>
#include <QCloseEvent>

#include <functional>

namespace Ui {
class options;
}

class options : public QDialog
{
	Q_OBJECT
public:
	struct Options{
		Options( const QStringList& e,bool r ) :
			options( e ),reverseMode( r ),success( true )
		{
		}
		Options() : success( false )
		{
		}
		QStringList options ;
		bool reverseMode ;
		bool success ;
	} ;
	static void instance( QWidget * parent,bool r,const Options& l,
			      std::function< void( const Options& ) >&& e )
	{
		new options( parent,r,l,std::move( e ) ) ;
	}

	options( QWidget * parent,bool,const Options&,std::function< void( const Options& ) >&& ) ;
	~options() ;
private slots:
        void pushButton( void ) ;
	void pbSet( void ) ;
	void pbCancel( void ) ;
private:
	void Hide( const Options& = Options() ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::options * m_ui ;
	bool m_create ;
	QString m_type ;
	std::function< void( const Options& ) > m_setOptions ;
	QWidget * m_parentWidget ;
};

#endif // OPTIONS_H
