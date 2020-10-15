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

#ifndef LXQT_KWALLET_DBUS_H
#define LXQT_KWALLET_DBUS_H

#include "lxqt_wallet.h"

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QtDBus>

class QWidget;

namespace LXQt
{

namespace Wallet
{

class kwallet_dbus : public LXQt::Wallet::Wallet
{
    Q_OBJECT
public:
    static bool has_functionality();

    kwallet_dbus();
    ~kwallet_dbus();

    void open(const QString &walletName,
              const QString &applicationName,
              std::function< void(bool) >,
              QWidget * = nullptr,
              const QString &password = QString(),
              const QString &displayApplicationName = QString());

    bool open(const QString &walletName,
	      const QString &applicationName,
	      QWidget * = nullptr,
	      const QString &password = QString(),
	      const QString &displayApplicationName = QString());

    bool addKey(const QString &key, const QByteArray &value);
    bool opened(void);

    QByteArray readValue(const QString &key);

    QVector<std::pair<QString, QByteArray>> readAllKeyValues(void);

    QStringList readAllKeys(void);
    QStringList managedWalletList(void);

    QString storagePath(void);
    QString localDefaultWalletName(void);
    QString networkDefaultWalletName(void);

    void deleteKey(const QString &key);
    void closeWallet(bool);
    void changeWalletPassWord(const QString &walletName,
                              const QString &applicationName = QString(),
			      std::function<void(bool)> = [](bool e) { Q_UNUSED(e) });
    void setImage(const QIcon &);

    int walletSize(void) ;

    void log(std::function<void(QString)>);

    LXQt::Wallet::BackEnd backEnd(void);
    QObject *qObject(void);

private slots:
    void walletOpened(bool);
private:
    void openedWallet(bool);

    int m_handle = -1;
    QString m_walletName;
    QString m_applicationName;
    QString m_password;
    QString m_folder;
    QDBusInterface m_dbus;
    std::function<void(bool)> m_walletOpened = [](bool e) { Q_UNUSED(e) };
    std::function<void(QString)> m_log;
};

}

}

#endif // LXQT_KWALLET_DBUS_H
