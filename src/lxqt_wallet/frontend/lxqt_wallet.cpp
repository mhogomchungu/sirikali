/*
 * copyright: 2013-2015
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

#include "lxqt_wallet.h"
#include "lxqt_internal_wallet.h"
#include "../backend/lxqtwallet.h"

#include "storage_manager.h"

#if HAS_KWALLET_SUPPORT
#include "lxqt_kwallet.h"
#endif

#if HAS_SECRET_SUPPORT
#include "lxqt_libsecret.h"
#endif

LXQt::Wallet::Wallet::Wallet()
{
}

LXQt::Wallet::Wallet::~Wallet()
{
}

LXQt::Wallet::Wallet * LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd bk )
{
	if( bk == LXQt::Wallet::BackEnd::internal ){

		return new LXQt::Wallet::internalWallet() ;
	}

	if( bk == LXQt::Wallet::BackEnd::kwallet ){
		#if HAS_KWALLET_SUPPORT
			return new LXQt::Wallet::kwallet() ;
		#else
			return nullptr ;
		#endif
	}

	if( bk == LXQt::Wallet::BackEnd::libsecret ){
		#if HAS_SECRET_SUPPORT
			return new LXQt::Wallet::libsecret() ;
		#else
			return nullptr ;
		#endif
	}

	return nullptr ;
}

bool LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd bk )
{
	if( bk == LXQt::Wallet::BackEnd::internal ){

		return true ;
	}

	if( bk == LXQt::Wallet::BackEnd::kwallet ){

		return HAS_KWALLET_SUPPORT ;
	}

	if( bk == LXQt::Wallet::BackEnd::libsecret ){

		return HAS_SECRET_SUPPORT ;
	}

	return false ;
}

bool LXQt::Wallet::deleteWallet( LXQt::Wallet::BackEnd bk,
				 const QString& walletName,
				 const QString& applicationName )
{
	QString appName ;

	if( applicationName.isEmpty() ){

		appName = walletName ;
	}else{
		appName = applicationName ;
	}

	if( bk == LXQt::Wallet::BackEnd::internal ){

		auto e = lxqt_wallet_delete_wallet( walletName.toLatin1().constData(),
						    appName.toLatin1().constData() ) ;

		return e == lxqt_wallet_no_error ;
	}

	if( bk == LXQt::Wallet::BackEnd::kwallet ){
		#if HAS_KWALLET_SUPPORT
			return KWallet::Wallet::deleteWallet( walletName ) == 0 ;
		#else
			return false ;
		#endif
	}

	if( bk == LXQt::Wallet::BackEnd::libsecret ){

		return false ;
	}

	return false ;
}

bool LXQt::Wallet::walletExists( LXQt::Wallet::BackEnd bk,
				 const QString& walletName,
				 const QString& applicationName )
{
	QString appName ;

	if( applicationName.isEmpty() ){

		appName = walletName ;
	}else{
		appName = applicationName ;
	}

	if( bk == LXQt::Wallet::BackEnd::internal ){

		return lxqt_wallet_exists( walletName.toLatin1().constData(),
					   appName.toLatin1().constData() ) == 0 ;
	}

	if( bk == LXQt::Wallet::BackEnd::kwallet ){
		#if HAS_KWALLET_SUPPORT
			return !KWallet::Wallet::folderDoesNotExist( walletName,appName ) ;
		#else
			return false ;
		#endif
	}

	if( bk == LXQt::Wallet::BackEnd::libsecret ){

		return false ;
	}

	return false ;
}

QStringList LXQt::Wallet::walletList( LXQt::Wallet::BackEnd bk )
{
	if( bk == LXQt::Wallet::BackEnd::internal ){

		char path[ 4096 ] ;

		lxqt_wallet_application_wallet_path( path,4096,"" ) ;

		QDir d( path ) ;

		auto l = d.entryList() ;

		l.removeOne( "." ) ;
		l.removeOne( ".." ) ;

		return l ;

	}else if( bk == LXQt::Wallet::BackEnd::kwallet ){

		#if HAS_KWALLET_SUPPORT
			return KWallet::Wallet::walletList() ;
		#else
			return QStringList() ;
		#endif

	}else if( bk == LXQt::Wallet::BackEnd::libsecret ){

		return QStringList() ;
	}else{
		return QStringList() ;
	}
}
