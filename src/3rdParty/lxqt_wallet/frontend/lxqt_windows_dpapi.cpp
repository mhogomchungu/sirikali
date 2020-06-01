
#include "lxqt_windows_dpapi.h"

#include "password_dialog.h"
#include "changepassworddialog.h"

QByteArray windowsKeysStorageData() ;

void windowsKeysStorageData( const QByteArray& e ) ;

int windowsPbkdf2Interations() ;

void windowsDebugWindow( const QString& e,bool = false ) ;

static const int TEST_VALUE = -1 ;

#include <cstring>

struct result{

	bool success ;
	QByteArray data ;
} ;

#ifdef Q_OS_WIN

#include "task.h"

#include <windows.h>
#include <dpapi.h>

#include <qpassworddigestor.h>

namespace SiriKali{
	namespace Windows{
		QString lastError() ;
	}
}

class db{
public:
	db( const QByteArray& e ) : m_delete( false )
	{
		ZeroMemory( &m_db,sizeof( DATA_BLOB ) ) ;

		m_db.pbData = reinterpret_cast< BYTE * >( const_cast< char * >( e.data() ) ) ;
		m_db.cbData = static_cast< DWORD >( e.size() ) ;
	}
	db()
	{
		ZeroMemory( &m_db,sizeof( DATA_BLOB ) ) ;
	}
	DATA_BLOB * operator&()
	{
		return &m_db ;
	}
	QByteArray data()
	{
		auto data = reinterpret_cast< const char * >( m_db.pbData ) ;
		auto size = static_cast< int >( m_db.cbData ) ;

		return { data,size } ;
	}
	CRYPTPROTECT_PROMPTSTRUCT * prompt( unsigned long flags = CRYPTPROTECT_PROMPT_ON_UNPROTECT )
	{
		ZeroMemory( &m_prompt,sizeof( m_prompt ) ) ;

		m_prompt.cbSize        = sizeof( m_prompt ) ;
		m_prompt.dwPromptFlags = flags ;
		m_prompt.szPrompt      = L"Enter Password To Unlock LXQt Credentials Storage System" ;

		return &m_prompt ;
	}
	~db()
	{
		if( m_delete ){

			LocalFree( m_db.pbData ) ;
		}
	}
private:
	bool m_delete = true ;
	DATA_BLOB m_db ;
	CRYPTPROTECT_PROMPTSTRUCT m_prompt ;
};

static QByteArray _entropy( const QByteArray& e )
{
	auto len = QCryptographicHash::hashLength( QCryptographicHash::Sha256 ) ;

	return QPasswordDigestor::deriveKeyPbkdf2( QCryptographicHash::Sha256,
						   e,
						   "lxqt_windows_dpapi",
						   windowsPbkdf2Interations(),
						   static_cast< quint64 >( len ) ) ;
}

static LXQt::Wallet::Task::future< result >& _encrypt( const QByteArray& e,const QByteArray& entropy )
{
	return LXQt::Wallet::Task::run< result >( [ & ]()->result{

		db In( e ) ;
		db Entropy( _entropy( entropy ) ) ;
		db Out ;

		auto a = L"LXQt Wallet::Windows_dpapi." ;

		if( CryptProtectData( &In,a,&Entropy,nullptr,nullptr,0,&Out ) ){

			windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Data Successfully Encrypted." ) ;

			return { true,Out.data() } ;
		}else{
			windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Failed To Encrypt Data." ) ;
			//windowsDebugWindow( SiriKali::Windows::lastError() ) ;

			return { false,{} } ;
		}
	} ) ;
}

static LXQt::Wallet::Task::future< result >& _decrypt( const QByteArray& e,const QByteArray& entropy )
{
	return LXQt::Wallet::Task::run< result >( [ & ]()->result{

		db In( e ) ;
		db Entropy( _entropy( entropy  ) ) ;
		db Out ;

		if( CryptUnprotectData(	&In,nullptr,&Entropy,nullptr,In.prompt(),0,&Out ) ){

			windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Data Successfully Decrypted." ) ;

			return { true,Out.data() } ;
		}else{
			windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Failed To Decrypt Data." ) ;
			//windowsDebugWindow( SiriKali::Windows::lastError() ) ;

			return { false,{} } ;
		}
	} ) ;
}

#else

static LXQt::Wallet::Task::future< result >& _decrypt( const QByteArray& e,const QByteArray& s )
{
	Q_UNUSED( e )
	Q_UNUSED( s )

	return LXQt::Wallet::Task::run< result >( [ & ]()->result{

		return { false,{} } ;
	} ) ;
}

