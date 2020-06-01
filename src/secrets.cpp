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

#include "secrets.h"
#include "utility.h"
#include "win.h"
#include "settings.h"

secrets::secrets( QWidget * parent ) : m_parent( parent )
{
}

void secrets::changeInternalWalletPassword( const QString& walletName,
					    const QString& appName,
					    std::function< void( bool ) > ff )
{
	auto e = this->internalWallet() ;

	e->closeWallet() ;

	e->changeWalletPassWord( walletName,appName,[ ff = std::move( ff ) ]( bool q ){

		ff( q ) ;
	} ) ;
}

void secrets::changeWindowsDPAPIWalletPassword( const QString& walletName,
						const QString& appName,
						std::function< void( bool ) > f )
{
	auto s = this->windows_dpapiBackend() ;

	s->changeWalletPassWord( walletName,appName,[ f = std::move( f ) ]( bool q ){

		f( q ) ;
	} ) ;
}

secrets::~secrets()
{
	this->close() ;
}

void secrets::close()
{
	delete m_internalWallet ;
	m_internalWallet = nullptr ;

	delete m_windows_dpapi ;
	m_windows_dpapi = nullptr ;
}

LXQt::Wallet::Wallet * secrets::internalWallet() const
{
	if( m_internalWallet == nullptr ){

		namespace w = LXQt::Wallet ;

		m_internalWallet = w::getWalletBackend( w::BackEnd::internal ) ;

		m_internalWallet->setParent( m_parent ) ;
	}

	return m_internalWallet ;
}

LXQt::Wallet::Wallet * secrets::windows_dpapiBackend() const
{
	if( m_windows_dpapi == nullptr ){

		auto a = SiriKali::Windows::windowsWalletBackend() ;

		m_windows_dpapi = LXQt::Wallet::getWalletBackend( a ) ;

		m_windows_dpapi->setParent( m_parent ) ;
	}

	return m_windows_dpapi ;
}

secrets::wallet secrets::walletBk( LXQt::Wallet::BackEnd e ) const
{
	if( e == SiriKali::Windows::windowsWalletBackend() ){

		return this->windows_dpapiBackend() ;

	}else if( e == LXQt::Wallet::BackEnd::internal ){

		return this->internalWallet() ;
	}else{
		return LXQt::Wallet::getWalletBackend( e ) ;
	}
}

QWidget * secrets::parent() const
{
	return m_parent ;
}

void secrets::setParent( QWidget * w )
{
	m_parent = w ;
}

static void _delete( LXQt::Wallet::Wallet * w )
{
	if( w ){

		auto wb = SiriKali::Windows::windowsWalletBackend() ;

		auto m = w->backEnd() ;

		if( m == wb || m == LXQt::Wallet::BackEnd::internal ){

		}else{
			delete w ;
		}
	}
}

secrets::wallet::wallet()
{
}

secrets::wallet::wallet( LXQt::Wallet::Wallet * w ) : m_wallet( w )
{
}

secrets::wallet::~wallet()
{
	_delete( m_wallet ) ;
}

secrets::wallet::wallet( secrets::wallet&& w )
{
	_delete( m_wallet ) ;
	m_wallet = w.m_wallet ;
	w.m_wallet = nullptr ;
}

secrets::wallet::walletKey secrets::wallet::getKey( const QString& keyID,QWidget * widget )
{
	auto _getKey = []( LXQt::Wallet::Wallet * wallet,const QString& volumeID ){

		return ::Task::await( [ & ](){ return wallet->readValue( volumeID ) ; } ) ;
	} ;

	walletKey w{ false,false,"" } ;

	auto s = m_wallet->backEnd() ;
	auto& wlt = settings::instance() ;

	auto _open = [ & ]( bool s ){

		if( s ){

			auto m = this->openSync( [](){ return true ; },
						 [ & ](){ if( widget ){	widget->hide() ; } },
						 [ & ](){ if( widget ){	widget->show() ; } } ) ;

			w.opened = m ;

			if( w.opened ){

				w.key = _getKey( m_wallet,keyID ) ;
			}
		}else{
			w.notConfigured = true ;
		}
	} ;

	if( s == LXQt::Wallet::BackEnd::internal ){

		_open( LXQt::Wallet::walletExists( s,wlt.walletName(),wlt.applicationName() ) ) ;

	}else if( s == SiriKali::Windows::windowsWalletBackend() ){

		_open( true ) ;
	}else{
		w.opened = m_wallet->open( wlt.walletName( s ),wlt.applicationName() ) ;

		if( w.opened ){

			w.key = _getKey( m_wallet,keyID ) ;
		}
	}

	return w ;
}

secrets::wallet::info secrets::wallet::walletInfo()
{
	auto& e = settings::instance() ;

	return { e.walletName( m_wallet->backEnd() ),e.applicationName() } ;
}
