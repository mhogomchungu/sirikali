
#include "lxqt_windows_dpapi.h"

#include "password_dialog.h"
#include "changepassworddialog.h"

QByteArray windowsKeysStorageData() ;

void windowsKeysStorageData( const QByteArray& e ) ;

void windowsDebugWindow( const QString& e ) ;

#include <cstring>

#ifdef Q_OS_WIN

#include <windows.h>
#include <dpapi.h>

namespace SiriKali{
	namespace Windows{
		QString lastError() ;
	}
}

static std::pair< bool,QByteArray > _encrypt( const QByteArray& e,const QByteArray& entropy )
{
	DATA_BLOB DataIn ;
	DATA_BLOB DataOut ;
	DATA_BLOB DataEntropy ;

	ZeroMemory( &DataIn,sizeof( DATA_BLOB ) ) ;
	ZeroMemory( &DataOut,sizeof( DATA_BLOB ) ) ;
	ZeroMemory( &DataEntropy,sizeof( DATA_BLOB ) ) ;

	DataIn.pbData = reinterpret_cast< BYTE * >( const_cast< char * >( e.data() ) ) ;
	DataIn.cbData = static_cast< DWORD >( e.size() ) ;

	DataEntropy.pbData = reinterpret_cast< BYTE * >( const_cast< char * >( entropy.data() ) ) ;
	DataEntropy.cbData = static_cast< DWORD >( entropy.size() ) ;

	CRYPTPROTECT_PROMPTSTRUCT PromptStruct ;

	ZeroMemory( &PromptStruct,sizeof( PromptStruct ) ) ;
	PromptStruct.cbSize = sizeof( PromptStruct ) ;
	PromptStruct.szPrompt = L"Enter Password To Unlock LXQt Credentials Storage System." ;

	auto a = L"LXQt Wallet::Windows_dpapi." ;

	if( CryptProtectData( &DataIn,a,&DataEntropy,nullptr,&PromptStruct,0,&DataOut ) ){

		auto data = reinterpret_cast< char * >( DataOut.pbData ) ;
		auto size = static_cast< int >( DataOut.cbData ) ;

		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Data Successfully Encrypted." ) ;

		QByteArray r{ data,size } ;

		LocalFree( DataOut.pbData ) ;

		return { true,std::move( r ) } ;
	}else{
		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Failed To Encrypt Data" ) ;
		//windowsDebugWindow( SiriKali::Windows::lastError() ) ;

		LocalFree( DataOut.pbData ) ;

		return { false,{} } ;
	}
}

static std::pair< bool,QByteArray > _decrypt( const QByteArray& e,const QByteArray& entropy )
{
	DATA_BLOB DataIn ;
	DATA_BLOB DataOut ;
	DATA_BLOB DataEntropy ;

	ZeroMemory( &DataIn,sizeof( DATA_BLOB ) ) ;
	ZeroMemory( &DataOut,sizeof( DATA_BLOB ) ) ;
	ZeroMemory( &DataEntropy,sizeof( DATA_BLOB ) ) ;

	DataIn.pbData = reinterpret_cast< BYTE * >( const_cast< char * >( e.data() ) ) ;
	DataIn.cbData = static_cast< DWORD >( e.size() ) ;

	DataEntropy.pbData = reinterpret_cast< BYTE * >( const_cast< char * >( entropy.data() ) ) ;
	DataEntropy.cbData = static_cast< DWORD >( entropy.size() ) ;

	CRYPTPROTECT_PROMPTSTRUCT PromptStruct;

	ZeroMemory( &PromptStruct,sizeof( PromptStruct ) ) ;

	PromptStruct.cbSize        = sizeof( PromptStruct ) ;
	PromptStruct.dwPromptFlags = CRYPTPROTECT_PROMPT_ON_UNPROTECT ;
	PromptStruct.szPrompt      = L"Enter Password To Unlock LXQt Credentials Storage System" ;

	if( CryptUnprotectData(	&DataIn,nullptr,&DataEntropy,nullptr,&PromptStruct,0,&DataOut ) ){

		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Data Successfully Decrypted." ) ;

		auto data = reinterpret_cast< char * >( DataOut.pbData ) ;
		auto size = static_cast< int >( DataOut.cbData ) ;

		QByteArray r{ data,size } ;

		LocalFree( DataOut.pbData ) ;

		return { true,std::move( r ) } ;
	}else{
		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Failed To Decrypt Data." ) ;
		//windowsDebugWindow( SiriKali::Windows::lastError() ) ;

		LocalFree( DataOut.pbData ) ;

		return { false,{} } ;
	}
}

