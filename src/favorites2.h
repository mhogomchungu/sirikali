#ifndef FAVORITES2_H
#define FAVORITES2_H
/*
 *
 *  Copyright (c) 2019
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

#include <QDialog>
#include <QTableWidget>
#include <QCloseEvent>

#include "settings.h"
#include "favorites.h"

namespace Ui {
class favorites2;
}

class favorites2 : public QDialog
{
	Q_OBJECT
public:
	static favorites2& instance( QWidget * parent,favorites::type type = favorites::type::others )
	{
		return *( new favorites2( parent,type ) ) ;
	}
	favorites2( QWidget * parent,favorites::type type = favorites::type::others ) ;
	~favorites2() ;
private :
	void tabChanged( int ) ;
	void updateVolumeList( const std::vector< favorites::entry >&,const QString& ) ;
	void updateVolumeList( const std::vector< favorites::entry >&,int ) ;
	void showUpdatedEntry( const favorites::entry& ) ;
	void updateFavorite( bool );
	void setOptionMenu( QMenu&,bool ) ;
	void toggleAutoMount( void ) ;
	void edit( void ) ;
	void configPath( void ) ;
	void removeEntryFromFavoriteList( void ) ;
	void add( void ) ;
	void cancel( void ) ;
	void folderPath( void ) ;
	void mountPointPath( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void shortcutPressed( void ) ;
	void devicePathTextChange( QString ) ;
	void clearEditVariables() ;
	void setVolumeProperties( const favorites::entry& e ) ;
	void ShowUI( favorites::type ) ;
	void HideUI( void ) ;
	void checkFavoritesConsistency() ;
	favorites::entry getEntry( int ) ;
	QString getExistingFile( const QString& ) ;
	QString getExistingDirectory( const QString& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void addEntries( const QStringList& ) ;
	Ui::favorites2 * m_ui ;
	QWidget * m_parentWidget ;
	favorites::type m_type ;
	int m_editRow ;
	bool m_reverseMode = false ;
	bool m_volumeNeedNoPassword = false ;
	bool m_mountReadOnly = false ;
	bool m_editMode = false ;
	QMenu m_optionMenu ;
	settings& m_settings ;
} ;

#endif // FAVORITES2_H
