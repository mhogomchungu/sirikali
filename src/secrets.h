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
#include "task.h"

#include <QWidget>

class secrets
{
public:
	class wallet
	{
	public:
		wallet( LXQt::Wallet::Wallet * ) ;
		wallet() ;

		~wallet() ;

		wallet( wallet&& ) ;

		secrets::wallet& operator=( secrets::wallet&& ) ;

		LXQt::Wallet::Wallet& bk()
		{
			return *m_wallet ;
		}

		LXQt::Wallet::Wallet * operator->()
		{
			return m_wallet ;
		}
	private:
		LXQt::Wallet::Wallet * m_wallet = nullptr ;
	};

	secrets::wallet walletBk( LXQt::Wallet::BackEnd ) ;

	QWidget * parent() ;

	void changeInternalWalletPassword( const QString&,const QString& ) ;
	void setParent( QWidget * ) ;

	secrets( QWidget * parent = nullptr ) ;
	secrets( const secrets& ) = delete ;

	secrets& operator=( const secrets& ) = delete ;

	~secrets() ;
private:
	LXQt::Wallet::Wallet * internalWallet() ;
	QWidget * m_parent = nullptr ;
	LXQt::Wallet::Wallet * m_internalWallet = nullptr ;
};

#endif
