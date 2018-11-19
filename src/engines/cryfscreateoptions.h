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

#ifndef CRYFSCREATEOPTIONS_H
#define CRYFSCREATEOPTIONS_H

#include <functional>
#include <utility>

#include <QCloseEvent>
#include <QString>
#include <QDialog>

#include "../engines.h"

namespace Ui {
class cryfscreateoptions;
}

class cryfscreateoptions : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,std::function< void( const engines::engine::Options& ) > function )
	{
		new cryfscreateoptions( parent,std::move( function ) ) ;
	}
	explicit cryfscreateoptions( QWidget * parent,std::function< void( const engines::engine::Options& ) > ) ;
	~cryfscreateoptions() ;
private slots:
	void pbSelectConfigPath() ;
	void pbOK() ;
	void pbCancel() ;
private:
	void HideUI( const engines::engine::Options& = engines::engine::Options() ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::cryfscreateoptions * m_ui ;
	std::function< void( const engines::engine::Options& ) > m_function ;
};

#endif // CRYFSCREATEOPTIONS_H
