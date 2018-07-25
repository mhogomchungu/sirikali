/*
 *
 *  Copyright ( c ) 2011-2015
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

#ifndef MANAGEDEVICENAMES_H
#define MANAGEDEVICENAMES_H

#include <QDialog>
#include <QString>
#include <QStringList>

#include "utility2.h"

class QAction ;
class QWidget ;
class QTableWidgetItem ;
class QCloseEvent ;

namespace Ui {
class favorites ;
}

class favorites : public QDialog
{
	Q_OBJECT
public:
	struct entry
	{
		entry()
		{
		}

		entry( const QStringList& e )
		{
			this->config( e ) ;
		}

		entry( const QString& r )
		{
			this->config( r.split( '\t',QString::SkipEmptyParts ) ) ;
		}

		QStringList list( bool e = true ) const
		{
			if( e ){

				return this->configString().split( '\t',QString::SkipEmptyParts ) ;
			}else{
				return { volumePath,
					 mountPointPath,
					 autoMountVolume,
					 configFilePath,
					 idleTimeOut,
					 mountOptions } ;
			}
		}

		QString string( char s = '\t' ) const
		{
			return this->list().join( QString( s ) ) ;
		}

		QString configString() const
		{
			auto _opt = [ ]( const QString& e )->QString{

				if( e.isEmpty() ){

					return "N/A" ;
				}else{
					return e ;
				}
			} ;

			auto e = "%1\t%2\t%3\t%4\t%5\t%6\t" ;

			return QString( e ).arg( volumePath,mountPointPath,autoMountVolume,
						 _opt( configFilePath ),_opt( idleTimeOut ),
						 _opt( mountOptions ) ) ;
		}
		QStringList configStringList() const
		{
			return this->configString().split( "\t",QString::SkipEmptyParts ) ;
		}
		bool operator!=( const favorites::entry& other ) const
		{
			return !( *this == other ) ;
		}

		bool operator==( const favorites::entry& other ) const
		{
			return  this->volumePath      == other.volumePath &&
				this->mountPointPath  == other.mountPointPath &&
				this->autoMountVolume == other.autoMountVolume &&
				this->configFilePath  == other.configFilePath &&
				this->idleTimeOut     == other.idleTimeOut &&
				this->mountOptions    == other.mountOptions ;
		}

		bool autoMount() const
		{
			return autoMountVolume == "true" ;
		}

		QString volumePath ;
		QString mountPointPath ;
		QString autoMountVolume ;
		QString configFilePath ;
		QString idleTimeOut ;
		QString mountOptions ;

	private:
		void config( const QStringList& e )
		{
			utility2::stringListToStrings( e,
						      volumePath,
						      mountPointPath,
						      autoMountVolume,
						      configFilePath,
						      idleTimeOut,
						      mountOptions ) ;

			if( configFilePath == "N/A" ){

				configFilePath.clear() ;
			}

			if( idleTimeOut == "N/A" ){

				idleTimeOut.clear() ;
			}

			if( mountOptions == "N/A" ){

				mountOptions.clear() ;
			}
		}
	};

	enum class type{ sshfs,others } ;
	static favorites& instance( QWidget * parent = 0,favorites::type type = favorites::type::others )
	{
		return *( new favorites( parent,type ) ) ;
	}
	explicit favorites( QWidget * parent = 0,favorites::type type = favorites::type::others ) ;
	~favorites() ;
signals:
	void ShowPartitionUI( void ) ;
private slots:
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
private:
	void ShowUI( favorites::type ) ;
	void HideUI( void ) ;
	void checkFavoritesConsistency() ;
	favorites::entry getEntry( int ) ;
	QString getExistingFile( const QString& ) ;
	QString getExistingDirectory( const QString& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	void addEntries( const QStringList& ) ;
	Ui::favorites * m_ui ;
	QWidget * m_parentWidget ;
	int m_editRow ;
};

#endif // MANAGEDEVICENAMES_H
