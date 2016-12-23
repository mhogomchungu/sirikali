/*
 *  Copyright ( c ) 2012-2015
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

#ifndef DIALOGMSG_H
#define DIALOGMSG_H

#include <QDialog>
#include <QString>

class QWidget ;
class QCloseEvent ;

namespace Ui {
class DialogMsg ;
}

class DialogMsg : public QDialog
{
	Q_OBJECT
public:
	explicit DialogMsg( QWidget * parent = 0 ) ;
	void ShowUIOK( const QString& title,const QString& msg ) ;
	int  ShowUIYesNo( const QString& title,const QString& msg ) ;
	int  ShowUIYesNoDefaultNo( const QString& title,const QString& msg ) ;
	void ShowUIInfo( const QString& title,bool,const QString& msg ) ;
	void ShowUIVolumeProperties( const QString& title,const QString& msg ) ;
	bool ShowUIOKDoNotShowOption( const QString& title,const QString& msg ) ;
	void ShowPermissionProblem( const QString& device ) ;
	void ShowPermissionProblem( const QString&,const QString& device ) ;
	void HideUI( void ) ;
	~DialogMsg() ;

private slots:
	void pbOK( void ) ;
	void pbYes( void ) ;
	void pbNo( void ) ;

private:
	void SetUpButtons( void ) ;
	void ShowLabels( void ) ;
	void HideLabels( void ) ;
	void ShowUI( const QString& title,const QString& msg ) ;
	void setDimentions( const QString& msg ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::DialogMsg * m_ui ;
	QWidget * m_parent ;
	int m_status ;
};

#endif // DIALOGMSG_H
