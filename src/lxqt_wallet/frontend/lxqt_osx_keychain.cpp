
/*
 * copyright: 2017
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lxqt_osx_keychain.h"

void LXQt::Wallet::osxKeyChain::open( const QString& walletName,
				      const QString& applicationName,
				      std::function< void( bool ) > function,
				      QWidget * widget,
				      const QString& password,
				      const QString& displayApplicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	Q_UNUSED( function ) ;
	Q_UNUSED( widget ) ;
	Q_UNUSED( password ) ;
	Q_UNUSED( displayApplicationName ) ;
}

LXQt::Wallet::osxKeyChain::osxKeyChain()
{
}

LXQt::Wallet::osxKeyChain::~osxKeyChain()
{
}

bool LXQt::Wallet::osxKeyChain::open( const QString& walletName,
				      const QString& applicationName,
				      QWidget * widget,
				      const QString& password,
				      const QString& displayApplicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	Q_UNUSED( widget ) ;
	Q_UNUSED( password ) ;
	Q_UNUSED( displayApplicationName ) ;

	return false ;
}

bool LXQt::Wallet::osxKeyChain::addKey( const QString& key,const QByteArray& value )
{
	Q_UNUSED( key ) ;
	Q_UNUSED( value ) ;

	return false ;
}

bool LXQt::Wallet::osxKeyChain::opened()
{
	return false ;
}

QByteArray LXQt::Wallet::osxKeyChain::readValue( const QString& key )
{
	Q_UNUSED( key ) ;
	return QByteArray() ;
}

QVector< std::pair< QString,QByteArray > > LXQt::Wallet::osxKeyChain::readAllKeyValues()
{
	return QVector< std::pair< QString,QByteArray > >() ;
}

QStringList LXQt::Wallet::osxKeyChain::readAllKeys()
{
	return QStringList() ;
}

QStringList LXQt::Wallet::osxKeyChain::managedWalletList()
{
	return QStringList() ;
}

QString LXQt::Wallet::osxKeyChain::storagePath()
{
	return QString() ;
}

QString LXQt::Wallet::osxKeyChain::localDefaultWalletName()
{
	return QString() ;
}

QString LXQt::Wallet::osxKeyChain::networkDefaultWalletName()
{
	return QString() ;
}

void LXQt::Wallet::osxKeyChain::deleteKey( const QString& key )
{
	Q_UNUSED( key ) ;
}

void LXQt::Wallet::osxKeyChain::closeWallet( bool e )
{
	Q_UNUSED( e ) ;
}

void LXQt::Wallet::osxKeyChain::changeWalletPassWord( const QString& walletName,
						      const QString& applicationName,
						      std::function< void( bool )> function )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	Q_UNUSED( function ) ;
}

void LXQt::Wallet::osxKeyChain::setImage( const QIcon& e )
{
	Q_UNUSED( e ) ;
}

int LXQt::Wallet::osxKeyChain::walletSize()
{
	return 0 ;
}

LXQt::Wallet::BackEnd LXQt::Wallet::osxKeyChain::backEnd()
{
	return LXQt::Wallet::BackEnd::osxkeychain ;
}

QObject * LXQt::Wallet::osxKeyChain::qObject()
{
	return nullptr ;
}
