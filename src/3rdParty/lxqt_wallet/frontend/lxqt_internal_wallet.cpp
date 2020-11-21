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

#include "lxqt_internal_wallet.h"

namespace Task = LXQt::Wallet::Task;

LXQt::Wallet::internalWallet::internalWallet() : m_wallet(nullptr)
{
}

LXQt::Wallet::internalWallet::~internalWallet()
{
    lxqt_wallet_close(&m_wallet);
}

void LXQt::Wallet::internalWallet::setImage(const QIcon &image)
{
    this->setWindowIcon(image);
}

void LXQt::Wallet::internalWallet::openWallet(QString password)
{
    m_password = password;

    Task::run< lxqt_wallet_error >([this]()
    {
        return lxqt_wallet_open(&m_wallet,
                                m_password.toLatin1().constData(),
                                m_password.size(), m_walletName.toLatin1().constData(),
				m_applicationName.toLatin1().constData());

    }).then([this](lxqt_wallet_error r)
    {
	this->opened(r == lxqt_wallet_no_error);
    });
}

void LXQt::Wallet::internalWallet::opened(bool opened)
{
    m_correctPassword(opened);

    m_opened = opened;

    if (m_opened)
    {
        if (m_loop.isRunning())
        {
	    m_loop.exit();
        }

	this->walletIsOpen(m_opened);
    }
}

bool LXQt::Wallet::internalWallet::open(const QString &walletName,
                                        const QString &applicationName,
                                        QWidget *parent,
                                        const QString &password,
                                        const QString &displayApplicationName)
{
    this->open(walletName,
               applicationName,
	       [](bool e) { Q_UNUSED(e) },
	       parent,
	       password,
	       displayApplicationName);

    m_loop.exec();

    return m_opened;
}

void LXQt::Wallet::internalWallet::open(const QString &walletName,
                                        const QString &applicationName,
                                        std::function< void(bool) > function,
                                        QWidget *parent,
                                        const QString &password,
                                        const QString &displayApplicationName)
{
    if (parent)
    {
	this->setParent(parent);
    }

    m_walletName      = walletName;
    m_applicationName = applicationName;
    m_password        = password;

    m_walletOpened = std::move(function);

    if (m_applicationName.isEmpty())
    {
	m_applicationName = m_walletName;
    }

    if (displayApplicationName.isEmpty())
    {
	m_displayApplicationName = m_applicationName;
    }
    else
    {
	m_displayApplicationName = displayApplicationName;
    }

    if (LXQt::Wallet::walletExists(LXQt::Wallet::BackEnd::internal, m_walletName, m_applicationName))
    {
	this->openWallet();
    }
    else
    {
	this->createWallet();
    }
}

void LXQt::Wallet::internalWallet::openWallet()
{
    if (m_password.isEmpty())
    {
        /*
         * to prevent an unnecessary prompt,try to open a wallet without a password and then
         * prompt on failure,this will allow a silent opening of the wallet set without a password.
         */

	Task::run< lxqt_wallet_error >([this]()
        {
            return lxqt_wallet_open(&m_wallet,
                                    m_password.toLatin1().constData(),
                                    m_password.size(),
                                    m_walletName.toLatin1().constData(),
				    m_applicationName.toLatin1().constData());

	}).then([this](lxqt_wallet_error r)
        {
            if (r == lxqt_wallet_no_error)
            {
		this->opened(true);
            }
            else
            {
                /*
                 * passwordless opening failed,prompt a user for a password
                 */

		using pwd = LXQt::Wallet::password_dialog;

		auto _cancelled = [this]()
                {
		    m_opened = false;

		    m_loop.exit();

		    this->walletIsOpen(false);
		};

                pwd::instance(this,
                              m_walletName,
                              m_displayApplicationName,
			      [this](const QString & p) { this->openWallet(p); },
			      std::move(_cancelled),
			      &m_correctPassword);
            }
	});
    }
    else
    {
	this->openWallet(m_password);
    }
}

void LXQt::Wallet::internalWallet::createWallet()
{
    using cbd = LXQt::Wallet::changePassWordDialog;

    const auto &w = m_walletName;
    const auto &d = m_displayApplicationName;

    cbd::createInstance(this, w, d, [this](const QString & password, bool create)
    {
        if (create)
        {
	    m_password = password;

	    Task::run< lxqt_wallet_error >([this]()
            {
                return lxqt_wallet_create(m_password.toLatin1().constData(),
                                          m_password.size(),
                                          m_walletName.toLatin1().constData(),
					  m_applicationName.toLatin1().constData());

	    }).then([this](lxqt_wallet_error r)
            {
                if (r == lxqt_wallet_no_error)
                {
		    this->openWallet(m_password);
                }
                else
                {
		    this->walletIsOpen(false);
                }
	    });
        }
        else
        {
	    this->walletIsOpen(false);
        }
    });
}

