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

void secrets::changeInternalWalletPassword( QWidget * parent,
					    const QString& walletName,
					    const QString& appName,
					    std::function< void( bool ) > ff )
{
	auto e = m_backends.get( LXQt::Wallet::BackEnd::internal ) ;

	e->setParent( parent ) ;

	e->changeWalletPassWord( walletName,appName,[ ff = std::move( ff ) ]( bool q ){

		ff( q ) ;
	} ) ;
}

void secrets::changeWindowsDPAPIWalletPassword( QWidget * parent,
						const QString& walletName,
						const QString& appName,
						std::function< void( bool ) > f )
{
	auto s = m_backends.get( LXQt::Wallet::BackEnd::windows_dpapi ) ;

	s->setParent( parent ) ;

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

secrets::secrets()
{
}

LXQt::Wallet::Wallet * secrets::walletBk( LXQt::Wallet::BackEnd e ) const
{
	return m_backends.get( e ) ;
}

secrets::wallet::wallet( LXQt::Wallet::Wallet * w ) : m_wallet( w )
{
}

secrets::wallet::~wallet()
{
}

QWidget * secrets::wallet::setParent()
{
	return m_parent ;
}

secrets::wallet::info secrets::wallet::walletInfo()
{
	auto& e = settings::instance() ;

	return { e.walletName( m_wallet->backEnd() ),e.applicationName() } ;
}

secrets::backends::backends()
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

	m_backends.emplace_back( e,a ) ;

	return m_backends.back().wallet ;
}

void secrets::backends::close()
{
	for( auto& it : m_backends ){

		delete it.wallet ;
	}

	m_backends.clear() ;
}
