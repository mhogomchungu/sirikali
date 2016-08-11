/*
 * copyright: 2013-2016
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

#ifndef LXQT_WALLET_INTERFACE_H
#define LXQT_WALLET_INTERFACE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QWidget>
#include <QByteArray>
#include <QVector>
#include <QStringList>
#include <QDir>

#include <functional>

namespace LXQt{

namespace Wallet{

enum class BackEnd{ internal,kwallet,libsecret } ;

/*
 * Forward declare the Wallet class
 */
class Wallet ;

/*
 * Check if there is a support for a backend and return true if the back end is supported.
 */
Q_DECL_EXPORT bool backEndIsSupported( LXQt::Wallet::BackEnd ) ;

/*
 * Delete a wallet.
 * KWallet backend does not use the applicationName argument.
 */
Q_DECL_EXPORT bool deleteWallet( LXQt::Wallet::BackEnd,
				 const QString& walletName,
				 const QString& applicationName = QString() ) ;

/*
 * Check if a particular wallet exists.
 */
Q_DECL_EXPORT bool walletExists( LXQt::Wallet::BackEnd,
				 const QString& walletName,
				 const QString& applicationName = QString() ) ;

/*
 * Get a pointer to a requested backend to be used to gain access to the API.It is advised to call
 * backEndIsSupported() to check if a backed is supported before calling this function.
 *
 * nullptr is returned if there is no support for requested backend.
 * A caller is responsible for the returned object and must delete it when done with it.
 */
Q_DECL_EXPORT LXQt::Wallet::Wallet * getWalletBackend( LXQt::Wallet::BackEnd ) ;

/*
 * Return a list of all wallets.
 * Returned value is undefined if the backend is not supported.
 */
QStringList walletList( LXQt::Wallet::BackEnd ) ;

/*
 * Below class is the interface that implements various backends.
 * See example at the end of this header file to see an example of how to use the interface.
 */

class Q_DECL_EXPORT Wallet : public QWidget
{
public:
	Wallet() ;
	virtual ~Wallet() ;

	/*
	 * Add an entry to the wallet.
	 */
	virtual bool addKey( const QString& key,const QByteArray& value ) = 0 ;

	/*
	 * overloaded method to add a key to the wallet.
	 */
	bool addKey( const QString& key,const QString& value )
	{
		return this->addKey( key,value.toLatin1() ) ;
	}

	/*
	 * Get a value through a key.
	 */
	virtual QByteArray readValue( const QString& key ) = 0 ;

	/*
	 * Get all keys and their respective values from the wallet.
	 * First argument of std::pair is the key.
	 * Second argument of std::pair is the value.
	 */
	virtual QVector< std::pair< QString,QByteArray > > readAllKeyValues() = 0 ;

	/*
	 * Get all keys in the wallet.
	 */
	virtual QStringList readAllKeys() = 0 ;

	/*
	 * Delete a key in a wallet.
	 */
	virtual void deleteKey( const QString& key ) = 0 ;

	/*
	 * Return the number of entries in the wallet.
	 */
	virtual int walletSize() = 0 ;

	/*
	 * Close the wallet.
	 */
	virtual void closeWallet( bool option = false ) = 0 ;

	/*
	 * Return the backend in use.
	 */
	virtual LXQt::Wallet::BackEnd backEnd() = 0 ;

	/*
	 * Check if a wallet is opened or not.
	 * If the wallet is not open,libsecret backend will block while the user is
	 * prompted for a key to inlock it.
	 */
	virtual bool opened() = 0 ;

	/*
	 * Return QObject pointer of the backend,not sure why you would want this.
	 */
	virtual QObject * qObject() = 0 ;

