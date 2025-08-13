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

#ifndef CONFIGOPTIONS_H
#define CONFIGOPTIONS_H

#include <QDialog>
#include <QCloseEvent>
#include <QMenu>

#include <functional>
#include <utility>
#include <vector>
#include "secrets.h"
#include "settings.h"

namespace Ui {
class configOptions ;
}

class sirikali ;

class configOptions : public QDialog
{
	Q_OBJECT
public:
	configOptions( sirikali *,secrets&,QMenu * ) ;
	void ShowUI() ;
	void translateUI() ;
	~configOptions() ;
private:
	void HideUI() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::configOptions * m_ui ;
	secrets& m_secrets ;
	settings& m_settings ;
	sirikali& m_sirikali ;
	std::vector< std::pair< QAction *,const char * > > m_actionPair ;
};

#endif // CONFIGOPTIONS_H
