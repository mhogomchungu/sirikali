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

#include "lxqt_kwallet.h"
#include "task.h"

LXQt::Wallet::kwallet::kwallet() : m_kwallet(nullptr)
{
}

LXQt::Wallet::kwallet::~kwallet()
{
    if (m_kwallet)
    {
        m_kwallet->sync();
        m_kwallet->deleteLater();
    }
}

void LXQt::Wallet::kwallet::setImage(const QIcon &image)
{
    Q_UNUSED(image)
}

bool LXQt::Wallet::kwallet::addKey(const QString &key, const QByteArray &value)
{
    return m_kwallet->writePassword(key, value) == 0;
}


bool LXQt::Wallet::kwallet::open(const QString &walletName,
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

void LXQt::Wallet::kwallet::openedWallet(bool e)
{
    if (e)
    {
        if (m_applicationName.isEmpty())
        {
            m_kwallet->createFolder(m_kwallet->PasswordFolder());
            m_kwallet->setFolder(m_kwallet->PasswordFolder());
        }
        else
        {
            m_kwallet->createFolder(m_applicationName);
            m_kwallet->setFolder(m_applicationName);
        }
    }

    m_walletOpened(e);
}

void LXQt::Wallet::kwallet::open(const QString &walletName,
                                 const QString &applicationName,
                                 std::function< void(bool) > function,
                                 QWidget *parent,
                                 const QString &password,
                                 const QString &displayApplicationName)
{
    if (walletName == "default")
    {
        m_walletName = KWallet::Wallet::LocalWallet();
    }
    else
    {
        m_walletName = walletName;
    }

    this->setParent(parent);

    m_applicationName   = applicationName;
    m_password          = password;

    m_walletOpened = std::move(function);

    Q_UNUSED(displayApplicationName)

    m_kwallet = KWallet::Wallet::openWallet(m_walletName, 0, KWallet::Wallet::Asynchronous);

    if (m_kwallet)
    {
	connect(m_kwallet, SIGNAL(walletOpened(bool)), this, SLOT(walletOpened(bool)));
    }
    else
    {
	m_log("LXQt:Wallet::kwallet: Failed to get a handle to kwallet, is it enabled?");
	QMetaObject::invokeMethod(this, "walletOpened", Qt::QueuedConnection,Q_ARG(bool, false));
    }
}

void LXQt::Wallet::kwallet::walletOpened(bool opened)
{
    this->openedWallet(opened);
}

QByteArray LXQt::Wallet::kwallet::readValue(const QString &key)
{
    QString value;

    m_kwallet->readPassword(key, value);

    return value.toLatin1();
}

QVector<std::pair<QString, QByteArray>> LXQt::Wallet::kwallet::readAllKeyValues(void)
{
    QVector<std::pair<QString, QByteArray>> p;

    auto l = m_kwallet->entryList();

    QString value;

    auto j = l.size();

    for (decltype(j) i = 0; i < j; i++)
    {
        const auto &e = l.at(i);

        m_kwallet->readPassword(e, value);

	p.append({ e, value.toLatin1() });
    }

    return p;
}

void LXQt::Wallet::kwallet::log(std::function<void(QString)> f)
{
    m_log = std::move(f);
}

QStringList LXQt::Wallet::kwallet::readAllKeys(void)
{
    return m_kwallet->entryList();
}

void LXQt::Wallet::kwallet::deleteKey(const QString &key)
{
    m_kwallet->removeEntry(key);
}

int LXQt::Wallet::kwallet::walletSize(void)
{
    return m_kwallet->entryList().size();
}

void LXQt::Wallet::kwallet::closeWallet(bool b)
{
    m_kwallet->closeWallet(m_walletName, b);
}

LXQt::Wallet::BackEnd LXQt::Wallet::kwallet::backEnd(void)
{
    return LXQt::Wallet::BackEnd::kwallet;
}

bool LXQt::Wallet::kwallet::opened(void)
{
    if (m_kwallet)
    {
        return m_kwallet->isOpen();
    }
    else
    {
        return false;
    }
}

QObject *LXQt::Wallet::kwallet::qObject(void)
{
    return this;
}

QString LXQt::Wallet::kwallet::storagePath()
{
    return m_kwallet->PasswordFolder();
}

void LXQt::Wallet::kwallet::changeWalletPassWord(const QString &walletName,
        const QString &applicationName,
        std::function< void(bool) > function)
{
    Q_UNUSED(applicationName)
    Q_UNUSED(function)

    m_kwallet->changePassword(walletName, 0);
}

QStringList LXQt::Wallet::kwallet::managedWalletList()
{
    return m_kwallet->walletList();
}

QString LXQt::Wallet::kwallet::localDefaultWalletName()
{
    return m_kwallet->LocalWallet();
}

QString LXQt::Wallet::kwallet::networkDefaultWalletName()
{
    return m_kwallet->NetworkWallet();
}