	/*
	 * First open() will return immediately and the status of the unlocking attempt will be returned
	 * through the passed in lambda( third argument ).The lambda will be called with "true" if
	 * the attempt was successful and with "false" otherwise.
	 *
	 * Second open() will block the call waiting for the result of the unlocking attempt.
	 * "true" will be returned if the attempt was successful and "false" will be returned otherwise.
	 * The blocking will be done in a way that does not hang the GUI.
	 *
	 * Optional arguments are not necessary in all backends:
	 *
	 * libsecret backend:
	 * "password" argument is ignored.
	 * "applicationName" argument is ignored.
	 * "displayApplicationName" argument is ignored.
	 * "widget" argument is ignored.
	 *
	 * kwallet backend:
	 * "password" argument is ignored.
	 * "walletName" argument corresponds to the same name in KWAllet API.
	 * "displayApplicationName" is ignored.
	 * "widget" argument is ignored.
	 * "applicationName" argument corresponds to password folder in KWallet API,default value will
	 *  set passwordFolder to KDE's default. Password argument is not used.
	 *
	 * internal:
	 * "walletName" argument is the name of the wallet to open.
	 * "applicationName" argument is the name of the program that owns the wallet.
	 * "displayApplicationName" argument will be used to show the name of the application that is
	 *  opening the wallet if a GUI prompt is generated.
	 * "widget" argument is necessary to give generated GUI prompts a parent object.
	 *
	 * If "password" argument is given,it will be used to unlock the wallet.
	 * If "password" argument is not given,a GUI window will be generated to ask the user for the password.
	 *
	 */
	virtual void open( const QString& walletName,
			   const QString& applicationName,
			   std::function< void( bool ) >,
			   QWidget * = nullptr,
			   const QString& password = QString(),
			   const QString& displayApplicationName = QString() ) = 0 ;

	virtual bool open( const QString& walletName,
			   const QString& applicationName,
			   QWidget * = nullptr,
			   const QString& password = QString(),
			   const QString& displayApplicationName = QString() ) = 0 ;

	/*
	 * This method is defined only with internal backend.
	 * This method is used to set an icon image to be used when the backend produces GUI windows.
	 */
	virtual void setImage( const QIcon& ) = 0 ;

	/*
	 * This method returns PasswordFolder() in kwallet backend and is undefined in other backends.
	 */
	virtual QString storagePath() = 0 ;

	/*
	 * Change the wallet key.
	 *
	 * The passed in lamba will be called with internal wallet with "true" if the password was
	 * successfully changed and with "false" otherwise.
	 *
	 * The passed in lambda will not be called with kwallet backend.
	 *
	 * This method is undefined in libsecret backend.
	 */
	virtual void changeWalletPassWord( const QString& walletName,
					   const QString& applicationName = QString(),
					   std::function< void( bool ) > = []( bool e ){ Q_UNUSED( e ) ; } ) = 0 ;

	/*
	 * List all wallets managed by an application.
	 *
	 * This method is equivalent to kwallet's "folderList()".
	 *
	 * This method is undefined in libsecret backend.
	 */
	virtual QStringList managedWalletList() = 0 ;

	/*
	 * This medhod is defined only with kwallet backend and it returns the name of the kwallet default local
	 * wallet name.
	 */
	virtual QString localDefaultWalletName() = 0 ;

	/*
	 * This medhod is defined only with kwallet backend and it returns the name of the kwallet default
	 * network wallet name.
	 */
	virtual QString networkDefaultWalletName() = 0 ;
};

} // namespace lxqt

} // namespace wallet

#endif // LXQT_WALLET_INTERFACE_H

/*
 * An example use of the API that opens a wallet and then prints all keys and their respective values
 */

#if 0

void TestClass::testWallet()
{
	/*
	 * open a default backend( internal one )
	 */
	m_wallet = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::internal ) ;

	/*
	 * Open the wallet and read its contents
	 */
	m_wallet->open( "test_wallet_name","test_application_name",[ this ]( bool walletIsOpen ){

		if( walletIsOpen ){

			for( const auto& it : m_wallet->readAllKeyValues() ){

				const auto& key   = it.first ;
				const auto& value = it.second ;

				qDebug() << "key=" << key << " : value=" << value ;
			}
		}else{
			qDebug() << "failed to open wallet" ;
		}

		/*
		 * Delete the wallet object when done with it
		 */
		m_wallet->deleteLater() ;

	},this ) ;
}

#endif

