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

#ifndef LXQT_INTERNAL_WALLET_H
#define LXQT_INTERNAL_WALLET_H

#include "lxqt_wallet.h"
#include "../backend/lxqtwallet.h"
#include "password_dialog.h"
#include "task.h"
#include "changepassworddialog.h"

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include "password_dialog.h"
#include <QPushButton>
#include <QDir>
#include <QEventLoop>

#include <functional>
#include <utility>

class QWidget ;

namespace LXQt
{

namespace Wallet
{

class internalWallet : public LXQt::Wallet::Wallet
{
public:
    internalWallet() ;
    ~internalWallet() ;

    void open(const QString &walletName,
              const QString &applicationName,
              std::function< void(bool) >,
              QWidget * = nullptr,
              const QString &password = QString(),
              const QString &displayApplicationName = QString()) ;

    bool open(const QString &walletName,
              const QString &applicationName,
              QWidget * = nullptr,
              const QString &password = QString(),
              const QString &displayApplicationName = QString()) ;

    bool addKey(const QString &key, const QByteArray &value) ;
    bool opened(void) ;

    QByteArray readValue(const QString &key) ;

    QVector< std::pair< QString, QByteArray > > readAllKeyValues(void) ;

    QStringList readAllKeys(void) ;
    QStringList managedWalletList(void) ;

    QString storagePath(void) ;
    QString localDefaultWalletName(void) ;
    QString networkDefaultWalletName(void) ;

    void log(std::function<void(QString)>);

    void deleteKey(const QString &key) ;
    void closeWallet(bool) ;
    void changeWalletPassWord(const QString &walletName,
                              const QString &applicationName = QString(),
    std::function< void(bool) > = [](bool e) { Q_UNUSED(e) }) ;
    void setImage(const QIcon &) ;

    int walletSize(void)  ;

    LXQt::Wallet::BackEnd backEnd(void) ;
    QObject *qObject(void) ;
private:
    void walletIsOpen(bool) ;

    void openWallet(QString) ;
    void createWallet(void) ;
    void openWallet() ;
    void opened(bool) ;

    lxqt_wallet_t m_wallet ;

    QString m_walletName ;
    QString m_applicationName ;
    QString m_displayApplicationName ;
    QString m_password ;
    QEventLoop m_loop ;
    bool m_opened ;

    std::function< void(bool) > m_correctPassword = [](bool e) { Q_UNUSED(e) } ;
    std::function< void(bool) > m_walletOpened = [](bool e) { Q_UNUSED(e) } ;
};

}

}
#endif // LXQT_INTERNAL_WALLET_H
