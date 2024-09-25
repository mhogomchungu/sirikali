/*
 *
 *  Copyright ( c ) 2016
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

#ifndef SECRETS_H
#define SECRETS_H

#include "lxqt_wallet.h"

#include <QIcon>
#include <QWidget>

#include <functional>
#include <type_traits>
#include <utility>

#include "utility2.h"

void windowsDebugWindow( const QString& e,bool s ) ;

class secrets
{
public:
	class wallet
	{
	public:
		static void setParent( QWidget * w )
		{
			m_parent = w ;
		}

		wallet( LXQt::Wallet::Wallet * ) ;

		~wallet() ;

		LXQt::Wallet::Wallet& bk() const
		{
			return *m_wallet ;
		}

		LXQt::Wallet::Wallet * operator->()
		{
			return m_wallet ;
		}

		operator bool() const
		{
			return m_wallet ;
		}

		template< typename Events >
		void open( Events ev )
		{
			if( m_wallet->opened() ){

				ev.opened() ;
			}else{
				ev.before() ;

				auto s = this->walletInfo() ;

				m_wallet->setImage( QIcon( ":/sirikali" ) ) ;

				m_wallet->log( []( QString e ){

					windowsDebugWindow( e,e.contains( "CRITICAL" ) ) ;
				} );

				m_wallet->open( s.walletName,s.appName,ev,this->setParent() ) ;

				ev.after() ;
			}
		}

		struct walletKey
		{
			bool opened ;
			bool notConfigured ;
			QString key ;
		} ;
	private:
		QWidget * setParent() ;
		struct info
		{
			QString walletName ;
			QString appName ;
		} ;

		info walletInfo() ;

		LXQt::Wallet::Wallet * m_wallet = nullptr ;

		static QWidget * m_parent ;
	} ;

	LXQt::Wallet::Wallet * walletBk( LXQt::Wallet::BackEnd ) const ;

	void changeInternalWalletPassword( QWidget *,const QString&,const QString&,std::function< void( bool ) > ) ;
	void changeWindowsDPAPIWalletPassword( QWidget *,const QString&,const QString&,std::function< void( bool ) > ) ;

	void close() ;
	secrets( const QString& ) ;
	secrets( const secrets& ) = delete ;

	secrets& operator=( const secrets& ) = delete ;

	~secrets() ;
private:
	class backends
	{
	public:
		backends() ;
		LXQt::Wallet::Wallet * get( LXQt::Wallet::BackEnd ) ;
		void close() ;
	private:
		struct bks
		{
			bks( LXQt::Wallet::BackEnd e,LXQt::Wallet::Wallet * s ) :
				bk( e ),wallet( s )
			{
			}
			LXQt::Wallet::BackEnd bk ;
			LXQt::Wallet::Wallet * wallet ;
		} ;

		std::vector< bks > m_backends ;

	} mutable m_backends ;
} ;

#endif
