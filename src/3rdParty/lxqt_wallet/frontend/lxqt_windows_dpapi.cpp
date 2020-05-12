
#include "lxqt_windows_dpapi.h"

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
	auto a = this->serializeData() ;

	if( !a.isEmpty() ){

		auto b = _encrypt( a,m_entropy ) ;

		if( b.first ){

			windowsKeysStorageData( b.second ) ;
		}
	}
}

void LXQt::Wallet::windows_dpapi::open( const QString& walletName,
					const QString& applicationName,
					std::function< void( bool )> function ,
					QWidget *,
					const QString& password,
					const QString& displayApplicationName )
{
	Q_UNUSED( displayApplicationName )

	auto a = walletName + applicationName + password ;

	m_entropy = a.toUtf8() ;

	m_opened = this->decryptData() ;

	function( m_opened ) ;
}

bool LXQt::Wallet::windows_dpapi::open( const QString& walletName,
					const QString& applicationName,
					QWidget *,
					const QString& password,
					const QString& displayApplicationName )
{
	Q_UNUSED( displayApplicationName )

	auto a = walletName + applicationName + password ;

	m_entropy = a.toUtf8() ;

	return this->decryptData() ;
}

bool LXQt::Wallet::windows_dpapi::decryptData()
{
	auto a = windowsKeysStorageData() ;

	if( a.isEmpty() ){

		windowsDebugWindow( "Window's Key Store Is Empty." ) ;
		return true ;
	}else{
		auto m = _decrypt( a,m_entropy ) ;

		if( m.first ){

			this->deserializeData( m.second ) ;
			return true ;
		}else{
			return false ;
		}
	}
}

void LXQt::Wallet::windows_dpapi::deserializeData( const QByteArray& e )
{
	if( e.isEmpty() ){

		windowsDebugWindow( "Windows Wallet Is Empty." ) ;
		return ;
	}

	int s ;

	auto data = e.constData() ;

	std::memcpy( &s,data,sizeof( int ) ) ;

	windowsDebugWindow( "Number Of Entries In Wallet Is: " + QString::number( s ) ) ;

	if( s != 0 ){

		const size_t size = sizeof( Header ) ;

		data = data + sizeof( int ) ;

		for( int i = 0 ; i < s ; i++ ){

			std::memcpy( &m_header,data,size ) ;

			auto a = QString::fromUtf8( data + size,m_header.keySize ) ;
			auto b = QByteArray{ data + size + m_header.keySize,m_header.valueSize } ;

			m_keys.append( { std::move( a ),std::move( b ) } ) ;

			data = data + size + m_header.keySize + m_header.valueSize ;
		}
	}
}

QByteArray LXQt::Wallet::windows_dpapi::serializeData()
{
	QByteArray data ;

	char buffer[ sizeof( int ) ] ;

	int s = m_keys.size() ;

	std::memcpy( buffer,&s,sizeof( int ) ) ;

	data.append( buffer,sizeof( int ) ) ;

	if( s != 0 ){

		char buffer[ sizeof( Header ) ] ;
		const size_t size = sizeof( Header ) ;

		for( const auto& it : m_keys ){

			const auto& key   = it.first ;
			const auto& value = it.second ;

			m_header.keySize   = key.size() ;
			m_header.valueSize = value.size() ;

			std::memcpy( buffer,&m_header,size ) ;

			data.append( buffer,size ) ;

			data.append( QByteArray{ key.toUtf8(),key.size() } ) ;

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
