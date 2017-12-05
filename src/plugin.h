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


#ifndef HMAC_H
#define HMAC_H

#include <QDialog>
#include <QObject>
#include <QString>
#include <QVector>

#include <functional>

class QCloseEvent ;
class QEvent ;

#include <QWidget>
#include "plugins.h"
#include "task.hpp"

namespace Ui {
class plugin;
}

class plugin : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,
			      QDialog * dialog,
			      plugins::plugin t,
			      const QString& f,
			      std::function< void( const QByteArray& ) > e )
	{
		new plugin( parent,dialog,t,f,e ) ;
	}
	plugin( QWidget * parent,
		QDialog * dialog,
		plugins::plugin,
		const QString&,
		std::function< void( const QByteArray& ) > ) ;
	~plugin() ;
private slots:
	void pbSetKey() ;
	void pbSelectKeyFile() ;
	void pbClose() ;
private:
	void ShowUI() ;
	void HideUI() ;
	void enableAll() ;
	void disableAll() ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::plugin * m_ui ;
	QByteArray m_key ;
	std::function< void( const QByteArray& ) > m_function ;
	plugins::plugin m_pluginType ;
	QWidget * m_parentWidget ;
	QDialog * m_dialog ;
};

#endif // HMAC_H
