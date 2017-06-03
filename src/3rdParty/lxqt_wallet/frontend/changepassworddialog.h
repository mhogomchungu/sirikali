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

#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QString>
#include <QDebug>
#include <QCloseEvent>
#include <QEvent>
#include <QKeyEvent>

#include "../backend/lxqtwallet.h"
#include "task.h"

#include <functional>

namespace Ui {
class changePassWordDialog;
}

namespace LXQt{

namespace Wallet{

class changePassWordDialog : public QDialog
{
	Q_OBJECT
public:
	static changePassWordDialog& instance( QWidget * parent,
					       const QString& walletName,
					       const QString& applicationName,
					       std::function< void( const QString&,bool ) >&& function )
	{
		auto& e = *( new changePassWordDialog( parent,walletName,applicationName ) ) ;

		e.ShowUI( std::move( function ) ) ;

		return e ;
	}
	static changePassWordDialog& instance_1( QWidget * parent,
					       const QString& walletName,
					       const QString& applicationName,
					       std::function< void( bool ) >&& function )
	{
		auto& e = *( new changePassWordDialog( parent,walletName,applicationName ) ) ;

		e.ShowUI_1( std::move( function ) ) ;

		return e ;
	}
	explicit changePassWordDialog( QWidget * parent = 0,const QString& walletName = QString(),const QString& applicationName = QString() ) ;
	void ShowUI( std::function< void( const QString&,bool ) >&& ) ;
	void ShowUI_1( std::function< void( bool ) >&& ) ;

	~changePassWordDialog() ;
signals:
	void walletpassWordChanged( bool ) ;
private slots:
	void create( void ) ;
	void change( void ) ;
	void cancel( void ) ;
	void ok( void ) ;
	void ok_1( void ) ;
private:
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	bool eventFilter ( QObject * watched,QEvent * event ) ;
	Ui::changePassWordDialog * m_ui ;
	lxqt_wallet_t m_wallet ;
	QString m_walletName ;
	QString m_applicationName ;
	QString m_banner ;
	bool m_walletPassWordChanged ;

	std::function< void( const QString&,bool ) > m_create = []( const QString& e,bool f ){

		Q_UNUSED( e ) ;
		Q_UNUSED( f ) ;
	} ;

	std::function< void( bool ) > m_change = []( bool e ){ Q_UNUSED( e ) ; } ;
};

}

}

#endif // CHANGEPASSWORDDIALOG_H