static LXQt::Wallet::Task::future< result >& _encrypt( const QByteArray& e,const QByteArray& s )
{
	Q_UNUSED( e )
	Q_UNUSED( s )

	return LXQt::Wallet::Task::run< result >( [ & ]()->result{

		return { false,{} } ;
	} ) ;
}

#endif

LXQt::Wallet::windows_dpapi::windows_dpapi()
{
}

LXQt::Wallet::windows_dpapi::~windows_dpapi()
{
	this->encrypt() ;
}

void LXQt::Wallet::windows_dpapi::encrypt()
{
	if( m_opened ){

		auto b = _encrypt( this->serializeData(),m_entropy ).await() ;

		if( b.success ){

			windowsKeysStorageData( b.data ) ;
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
	QEventLoop loop ;

	auto exitLoop = [ & ]( bool e ){

		Q_UNUSED( e )

		loop.exit() ;
	} ;

	this->open( walletName,
		    applicationName,
		    std::move( exitLoop ),
		    parent,
		    password,
		    displayApplicationName ) ;

	loop.exec() ;

	return m_opened ;
}

void LXQt::Wallet::windows_dpapi::open( const QString& walletName,
					const QString& applicationName,
					std::function< void( bool ) > function ,
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

	cbd::createInstance( this,w,d,[ this ]( const QString& password,bool create ){

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
	_decrypt( m_data,m_entropy ).then( [ this ]( const result& m ){

		if( m.success ){

			this->deserializeData( m.data ) ;

			m_opened = true ;

			m_walletOpened( true ) ;
		}else{
			using pwd = LXQt::Wallet::password_dialog ;

			auto _cancelled = [ this ](){

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
	} ) ;
}

void LXQt::Wallet::windows_dpapi::openWalletWithPassword( QString e )
{
	this->setEntropy( e ) ;

	_decrypt( m_data,m_entropy ).then( [ this ]( const result& m ){

		bool s = m.success ;

		m_opened = s ;

		m_correctPassword( s ) ;

		if( s ){

			this->deserializeData( m.data ) ;

			m_walletOpened( s ) ;
		}
	} ) ;
}

void LXQt::Wallet::windows_dpapi::deserializeData( const QByteArray& e )
{
	const size_t int_size = sizeof( int ) ;

	int test ;

	auto data = e.constData() ;

	std::memcpy( &test,data,int_size ) ;

	if( test != TEST_VALUE ){

		windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Deleting Stored Data Because It Appears To Be Corrupted.",true ) ;

		return ;
	}

	int s ;

	std::memcpy( &s,data + int_size,int_size ) ;

	windowsDebugWindow( "LXQt:Wallet::Windows_dpapi: Number Of Entries In Wallet Is: " + QString::number( s ) ) ;

	if( s != 0 ){

		const size_t header_size = 2 * sizeof( int ) ;

		data = data + 2 * int_size ;

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

	char buffer[ 2 * int_size ] ;

	std::memcpy( buffer,&TEST_VALUE,int_size ) ;

	int s = m_keys.size() ;

	std::memcpy( buffer + int_size,&s,int_size ) ;

	data.append( buffer,2 * int_size ) ;

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
	using args = LXQt::Wallet::changePassWordDialog::changeArgs ;

	auto change = [ this,function = std::move( function ) ]( const QString& old,
			const QString& New,bool cancelled  )->args{

		if( cancelled ){

			function( false ) ;

			return { false,false } ;
		}

		auto a = windowsKeysStorageData() ;

		auto m = _decrypt( a,old.toUtf8() ).await() ;

		if( m.success ){

			this->setEntropy( New ) ;

			if( !m_opened ){

				m_opened = true ;

				m_keys.clear() ;
				this->deserializeData( m.data ) ;
			}

			this->encrypt() ;

			function( true ) ;

			return { false,false } ;
		}else{
			return { true,false } ;
		}
	} ;

	LXQt::Wallet::changePassWordDialog::changeInstance( this,
							    walletName,
							    applicationName,
							    std::move( change ) ) ;
}

void LXQt::Wallet::windows_dpapi::setImage( const QIcon& e )
{
	this->setWindowIcon( e ) ;
}

LXQt::Wallet::BackEnd LXQt::Wallet::windows_dpapi::backEnd()
{
	return LXQt::Wallet::BackEnd::windows_DPAPI ;
}

QObject * LXQt::Wallet::windows_dpapi::qObject()
{
	return nullptr ;
}
