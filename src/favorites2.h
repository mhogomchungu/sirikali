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

#include <functional>
#include <memory>

namespace Ui {
class favorites2;
}

class favorites2 : public QDialog
{
	Q_OBJECT
public:
	static Task::future< void >& deleteKey( secrets::wallet&,const QString& id ) ;

	static Task::future< bool >& addKey( secrets::wallet&,
					     const QString& id,
					     const QString& key ) ;

	static favorites2& instance( QWidget * parent,
				     secrets& wallet,
				     std::function< void() > function,
				     const engines::engine& engine = engines::instance().getUnKnown(),
				     const QString& cipherPath = QString() )
	{
		return *( new favorites2( parent,
					  wallet,
					  std::move( function ),
					  engine,
					  cipherPath ) ) ;
	}
	favorites2( QWidget * parent,
		    secrets& wallet,
		    std::function< void() > function,
		    const engines::engine&,
		    const QString& cp ) ;
	~favorites2() ;
private :
	void showContextMenu( QTableWidgetItem * item,bool itemClicked ) ;
	void addkeyToWallet() ;
	void deleteKeyFromWallet( const QString& ) ;
	void walletBkChanged( LXQt::Wallet::BackEnd ) ;
	void setControlsAvailability( bool,bool clearTable ) ;
	void tabChanged( int ) ;
	void updateVolumeList( const std::vector< favorites::entry >&,const QString& ) ;
	void updateVolumeList( const std::vector< favorites::entry >&,size_t ) ;
	void showUpdatedEntry( const favorites::entry& ) ;
	void updateFavorite( bool );
	void toggleAutoMount( void ) ;
	void setCommand( QLineEdit * ) ;
	void edit( void ) ;
	void configPath( void ) ;
	void removeEntryFromFavoriteList( void ) ;
	void add( void ) ;
	void cancel( void ) ;
	void folderPath( void ) ;
	void filePath( void ) ;
	void mountPointPath( void ) ;
	void currentItemChanged( QTableWidgetItem * current,QTableWidgetItem * previous ) ;
	void itemClicked( QTableWidgetItem * current,bool ) ;
	void itemClicked( QTableWidgetItem * current ) ;
	void shortcutPressed( void ) ;
	void clearEditVariables() ;
	void setVolumeProperties( utility2::result_ref< const favorites::entry& > e ) ;
	void clearVolumeProperties() ;
	void ShowUI() ;
	void HideUI( void ) ;
	void checkFavoritesConsistency() ;
	void setUiLikeSsh( const QString& cipherPath,const engines::engine& engine ) ;
	void setDefaultUI( const engines::engine& ) ;
	QStringList readAllKeys() ;
	utility2::result_ref< const favorites::entry& > getEntry( int ) ;
	QString getExistingFile( const QString& ) ;
	QString getExistingDirectory( const QString& ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter( QObject * watched,QEvent * event ) ;
	Ui::favorites2 * m_ui ;
	secrets& m_secrets ;
	QWidget * m_parentWidget ;
	QMenu m_volPathFav ;

	int m_editRow ;
	bool m_reverseMode = false ;
	bool m_volumeNeedNoPassword = false ;
	bool m_mountReadOnly = false ;
	bool m_editMode = false ;
	settings& m_settings ;
	std::function< void() > m_function ;
	const engines::engine& m_engine ;
	QString m_cipherPath ;

	class wallet{

	public:
		void operator=( secrets::wallet s )
		{
			m_w.reset( new w{ std::move( s ) } ) ;
		}
		LXQt::Wallet::Wallet * operator->()
		{
			return m_w.get()->wallet.operator->() ;
		}
		secrets::wallet& get()
		{
			return m_w.get()->wallet ;
		}
		template< typename ... Args >
		void open( Args&& ... args )
		{
			m_w->wallet.open( std::forward< Args >( args ) ... ) ;
		}
		operator bool()
		{
			auto s = m_w.get() ;

			if( s == nullptr ){

				return false ;
			}else{
				return s->wallet.operator bool() ;
			}
		}
	private:
		struct w{
			secrets::wallet wallet ;
		} ;
		std::unique_ptr< w > m_w = nullptr ;

	} m_wallet ;
} ;

#endif // FAVORITES2_H
