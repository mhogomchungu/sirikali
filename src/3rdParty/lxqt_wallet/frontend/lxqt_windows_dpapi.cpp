
#include "lxqt_windows_dpapi.h"

#include "password_dialog.h"
#include "changepassworddialog.h"

static const int TEST_VALUE = -1;

#include <cstring>

#ifdef Q_OS_WIN

#include <windows.h>
#include <dpapi.h>
#include <bcrypt.h>

class db
{
public:
    db(QByteArray&&) = delete;
    db(const QByteArray &e) : m_delete(false)
    {
	ZeroMemory(&m_db, sizeof(DATA_BLOB));

	m_db.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(e.data()));
	m_db.cbData = static_cast<DWORD>(e.size());
    }
    db()
    {
	ZeroMemory(&m_db, sizeof(DATA_BLOB));
    }
    DATA_BLOB *operator&()
    {
	return &m_db;
    }
    QByteArray data()
    {
	auto data = reinterpret_cast<const char*>(m_db.pbData);
	auto size = static_cast<int>(m_db.cbData);

	return {data, size};
    }
    CRYPTPROTECT_PROMPTSTRUCT *prompt(unsigned long flags = CRYPTPROTECT_PROMPT_ON_UNPROTECT)
    {
	ZeroMemory(&m_prompt, sizeof(m_prompt));

	m_prompt.cbSize        = sizeof(m_prompt);
	m_prompt.dwPromptFlags = flags;
	m_prompt.szPrompt      = L"Enter Password To Unlock LXQt Credentials Storage System";

	return &m_prompt;
    }
    ~db()
    {
        if (m_delete)
        {
	    LocalFree(m_db.pbData);
        }
    }
private:
    bool m_delete = true;
    DATA_BLOB m_db;
    CRYPTPROTECT_PROMPTSTRUCT m_prompt;
};

