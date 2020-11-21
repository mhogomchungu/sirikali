lxqt_wallet
===========

This project seeks to give a functionality for secure storage of information that can be presented in key-values pair
like user names-passwords pairs.

Currently the project can store the information in KDE's kwallet,GNOME's secret service or in an internal system that use libgcrypt
as its cryptographic backend.

The internal secure storage system allows the functionality to be provided without dependencies on KDE or gnome libraries.

This project is designed to be used by other projects simply by adding the source folder in the build system and start using it.

The front end is build on Qt/C++ and has an optional dependency on KDE's kwallet and GNOME's libsecret library.

The project allows other Qt based project's to target one API and use it for secure storage of information in different
secure storage systems.

GUI windows produced by the internat backend are <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet.jpeg">here</a>, <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet1.jpeg">here</a> and <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet2.jpeg">here</a>.

An example of how to use the library is below.
===========

**Full API is located <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/master/frontend/lxqt_wallet.h">here</a>.**

**1. Create an instance of the backend you want to use. A list of supported backends is <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/777d85735a1149c143a4d8933194d24fa5445174/frontend/lxqt_wallet.h#L51">here</a>. Call <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/777d85735a1149c143a4d8933194d24fa5445174/frontend/lxqt_wallet.h#L61">this</a> method first to make sure the backend you want to use was enabled at compile time. User of this API is responsible for the returned object and must delete it when done with it.**

```
LXQt::Wallet::Wallet * m_wallet = LXQt::Wallet::getWalletBackend(LXQt::Wallet::BackEnd::internal);
```
**2. Set parent widget. This step is necessary if an internal backend is used. Also call setImage() to set a custom image when when trying to unlock an internal wallet.**
```
m_wallet->setParent(this);
```
**3. Open a wallet. Full documentation of this API is <a href="https://github.com/mhogomchungu/lxqt_wallet/blob/777d85735a1149c143a4d8933194d24fa5445174/frontend/lxqt_wallet.h#L186">here</a>.**
```
bool opened = m_wallet->open("walletName","applicationName");
```
**4. Add a key to the wallet.**
```
m_wallet->add("key","value");
```
**5. Delete a key from a wallet.**
```
m_wallet->deleteKey("key");
```
**6. Get a list of all keys in a wallet.**
```
QStringList keys = m_wallet->readAllKeys();
```
**7. Read all keys and their corresponding values.**
```
QVector<std::pair<QString, QByteArray>> keyValues = readAllKeyValues();
```
**8. Close the wallet when done with it.**
```
m_wallet->closeWallet();
```