void LXQt::Wallet::internalWallet::changeWalletPassWord(const QString &walletName,
							const QString &applicationName,
							std::function<void(bool)> function)
{
    using args = LXQt::Wallet::changePassWordDialog::changeArgs;

    auto change = [ this, function = std::move(function) ](const QString & old,
							   const QString & New,
							   bool cancelled)->args
    {
	auto _open = [&](const QString &password)
        {
            return lxqt_wallet_open(&m_wallet,
				    password.toLatin1().constData(),
				    password.size(),
				    m_walletName.toLatin1().constData(),
				    m_applicationName.toLatin1().constData());
	};

        if (cancelled)
        {
	    function(false);

	    return {false, false};
        }

	auto result = Task::await<args>([&]()->args{

            if (!this->opened())
            {
		auto s = _open(old);

                if (s != lxqt_wallet_no_error)
                {
		    return {true, false};
                }
            }

            auto m = lxqt_wallet_change_wallet_password(m_wallet,
							New.toLatin1().constData(),
							New.size());

            if (m != lxqt_wallet_no_error)
            {
		lxqt_wallet_close(&m_wallet);

		_open(New);

		return {false, true};
            }
	    else
	    {
		return {false, false};
            }
	});

        if (result.failedToChange == false && result.failedToUnlock == false)
        {
	    function(true);
        }

	return result;
    };

    LXQt::Wallet::changePassWordDialog::changeInstance(this,
						       walletName,
						       applicationName,
						       std::move(change));
}

QByteArray LXQt::Wallet::internalWallet::readValue(const QString &key)
{
    lxqt_wallet_key_values_t key_value;

    if (lxqt_wallet_read_key_value(m_wallet, key.toLatin1().constData(), key.size() + 1, &key_value))
    {
	return QByteArray(key_value.key_value, key_value.key_value_size);
    }
    else
    {
	return QByteArray();
    }
}

QVector< std::pair< QString, QByteArray > > LXQt::Wallet::internalWallet::readAllKeyValues(void)
{
    QVector< std::pair < QString, QByteArray > > w;

    lxqt_wallet_iterator_t iter;

    iter.iter_pos = 0;

    while (lxqt_wallet_iter_read_value(m_wallet, &iter))
    {
	w.append({ QByteArray(iter.entry.key, iter.entry.key_size - 1),
                    QByteArray(iter.entry.key_value, iter.entry.key_value_size)
		  });
    }

    return w;
}

QStringList LXQt::Wallet::internalWallet::readAllKeys()
{
    QStringList l;
    lxqt_wallet_iterator_t iter;

    iter.iter_pos = 0;

    while (lxqt_wallet_iter_read_value(m_wallet, &iter))
    {
	l.append(QByteArray(iter.entry.key, iter.entry.key_size - 1));
    }

    return l;
}

bool LXQt::Wallet::internalWallet::addKey(const QString &key, const QByteArray &value)
{
    /*
     * For the key,we add +1 to the key size to include the '\0' character in the key to
     * avoid possible collisions if our keys prefix match
     */
    auto r = lxqt_wallet_add_key(m_wallet,
                                 key.toLatin1().constData(),
                                 key.size() + 1,
                                 value.constData(),
				 value.size());

    return r == lxqt_wallet_no_error;
}

void LXQt::Wallet::internalWallet::deleteKey(const QString &key)
{
    lxqt_wallet_delete_key(m_wallet, key.toLatin1().constData(), key.size() + 1);
}

int LXQt::Wallet::internalWallet::walletSize(void)
{
    return lxqt_wallet_wallet_size(m_wallet);
}

void LXQt::Wallet::internalWallet::closeWallet(bool b)
{
    Q_UNUSED(b)
    lxqt_wallet_close(&m_wallet);
}

LXQt::Wallet::BackEnd LXQt::Wallet::internalWallet::backEnd()
{
    return LXQt::Wallet::BackEnd::internal;
}

bool LXQt::Wallet::internalWallet::opened()
{
    return m_wallet != nullptr;
}

QObject *LXQt::Wallet::internalWallet::qObject()
{
    this->setObjectName(m_password);
    return this;
}

void LXQt::Wallet::internalWallet::walletIsOpen(bool e)
{
    m_walletOpened(e);
}

QString LXQt::Wallet::internalWallet::storagePath()
{
    return QString();
}

QStringList LXQt::Wallet::internalWallet::managedWalletList()
{
    char path[ 4096 ];
    lxqt_wallet_application_wallet_path(path, 4096, m_applicationName.toLatin1().constData());
    QDir d(path);
    QStringList l = d.entryList();
    l.removeOne(".");
    l.removeOne("..");

    if (l.size() > 0)
    {
        /*
         * remove the extension part of a file name
         */
	const QString &q = l.at(0);
	l.replaceInStrings(q.mid(q.indexOf(".")), "");
    }

    return l;
}

QString LXQt::Wallet::internalWallet::localDefaultWalletName()
{
    return QString();
}

QString LXQt::Wallet::internalWallet::networkDefaultWalletName()
{
	return QString();
}

void LXQt::Wallet::internalWallet::log(std::function<void(QString)>)
{
}