#else

static std::pair< bool,QByteArray > _decrypt( const QByteArray& e,const QByteArray& s )
{
	Q_UNUSED( e )
	Q_UNUSED( s )
	return { false,{} } ;
}

static std::pair< bool,QByteArray > _encrypt( const QByteArray& e,const QByteArray& s )
{
	Q_UNUSED( e )
	Q_UNUSED( s )
	return { false,{} } ;
}

#endif

LXQt::Wallet::windows_dpapi::windows_dpapi()
{
}

LXQt::Wallet::windows_dpapi::~windows_dpapi()
{
	if( m_opened ){

		auto b = _encrypt( this->serializeData(),m_entropy ) ;

		if( b.first ){

			windowsKeysStorageData( b.second ) ;
		}
	}else{
		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Wallet Not Opened." ) ;
	}
}

bool LXQt::Wallet::windows_dpapi::open( const QString& walletName,
					const QString& applicationName,
					QWidget * parent,
					const QString& password,
					const QString& displayApplicationName )
{
	this->open( walletName,
		    applicationName,
		    []( bool e ){ Q_UNUSED( e ) },
		    parent,
		    password,
		    displayApplicationName ) ;

	m_loop.exec() ;

	return m_opened ;
}

void LXQt::Wallet::windows_dpapi::open( const QString& walletName,
					const QString& applicationName,
					std::function< void( bool )> function ,
					QWidget * parent,
					const QString& password,
					const QString& displayApplicationName )
{
	if( parent ){

		this->setParent( parent ) ;
	}

	m_walletName      = walletName ;
	m_applicationName = applicationName ;

	m_walletOpened = std::move( function ) ;

	if( m_applicationName.isEmpty() ){

		m_applicationName = m_walletName ;
	}

	if( displayApplicationName.isEmpty() ){

		m_displayApplicationName = m_applicationName ;
	}else{
		m_displayApplicationName = displayApplicationName ;
	}

	this->setEntropy( password ) ;

	m_data = windowsKeysStorageData() ;

	if( m_data.isEmpty() ){

		this->createWallet() ;
	}else{
		this->openWallet() ;
	}
}

void LXQt::Wallet::windows_dpapi::setEntropy( const QString& e )
{
	m_entropy = e.toUtf8() ;
}

void LXQt::Wallet::windows_dpapi::createWallet()
{
	using cbd = LXQt::Wallet::changePassWordDialog ;

	const auto& w = m_walletName ;
	const auto& d = m_displayApplicationName ;

	cbd::instance( this,w,d,[ this ]( const QString& password,bool create ){

		if( create ){

			this->setEntropy( password ) ;
			m_opened = true ;
			m_walletOpened( true ) ;
		}else{
			m_opened = false ;
			m_walletOpened( false ) ;
		}
	} ) ;
}

void LXQt::Wallet::windows_dpapi::openWallet()
{
	auto m = _decrypt( m_data,m_entropy ) ;

	if( m.first ){

		this->deserializeData( m.second ) ;

		m_opened = true ;

		return m_walletOpened( true ) ;
	}

	using pwd = LXQt::Wallet::password_dialog ;

	auto _cancelled = [ this ](){

		if( m_loop.isRunning() ){

			m_loop.exit() ;
		}

		m_opened = false ;

		m_walletOpened( m_opened ) ;
	} ;

	pwd::instance( this,
		       m_walletName,
		       m_displayApplicationName,
		       [ this ]( const QString& p ){ this->openWalletWithPassword( p ) ; },
		       std::move( _cancelled ),
		       &m_correctPassword ) ;
}

void LXQt::Wallet::windows_dpapi::openWalletWithPassword( QString e )
{
	this->setEntropy( e ) ;

	auto m = _decrypt( m_data,m_entropy ) ;

	bool s = m.first ;

	m_opened = s ;

	if( s ){

		this->deserializeData( m.second ) ;

		m_walletOpened( s ) ;

		if( m_loop.isRunning() ){

			m_loop.exit() ;
		}
	}

	m_correctPassword( s ) ;
}

