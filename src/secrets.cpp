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

secrets::secrets( QWidget * parent ) : m_parent( parent ),m_backends( m_parent )
{
}

void secrets::changeInternalWalletPassword( const QString& walletName,
					    const QString& appName,
					    std::function< void( bool ) > ff )
{
	auto e = m_backends.get( LXQt::Wallet::BackEnd::internal ) ;

	e->changeWalletPassWord( walletName,appName,[ ff = std::move( ff ) ]( bool q ){

		ff( q ) ;
	} ) ;
}

void secrets::changeWindowsDPAPIWalletPassword( const QString& walletName,
						const QString& appName,
						std::function< void( bool ) > f )
{
	auto s = m_backends.get( LXQt::Wallet::BackEnd::windows_dpapi ) ;

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
	m_backends.close() ;
}

secrets::wallet secrets::walletBk( LXQt::Wallet::BackEnd e ) const
{
	return m_backends.get( e ) ;
}

QWidget * secrets::parent() const
{
	return m_parent ;
}

secrets::wallet::wallet()
{
}

secrets::wallet::wallet( LXQt::Wallet::Wallet * w ) : m_wallet( w )
{
}

secrets::wallet::~wallet()
{
}

secrets::wallet::wallet( secrets::wallet&& w )
{
	m_wallet = w.m_wallet ;
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

	}else if( s == LXQt::Wallet::BackEnd::windows_dpapi ){

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

secrets::backends::backends( QWidget * w ) : m_parent( w )
{
}

LXQt::Wallet::Wallet * secrets::backends::get( LXQt::Wallet::BackEnd e )
{
	for( const auto& it : m_backends ){

		if( it.bk == e ){

			return it.wallet ;
		}
	}

	auto a = LXQt::Wallet::getWalletBackend( e ).release() ;

	a->setParent( m_parent ) ;

	m_backends.emplace_back( e,a ) ;

	return a ;
}

void secrets::backends::close()
{
	for( auto& it : m_backends ){

		delete it.wallet ;
	}

	m_backends.clear() ;
}