template<typename Function>
static QByteArray _entropy(QSettings &settings, const QByteArray& entropy, const Function& log)
{
    if(!settings.contains("WindowsPbkdf2Interations"))
    {
	settings.setValue("WindowsPbkdf2Interations",50000);
    }

    int iterations = settings.value( "WindowsPbkdf2Interations" ).toInt();

    BCRYPT_ALG_HANDLE h;

    auto s = BCryptOpenAlgorithmProvider(&h,
					 BCRYPT_SHA256_ALGORITHM,
					 nullptr,
					 BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (BCRYPT_SUCCESS(s))
    {
	auto ss = "lxqt_windows_dpapi";

	std::array<unsigned char,32> buffer;

	BCryptDeriveKeyPBKDF2(h,
			      reinterpret_cast<unsigned char*>(const_cast<char*>(entropy.data())),
			      static_cast<ULONG>(entropy.size()),
			      reinterpret_cast<unsigned char*>(const_cast<char*>(ss)),
			      static_cast<ULONG>(strlen(ss)),
			      static_cast<ULONGLONG>(iterations),
			      buffer.data(),
			      static_cast<ULONG>(buffer.size()),
			      0);

	BCryptCloseAlgorithmProvider(h, 0);
	return QByteArray(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }else{
	log("LXQt Wallet::Windows_dpapi: Failed to created PBKDF2 key");
	return {} ;
    }
}

LXQt::Wallet::Task::future<LXQt::Wallet::windows_dpapi::result>
&LXQt::Wallet::windows_dpapi::encrypt(QByteArray data)
{
    return LXQt::Wallet::Task::run<result>([this,data=std::move(data)]()->result
    {
	auto entropy = _entropy(*m_settings, m_entropy, m_log);
	db In(data);
	db Entropy(entropy);
	db Out;

	auto a = L"LXQt Wallet::Windows_dpapi.";

        if (CryptProtectData(&In, a, &Entropy, nullptr, nullptr, 0, &Out))
        {
	    m_log("LXQt:Wallet::Windows_dpapi: Data Successfully Encrypted.");

	    return {true, Out.data()};
        }
	else
	{
	    m_log("LXQt:Wallet::Windows_dpapi: Failed To Encrypt Data.");

	    return {false, std::move(data)};
        }
    });
}

LXQt::Wallet::Task::future<LXQt::Wallet::windows_dpapi::result>
&LXQt::Wallet::windows_dpapi::decrypt(QByteArray data)
{
    return LXQt::Wallet::Task::run<result>([this,data = std::move(data)]()->result
    {
	auto entropy = _entropy(*m_settings, m_entropy, m_log);
	db In(data);
	db Entropy(entropy);
	db Out;

        if (CryptUnprotectData(&In, nullptr, &Entropy, nullptr, In.prompt(), 0, &Out))
        {
	    m_log("LXQt:Wallet::Windows_dpapi: Data Successfully Decrypted.");

	    return {true, Out.data()};
        }
	else
	{
	    m_log("LXQt:Wallet::Windows_dpapi: Failed To Decrypt Data.");

	    return {false, std::move(data)};
        }
    });
}

#else

LXQt::Wallet::Task::future<LXQt::Wallet::windows_dpapi::result>
&LXQt::Wallet::windows_dpapi::decrypt(QByteArray data)
{
    Q_UNUSED(data)
    return LXQt::Wallet::Task::run<result>([]()->result
    {
	return {false, {}};
    });
}

LXQt::Wallet::Task::future<LXQt::Wallet::windows_dpapi::result>
&LXQt::Wallet::windows_dpapi::encrypt(QByteArray data)
{
    Q_UNUSED(data)
    return LXQt::Wallet::Task::run<result>([]()->result
    {
	return {false, {}};
    });
}

#endif

LXQt::Wallet::windows_dpapi::windows_dpapi()
{
}

LXQt::Wallet::windows_dpapi::~windows_dpapi()
{
    this->store();
}

QByteArray LXQt::Wallet::windows_dpapi::getData()
{
    if( !m_settings->contains(m_settingsName))
    {
	m_settings->setValue(m_settingsName,QByteArray());
    }

    return m_settings->value(m_settingsName).toByteArray();
}

void LXQt::Wallet::windows_dpapi::store()
{
    if (m_opened)
    {
	auto b = this->encrypt(this->serializeData()).await();

        if (b.success)
        {
	    m_settings->setValue(m_settingsName, b.data);
        }
    }
    else
    {
	m_log("LXQt:Wallet::Windows_dpapi: Wallet Not Opened.");
    }
}

bool LXQt::Wallet::windows_dpapi::open(const QString &walletName,
                                       const QString &applicationName,
                                       QWidget *parent,
                                       const QString &password,
                                       const QString &displayApplicationName)
{
    QEventLoop loop;

    auto exitLoop = [&](bool e)
    {
        Q_UNUSED(e)

	loop.exit();
    };

    this->open(walletName,
               applicationName,
               std::move(exitLoop),
               parent,
               password,
	       displayApplicationName);

    loop.exec();

    return m_opened;
}

void LXQt::Wallet::windows_dpapi::open(const QString &walletName,
                                       const QString &applicationName,
				       std::function<void(bool)> function,
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

    m_settings.reset(new QSettings(m_applicationName, m_walletName));

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

    this->setEntropy(password);

    auto data = this->getData();

    if (data.isEmpty())
    {
	this->createWallet();
    }
    else
    {
	this->openWallet(std::move(data));
    }
}

void LXQt::Wallet::windows_dpapi::setEntropy(const QString &e)
{
    m_entropy = e.toUtf8();
}

void LXQt::Wallet::windows_dpapi::createWallet()
{
    using cbd = LXQt::Wallet::changePassWordDialog;

    const auto &w = m_walletName;
    const auto &d = m_displayApplicationName;

    cbd::createInstance(this, w, d, [this](const QString & password, bool create)
    {
        if (create)
        {
	    this->setEntropy(password);
	    m_opened = true;
	    m_walletOpened(true);
        }
        else
        {
	    m_opened = false;
	    m_walletOpened(false);
        }
    });
}

void LXQt::Wallet::windows_dpapi::openWallet(QByteArray data)
{
    this->decrypt(std::move(data)).then([this](result m)
    {
        if (m.success)
        {
	    this->deserializeData(m.data);
	    m_opened = true;
	    m_walletOpened(true);
        }
        else
        {
	    using pwd = LXQt::Wallet::password_dialog;

	    auto _cancelled = [this]()
            {
		m_opened = false;
		m_walletOpened(m_opened);
	    };

            pwd::instance(this,
                          m_walletName,
                          m_displayApplicationName,
			  [this,data=std::move(m.data)](const QString &p) { this->openWalletWithPassword(p, data);},
			  std::move(_cancelled),
			  &m_correctPassword);
        }
    });
}
void LXQt::Wallet::windows_dpapi::log(std::function<void(QString)> function)
{
    m_log = std::move(function);
}

void LXQt::Wallet::windows_dpapi::openWalletWithPassword(QString e, const QByteArray &data)
{
    this->setEntropy(e);

    this->decrypt(data).then([this](const result &m)
    {
	bool s = m.success;

	m_opened = s;
	m_correctPassword(s);

        if (s)
        {
	    this->deserializeData(m.data);
	    m_walletOpened(s);
        }
    });
}

void LXQt::Wallet::windows_dpapi::deserializeData(const QByteArray &e)
{
    const size_t int_size = sizeof(int);

    int test;

    auto data = e.constData();

    std::memcpy(&test, data, int_size);

    if (test != TEST_VALUE)
    {
	m_log("LXQt:Wallet::Windows_dpapi: CRITICAL: Deleting Stored Data Because It Appears To Be Corrupted.");

	return;
    }

    int s;

    std::memcpy(&s, data + int_size, int_size);

    m_log("LXQt:Wallet::Windows_dpapi: Number Of Entries In Wallet Is: " + QString::number(s));

    if (s != 0)
    {
	const size_t header_size = 2 * sizeof(int);

	data = data + 2 * int_size;

	int keySize;
	int valueSize;

	for (int i = 0; i < s; i++)
        {
	    std::memcpy(&keySize, data, int_size);
	    std::memcpy(&valueSize, data + int_size, int_size);

	    auto a = QString::fromUtf8(data + header_size, keySize);
	    auto b = QByteArray { data + header_size + keySize, valueSize };

	    m_keys.append( { std::move(a), std::move(b) });

	    data = data + header_size + keySize + valueSize;
        }
    }
}

QByteArray LXQt::Wallet::windows_dpapi::serializeData()
{
    QByteArray data;

    const size_t int_size = sizeof(int);

    char buffer[2 * int_size];

    std::memcpy(buffer, &TEST_VALUE, int_size);

    int s = m_keys.size();

    std::memcpy(buffer + int_size, &s, int_size);

    data.append(buffer, 2 * int_size);

    if (s != 0)
    {
	const size_t header_size = 2 * sizeof(int);

	char buffer[header_size];

	for (const auto &it : m_keys)
        {
	    const auto &key   = it.first;
	    const auto &value = it.second;

	    int keySize   = key.size();
	    int valueSize = value.size();

	    std::memcpy(buffer, &keySize, int_size);
	    std::memcpy(buffer + int_size, &valueSize, int_size);

	    data.append(buffer, header_size);

	    data.append(QByteArray {key.toUtf8(), keySize});

	    data.append(value);
        }
    }

    return data;
}

bool LXQt::Wallet::windows_dpapi::opened()
{
    return m_opened;
}

bool LXQt::Wallet::windows_dpapi::addKey(const QString &key, const QByteArray &value)
{
    m_keys.append({key, value});
    return true;
}

QByteArray LXQt::Wallet::windows_dpapi::readValue(const QString &key)
{
    for (const auto & it : m_keys)
    {
        if (it.first == key)
        {
	    return it.second;
        }
    }

    return {};
}

void LXQt::Wallet::windows_dpapi::deleteKey(const QString &key)
{
    for (int i = 0; i < m_keys.size(); i++)
    {
	if (m_keys[i].first == key)
        {
	    m_keys.remove(i);
	    break;
        }
    }
}

QStringList LXQt::Wallet::windows_dpapi::readAllKeys()
{
    QStringList s;

    for (const auto & it : m_keys)
    {
	s.append(it.first);
    }

    return s;
}

QVector<std::pair<QString, QByteArray>> LXQt::Wallet::windows_dpapi::readAllKeyValues()
{
    return m_keys;
}

int LXQt::Wallet::windows_dpapi::walletSize()
{
    return m_keys.size();
}

QStringList LXQt::Wallet::windows_dpapi::managedWalletList()
{
    return {};
}

QString LXQt::Wallet::windows_dpapi::storagePath()
{
    return {};
}

QString LXQt::Wallet::windows_dpapi::localDefaultWalletName()
{
    return {};
}

QString LXQt::Wallet::windows_dpapi::networkDefaultWalletName()
{
    return {};
}

void LXQt::Wallet::windows_dpapi::closeWallet(bool e)
{
    Q_UNUSED(e)
}

void LXQt::Wallet::windows_dpapi::changeWalletPassWord(const QString &walletName,
						       const QString &applicationName,
						       std::function< void(bool) > function)
{
    using args = LXQt::Wallet::changePassWordDialog::changeArgs;

    auto change = [this, function = std::move(function)](const QString & old,
							 const QString & New,
							 bool cancelled)->args
    {
        if (cancelled)
        {
	    function(false);
	    return {false, false};
        }

	auto a = this->getData();

	this->setEntropy(old);

	auto m = this->decrypt(std::move(a)).await();

        if (m.success)
        {
	    this->setEntropy(New);

            if (!m_opened)
            {
		m_opened = true;
		m_keys.clear();
		this->deserializeData(m.data);
            }

	    this->store();

	    function(true);

	    return {false, false};
        }
	else
	{
	    return {true, false};
        }
    };

    LXQt::Wallet::changePassWordDialog::changeInstance(this,
						       walletName,
						       applicationName,
						       std::move(change));
}

void LXQt::Wallet::windows_dpapi::setImage(const QIcon &e)
{
    this->setWindowIcon(e);
}

LXQt::Wallet::BackEnd LXQt::Wallet::windows_dpapi::backEnd()
{
    return LXQt::Wallet::BackEnd::windows_dpapi;
}

QObject *LXQt::Wallet::windows_dpapi::qObject()
{
	return nullptr;
}
