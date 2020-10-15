/*
 * copyright: 2020
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

#include "lxqt_kwallet-dbus.h"
#include "task.h"

static auto _service   = "org.kde.kwalletd5";
static auto _path      = "/modules/kwalletd5";
static auto _interface = "org.kde.KWallet";

LXQt::Wallet::kwallet_dbus::kwallet_dbus() : m_dbus(_service, _path, _interface)
{
}

LXQt::Wallet::kwallet_dbus::~kwallet_dbus()
{
}

void LXQt::Wallet::kwallet_dbus::setImage(const QIcon &image)
{
    Q_UNUSED(image)
}

bool LXQt::Wallet::kwallet_dbus::addKey(const QString &key, const QByteArray &value)
{
    QDBusReply<int> m = m_dbus.call("writePassword", m_handle, m_folder, key, QString(value), m_applicationName);
    if (m.isValid()){
        return m.value() == 0;
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: writePassword dbus call failed: " + m.error().message());
        return false;
    }
}

bool LXQt::Wallet::kwallet_dbus::open(const QString &walletName,
				 const QString &applicationName,
				 QWidget *parent,
				 const QString &password,
				 const QString &displayApplicationName)
{
    QEventLoop loop;
    bool opened;
    this->open(walletName,
                applicationName,
                [&](bool e) {opened = e;loop.exit();},
                parent,
                password,
                displayApplicationName);
    loop.exec();
    return opened;
}

void LXQt::Wallet::kwallet_dbus::openedWallet(bool e)
{
    m_log("LXQt::Wallet::kwallet_dbus: kwallet support is provided through dbus API");

    if (e)
    {
        if (m_applicationName.isEmpty())
        {
            m_folder = "Passwords";
        }
        else
        {
            m_folder = m_applicationName;
        }
        QDBusReply<bool> m = m_dbus.call("createFolder", m_handle, m_folder, m_applicationName);
        if (!m.isValid()){
            m_log("LXQt::Wallet::kwallet_dbus: createFolder dbus call failed: " + m.error().message());
        }
    }

    m_walletOpened(e);
}

void LXQt::Wallet::kwallet_dbus::open(const QString &walletName,
                                 const QString &applicationName,
                                 std::function< void(bool) > function,
                                 QWidget *parent,
                                 const QString &password,
                                 const QString &displayApplicationName)
{
    if (walletName == "default")
    {
        m_walletName = Task::await<QString>([this](){QDBusReply<QString> a = m_dbus.call("localWallet");return a.value();});
    }
    else
    {
        m_walletName = walletName;
    }

    this->setParent(parent);

    m_applicationName = applicationName;
    m_password        = password;

    m_walletOpened = std::move(function);

    Q_UNUSED(displayApplicationName)

    struct result{bool success;int handle;};

    Task::run<result>([this](){
        qlonglong wid = 0;
        QDBusReply<int> m = m_dbus.call("open", m_walletName ,wid , m_applicationName);
        if (m.isValid()){
            return result{true,m.value()};
        }else{
            m_log("LXQt::Wallet::kwallet_dbus: open dbus call failed: " + m.error().message());
            return result{false,-1};
	}
    }).then([this](result r){
        if (r.success){
            m_handle = r.handle;
            if (m_handle == -1){
                QMetaObject::invokeMethod(this, "walletOpened", Qt::QueuedConnection ,Q_ARG(bool, false));
                m_log("LXQt::Wallet::kwallet_dbus: Failed to get a handle to kwallet, is it enabled?");
            }else{
                QMetaObject::invokeMethod(this, "walletOpened", Qt::QueuedConnection, Q_ARG(bool, true));
            }
        }else{
            QMetaObject::invokeMethod(this, "walletOpened", Qt::QueuedConnection, Q_ARG(bool, false));
        }
    });
}

void LXQt::Wallet::kwallet_dbus::walletOpened(bool opened)
{
    this->openedWallet(opened);
}

QByteArray LXQt::Wallet::kwallet_dbus::readValue(const QString &key)
{
    QDBusReply<QString> m = m_dbus.call("readPassword", m_handle, m_folder, key, m_applicationName) ;

    if (m.isValid()){
        return m.value().toLatin1();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: readPassword dbus call failed: " + m.error().message());
        return {};
    }
}

QVector<std::pair<QString, QByteArray>> LXQt::Wallet::kwallet_dbus::readAllKeyValues(void)
{
    QVector<std::pair<QString, QByteArray>> p;

    auto l = this->readAllKeys();

    auto j = l.size();

    for (int i = 0; i < j; i++)
    {
        const auto &e = l.at(i);

        p.append({ e, this->readValue(e) });
    }

    return p;
}

void LXQt::Wallet::kwallet_dbus::log(std::function<void(QString)> f)
{
    m_log = std::move(f);
}

QStringList LXQt::Wallet::kwallet_dbus::readAllKeys(void)
{
    QDBusReply<QStringList> m = m_dbus.call("entryList", m_handle, m_folder, m_applicationName);

    if (m.isValid()){
        return m.value();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: EntryList dbus call failed: " + m.error().message());
        return {};
    }
}

void LXQt::Wallet::kwallet_dbus::deleteKey(const QString &key)
{
    QDBusReply<int> m = m_dbus.call("removeEntry", m_handle, m_folder, key, m_applicationName);
    if (!m.isValid()){
        m_log("LXQt::Wallet::kwallet_dbus: removeEntry dbus call failed: " + m.error().message());
    }
}

int LXQt::Wallet::kwallet_dbus::walletSize(void)
{
    return this->readAllKeys().size();
}

void LXQt::Wallet::kwallet_dbus::closeWallet(bool b)
{
    QDBusReply<bool> m = m_dbus.call("close", m_handle, b, m_applicationName);
    if (!m.isValid()){
        m_log("LXQt::Wallet::kwallet_dbus: close dbus call failed: " + m.error().message());
    }
}

LXQt::Wallet::BackEnd LXQt::Wallet::kwallet_dbus::backEnd(void)
{
    return LXQt::Wallet::BackEnd::kwallet;
}

bool LXQt::Wallet::kwallet_dbus::opened(void)
{
    if (m_handle == -1){
        return false;
    }

    QDBusReply<bool> m = m_dbus.call("isOpen",m_handle);
    if (m.isValid()){
        return m.value();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: isOpen dbus call failed: " + m.error().message());
        return false;
    }
}

QObject *LXQt::Wallet::kwallet_dbus::qObject(void)
{
    return this;
}

QString LXQt::Wallet::kwallet_dbus::storagePath()
{
    return {};
}

void LXQt::Wallet::kwallet_dbus::changeWalletPassWord(const QString &walletName,
        const QString &applicationName,
        std::function< void(bool) > function)
{
    Q_UNUSED(applicationName)
    Q_UNUSED(function)
    qlonglong wId = 0;
    QDBusReply<bool> m = m_dbus.call("changePassword",walletName, wId, applicationName);
    if (!m.isValid()){
        m_log("LXQt::Wallet::kwallet_dbus: changePassword dbus call failed: " + m.error().message());
    }
}

QStringList LXQt::Wallet::kwallet_dbus::managedWalletList()
{
    QDBusReply<QStringList> m = m_dbus.call("wallets");
    if(m.isValid()){
        return m.value();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: wallets dbus call failed: " + m.error().message());
        return {};
    }
}

QString LXQt::Wallet::kwallet_dbus::localDefaultWalletName()
{
    QDBusReply<QString> m = m_dbus.call("localWallet");
    if(m.isValid()){
        return m.value();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: localWallet dbus call failed: " + m.error().message());
        return {};
    }
}

QString LXQt::Wallet::kwallet_dbus::networkDefaultWalletName()
{
    QDBusReply<QString> m = m_dbus.call("networkWallet");
    if(m.isValid()){
        return m.value();
    }else{
        m_log("LXQt::Wallet::kwallet_dbus: networkWallet dbus call failed: " + m.error().message());
        return {};
    }
}

bool LXQt::Wallet::kwallet_dbus::has_functionality()
{
    QDBusInterface m(_service, _path, _interface, QDBusConnection::sessionBus());
    m.setTimeout(500);
    QDBusMessage reply = m.call("localWallet");
    return reply.type() == QDBusMessage::ReplyMessage;
}
