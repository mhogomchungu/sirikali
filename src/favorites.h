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

#ifndef FAVORITES_H
#define FAVORITES_H

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
	static constexpr const char * reverseModeOption = "-SiriKaliReverseMode" ;
	static constexpr const char * volumeNeedNoPassword = "-SiriKaliVolumeNeedNoPassword" ;
	static constexpr const char * mountReadOnly = "-SiriKaliMountReadOnly" ;

	struct entry
	{
		class readOnly
		{
		public:
			readOnly() ;
			readOnly( bool e ) ;
			operator bool() const ;
			bool onlyRead() const ;
			bool operator==( const readOnly& other ) ;
		private:
			bool m_readOnlyVolume = false ;
			bool m_isSet = false ;
		} ;

		entry() ;
		entry( const QStringList& e ) ;
		entry( const QString& r ) ;
		QStringList list( bool e = true ) const ;
		QString string( char s = '\t' ) const ;
		QString configString() const ;
		QStringList configStringList() const;
		bool operator!=( const favorites::entry& other ) const ;
		bool operator==( const favorites::entry& other ) const ;
		bool autoMount() const ;
		QString sanitizedMountOptions() const ;
		static QString sanitizedMountOptions( const QString& s ) ;
		QString volumePath ;
		QString mountPointPath ;
		QString autoMountVolume ;
		QString configFilePath ;
		QString idleTimeOut ;
		QString mountOptions ;
		bool reverseMode = false ;
		bool volumeNeedNoPassword = false ;
		favorites::entry::readOnly readOnlyMode = readOnly() ;
	private:
		void config( const QStringList& e ) ;
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
	favorites::type m_type ;
	int m_editRow ;
	bool m_reverseMode = false ;
	bool m_volumeNeedNoPassword = false ;
	bool m_mountReadOnly = false ;
};

#endif // FAVORITES_H
