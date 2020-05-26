#ifndef FSCRYPTCREATEOPTIONS_H
#define FSCRYPTCREATEOPTIONS_H

#include <functional>
#include <utility>
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

#include <QCloseEvent>
#include <QString>
#include <QDialog>
#include <QStringList>

#include "../engines.h"

namespace Ui {
class fscryptcreateoptions;
}

class fscryptcreateoptions : public QDialog
{
	Q_OBJECT
public:
	static void instance( const engines::engine::createGUIOptions& s,
			      const QStringList& l )
	{
		new fscryptcreateoptions( s,l ) ;
	}
	fscryptcreateoptions( const engines::engine::createGUIOptions&,const QStringList& ) ;
	~fscryptcreateoptions() ;
private:
	Ui::fscryptcreateoptions * m_ui ;
	void HideUI( const engines::engine::cOpts& = engines::engine::cOpts() ) ;
	void closeEvent( QCloseEvent * ) ;
	engines::engine::fcreate m_function ;
	QStringList m_protectorNames ;
};

#endif // FSCRYPTCREATEOPTIONS_H