void LXQt::Wallet::windows_dpapi::deserializeData( const QByteArray& e )
{
	int s ;

	auto data = e.constData() ;

	std::memcpy( &s,data,sizeof( int ) ) ;

	windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Number Of Entries In Wallet Is: " + QString::number( s ) ) ;

	if( s != 0 ){

		const size_t int_size    = sizeof( int ) ;
		const size_t header_size = 2 * sizeof( int ) ;

		data = data + sizeof( int ) ;

		int keySize ;
		int valueSize ;

		for( int i = 0 ; i < s ; i++ ){

			std::memcpy( &keySize,data,int_size ) ;
			std::memcpy( &valueSize,data + int_size,int_size ) ;

			auto a = QString::fromUtf8( data + header_size,keySize ) ;
			auto b = QByteArray{ data + header_size + keySize,valueSize } ;

			m_keys.append( { std::move( a ),std::move( b ) } ) ;

			data = data + header_size + keySize + valueSize ;
		}
	}
}

QByteArray LXQt::Wallet::windows_dpapi::serializeData()
{
	QByteArray data ;

	const size_t int_size = sizeof( int ) ;

	char buffer[ int_size ] ;

	int s = m_keys.size() ;

	std::memcpy( buffer,&s,int_size ) ;

	data.append( buffer,int_size ) ;

	if( s != 0 ){

		const size_t header_size = 2 * sizeof( int ) ;

		char buffer[ header_size ] ;

		for( const auto& it : m_keys ){

			const auto& key   = it.first ;
			const auto& value = it.second ;

			int keySize   = key.size() ;
			int valueSize = value.size() ;

			std::memcpy( buffer,&keySize,int_size ) ;
			std::memcpy( buffer + int_size,&valueSize,int_size ) ;

			data.append( buffer,header_size ) ;

			data.append( QByteArray{ key.toUtf8(),keySize } ) ;

			data.append( value ) ;
		}
	}

	return data ;
}

bool LXQt::Wallet::windows_dpapi::opened()
{
	return m_opened ;
}

bool LXQt::Wallet::windows_dpapi::addKey( const QString& key,const QByteArray& value )
{
	m_keys.append( { key,value } ) ;
	return true ;
}

QByteArray LXQt::Wallet::windows_dpapi::readValue( const QString& key )
{
	for( const auto& it : m_keys ){

		if( it.first == key ){

			return it.second ;
		}
	}

	return {} ;
}

void LXQt::Wallet::windows_dpapi::deleteKey( const QString& key )
{
	for( int i = 0 ; i < m_keys.size() ; i++ ){

		if( m_keys[ i ].first == key ){

			m_keys.remove( i ) ;
			break ;
		}
	}
}

QStringList LXQt::Wallet::windows_dpapi::readAllKeys()
{
	QStringList s ;

	for( const auto& it : m_keys ){

		s.append( it.first ) ;
	}

	return s ;
}

QVector< std::pair< QString,QByteArray > > LXQt::Wallet::windows_dpapi::readAllKeyValues()
{
	return m_keys ;
}

int LXQt::Wallet::windows_dpapi::walletSize()
{
	return m_keys.size() ;
}

QStringList LXQt::Wallet::windows_dpapi::managedWalletList()
{
	return {} ;
}

QString LXQt::Wallet::windows_dpapi::storagePath()
{
	return {} ;
}

QString LXQt::Wallet::windows_dpapi::localDefaultWalletName()
{
	return {} ;
}

QString LXQt::Wallet::windows_dpapi::networkDefaultWalletName()
{
	return {} ;
}

void LXQt::Wallet::windows_dpapi::closeWallet( bool e )
{
	Q_UNUSED( e )
}

void LXQt::Wallet::windows_dpapi::changeWalletPassWord( const QString& walletName,
							const QString& applicationName,
							std::function< void( bool ) > function )
{
	Q_UNUSED( walletName )
	Q_UNUSED( applicationName )
	Q_UNUSED( function )
}

void LXQt::Wallet::windows_dpapi::setImage( const QIcon& e )
{
	Q_UNUSED( e )
}

LXQt::Wallet::BackEnd LXQt::Wallet::windows_dpapi::backEnd()
{
	return LXQt::Wallet::BackEnd::windows_DPAPI ;
}

QObject * LXQt::Wallet::windows_dpapi::qObject()
{
	return nullptr ;
}
