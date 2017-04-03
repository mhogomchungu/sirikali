
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
#include "osx_keychain.h"

/*
 * https://developer.apple.com/library/content/documentation/Security/Conceptual/keychainServConcepts/03tasks/tasks.html#//apple_ref/doc/uid/TP30000897-CH205-TP9
 *
 */

#if OSX_KEYCHAIN

#include <CoreFoundation/CoreFoundation.h>

#include <Security/Security.h>

static char * WALLET_KEYS = "lxqt.Wallet.WalletKeys" ;

void LXQt::Wallet::osxKeyChain::open( const QString& walletName,
				      const QString& applicationName,
				      std::function< void( bool ) > function,
				      QWidget * widget,
				      const QString& password,
				      const QString& displayApplicationName )
{
	m_walletName = "lxqt.Wallet." + walletName.toLatin1() + "." + applicationName.toLatin1() ;

	function( true ) ;

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
	m_walletName = "lxqt.Wallet." + walletName.toLatin1() + "." + applicationName.toLatin1() ;

	Q_UNUSED( widget ) ;
	Q_UNUSED( password ) ;
	Q_UNUSED( displayApplicationName ) ;

	return true ;
}

static void _delete_key( const QString& key )
{
	void * data = nullptr ;
	quint32 len = 0;
	SecKeychainItemRef ref = 0 ;

	auto status = SecKeychainFindGenericPassword( nullptr,
						      m_walletName.size(),
						      m_walletName.constData(),
						      key.size(),
						      key.toLatin1().constData(),
						      &len,
						      &data,
						      ref ) ;
	if( status == noErr ){

		SecKeychainItemDelete( ref ) ;
	}

	if( ref ){

		CFRelease( ref ) ;
	}
}

static void _add_key( const QString& key,const QByteArray& value )
{
	auto status = SecKeychainAddGenericPassword( nullptr,
						     m_walletName.size(),
						     m_walletName.constData(),
						     key.size(),
						     key.toLatin1().constData(),
						     value.size(),
						     value.constData(),
						     nullptr ) ;

	return status == noErr ;
}

static void _update_wallet_keys( const QStringList& s )
{
	_delete_key( WALLET_KEYS ) ;
	_add_key( WALLET_KEYS,e.join( "\n" ) ) ;
}

void LXQt::Wallet::osxKeyChain::deleteKey( const QString& key )
{
	_delete_key( key ) ;

	QString s = this->readValue( WALLET_KEYS ) ;

	auto e = s.split( '\n',QString::SkipEmptyParts ) ;

	e.removeOne( key ) ;

	_update_wallet_keys( e ) ;
}

bool LXQt::Wallet::osxKeyChain::addKey( const QString& key,const QByteArray& value )
{	
	if( _add_key( key,value ) ){

		QString s = this->readValue( WALLET_KEYS ) ;

		s += "\n" + key ;

		_update_wallet_keys( s.split( "\n",QString::SkipEmptyParts ) ) ;

		return true ;
	}else{
		return false ;
	}
}

bool LXQt::Wallet::osxKeyChain::opened()
{
	return true ;
}

QByteArray LXQt::Wallet::osxKeyChain::readValue( const QString& key )
{
	SecKeychainItemRef ref = 0 ;
	void * data = nullptr ;
	quint32 len = 0 ;

	SecKeychainFindGenericPassword( nullptr,
					m_walletName.size(),
					m_walletName.constData(),
					key.size(),
					key.toLatin1().constData(),
					&len,
					&data,
					&ref ) ;

	auto e = reinterpret_cast< const char * >( data ) ;
	auto s = static_cast< int >( len ) ;

	SecKeychainItemFreeContent( 0,data ) ;

	if( ref ){

		CFRelease( ref ) ;
	}

	return QByteArray( e,s ) ;
}

QVector< std::pair< QString,QByteArray > > LXQt::Wallet::osxKeyChain::readAllKeyValues()
{
	QVector< std::pair< QString,QByteArray > > e ;

	for( const auto& it : this->readAllKeys() ){

		e.append( { it,this->readValue( it ) } ) ;
	}

	return e ;
}

QStringList LXQt::Wallet::osxKeyChain::readAllKeys()
{
	QString s = this->readValue( WALLET_KEYS ) ;

	return s.split( '\n',QString::SkipEmptyParts ) ;
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
	return this->readAllKeys().size() ;
}

LXQt::Wallet::BackEnd LXQt::Wallet::osxKeyChain::backEnd()
{
	return LXQt::Wallet::BackEnd::osxkeychain ;
}

QObject * LXQt::Wallet::osxKeyChain::qObject()
{
	return nullptr ;
}

#else

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

#endif
