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

#ifndef CONFIGFILEOPTION_H
#define CONFIGFILEOPTION_H

#include <QDialog>
#include <QStringList>
#include <QCloseEvent>

#include <functional>
#include <utility>

#include "engines.h"

namespace Ui {
class configFileOption;
}

class configFileOption : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * e,
			      const engines::engine& engine,
			      std::function< void( const QStringList& ) > s )
	{
		new configFileOption( e,engine,std::move( s ) ) ;
	}

	explicit configFileOption( QWidget * parent,
				   const engines::engine&,
				   std::function< void( const QStringList& ) > ) ;
	~configFileOption() ;
private slots:
	void pbCancel() ;
	void pbSet() ;
	void pbSetConfigFile() ;
private:
	void closeEvent( QCloseEvent * e ) ;
	Ui::configFileOption * m_ui ;
	QString m_configFileName ;
	std::function< void( const QStringList& ) > m_function ;
};

#endif // CONFIGFILEOPTION_H
