/*
 *
 *  Copyright ( c ) 2019
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JSON_H
#define JSON_H

#include "json.h"

#include <QString>
#include <QStringList>
#include <QFile>

class SirikaliJson
{
public:
	using function = std::function< void( const QString& ) > ;

	struct result{
		bool contains ;
		bool exceptionThrown ;
	};

	SirikaliJson( function log = []( const QString& e ){ Q_UNUSED( e ) } ) :
		m_log( std::move( log ) )
	{
	}
	SirikaliJson( const QByteArray& e,function log = []( const QString& e ){ Q_UNUSED( e ) } ) :
		m_log( std::move( log ) )
	{
	        this->getData( e ) ;
	}
	SirikaliJson( const QString& e,function log = []( const QString& e ){ Q_UNUSED( e ) } ) :
		m_log( std::move( log ) )
	{
	        this->getData( e.toUtf8() ) ;
	}
	SirikaliJson( QFile&& e,function log = []( const QString& e ){ Q_UNUSED( e ) } ) :
	        m_log( std::move( log ) )
	{
	        this->getData( e ) ;
	}
	SirikaliJson( QFile& e,function log = []( const QString& e ){ Q_UNUSED( e ) } ) :
	        m_log( std::move( log ) )
	{
	        this->getData( e ) ;
	}
	result contains( const char * key )
	{
		try{
			auto a = m_json.find( key ) ;

			if( a != m_json.end() ){

				return { true,false } ;
			}else{
				return { false,false } ;
			}

		}catch( const std::exception& e ){

			this->logException( e,key ) ;
			return { false,true } ;

		}catch( ... ){

			this->logException( key ) ;
			return { false,true } ;
		}
	}
	template< typename T >
	T get( const char * key,const T& t = T() ) const
	{
		try{
			auto a = m_json.find( key ) ;

			if( a != m_json.end() ){

				return a->get< T >() ;
			}else{
			        if( m_fileName.isEmpty() ){

					m_log( QString( "Warning, Key \"%1\" Not Found" ).arg( key ) ) ;
				}else{
					m_log( QString( "Warning, Key \"%1\" Not Found In File: %2" ).arg( key,m_fileName ) ) ;
				}

				return t ;
			}

		}catch( const std::exception& e ){

			this->logException( e,key ) ;
			return t ;

		}catch( ... ) {

			this->logException( key ) ;
			return t ;
		}
	}
	template< typename E >
	std::vector< E > getVector( const char * key,const std::vector< E > e = {} ) const
	{
		return this->get< std::vector< E > >( key,e ) ;
	}
	QStringList getStringList( const char * key,const QStringList& l ) const
	{
		std::vector< std::string > m ;

		for( const auto& it : l ){

			m.emplace_back( it.toStdString() ) ;
		}

		const auto e = this->get< std::vector< std::string > >( key,m ) ;

		QStringList s ;

		for( const auto& it : e ){

			s.append( it.c_str() ) ;
		}

		return s ;
	}
	QStringList getStringList( const char * key ) const
	{
		QStringList s ;

		auto e = this->get< std::vector< std::string > >( key,m_defaultStringList ) ;

		for( const auto& it : e ){

			s.append( it.c_str() ) ;
		}

		return s ;
	}
	QString getString( const char * key,const QString& defaultValue ) const
	{
		return this->get< std::string >( key,defaultValue.toStdString() ).c_str() ;
	}
	QString getString( const char * key ) const
	{
		return this->get< std::string >( key,m_defaultString ).c_str() ;
	}
	QByteArray getByteArray( const char * key,const QByteArray& defaultValue ) const
	{
		return this->get< std::string >( key,defaultValue.constData() ).c_str() ;
	}
	QByteArray getByteArray( const char * key ) const
	{
		return this->get< std::string >( key,m_defaultString ).c_str() ;
	}
	bool getBool( const char * key,bool s = false ) const
	{
		return this->get< bool >( key,s ) ;
	}
	int getInterger( const char * key,int s = 0 ) const
	{
		return this->get< int >( key,s ) ;
	}
	double getDouble( const char * key,double s = 0.0 ) const
	{
		return this->get< double >( key,s ) ;
	}
	SirikaliJson& operator[]( const char * key )
	{
		m_key = key ;
		return *this ;
	}
	void operator=( const QString& value )
	{
		this->insert( m_key,value.toStdString() ) ;
	}
	void operator=( const QByteArray& value )
	{
		this->insert( m_key,value.constData() ) ;
	}
	void operator=( const QStringList& value )
	{
		std::vector< std::string > m ;

		for( const auto& it : value ){

			m.emplace_back( it.toStdString() ) ;
		}

		this->insert( m_key,m ) ;
	}
	template< typename T >
	void operator=( const T& value )
	{
		this->insert( m_key,value ) ;
	}
	template< typename T >
	void insert( const char * key,const T& value )
	{
		try{
			if( m_passed ){

				m_json[ key ] = value ;
			}

		}catch( const std::exception& e ){

			QString s ;

			if( m_fileName.isEmpty() ){

				s = QString( "Error, Exception Thrown When Adding Key \"%1\"" ).arg( key ) ;
			}else{
				s = QString( "Error, Exception Thrown When Adding Key \"%1\" in File: %2" ).arg( key,m_fileName ) ;
			}

			m_log( s + QString( "\n\nException msg: " ) + e.what() ) ;

			m_passed = false ;

		}catch( ... ) {

			QString s ;

			if( m_fileName.isEmpty() ){

				s = QString( "Error, Exception Thrown When Searching For Key \"%1\"" ).arg( key ) ;
			}else{
				s = QString( "Error, Exception Thrown When Searching For Key \"%1\" in File: %2" ).arg( key,m_fileName ) ;
			}

			m_log( s + QString( "\n\nException msg: Unknown" ) ) ;

			m_passed = false ;
		}
	}
	bool toFile( const QString& path,int indent = 8 ) const
	{
		QFile file( path ) ;

		if( file.open( QIODevice::WriteOnly ) ){

			file.write( m_json.dump( indent ).data() ) ;

			return true ;
		}else{
			return false ;
		}
	}
	QByteArray structure( int indent = 8 ) const
	{
		return m_json.dump( indent ).c_str() ;
	}
	QStringList getTags( const char * tag )
	{
		QStringList s ;

		for( const auto& it : m_json ){

			auto e = it.find( tag ) ;

			if( e != it.end() ){

				s.append( QString::fromStdString( e.value() ) ) ;
			}
		}

		return s ;
	}
	bool passed() const
	{
		return m_passed ;
	}
	bool failed() const
	{
		return !this->passed() ;
	}
private:
	QString exeptionLog( const char * key ) const
	{
		if( key == nullptr ){

			if( m_fileName.isEmpty() ){

				return QString( "Error, Exception Thrown When Parsing Data" ) ;
			}else{
				return QString( "Error, Exception Thrown When Parsing Data in File: %1" ).arg( m_fileName ) ;
			}
		}else{
			if( m_fileName.isEmpty() ){

				return QString( "Error, Exception Thrown When Searching For Key \"%1\"" ).arg( key ) ;
			}else{
				return QString( "Error, Exception Thrown When Searching For Key \"%1\" in File: %2" ).arg( key,m_fileName ) ;
			}
		}
	}
	void logException( const char * key = nullptr ) const
	{
		m_log( this->exeptionLog( key ) + "\n\nException msg: Unknown" ) ;
	}
	void logException( const std::exception& e,const char * key = nullptr ) const
	{
		m_log( this->exeptionLog( key ) + QString( "\n\nException msg: " ) + e.what() ) ;
	}
	void getData( QFile& f )
	{
		m_fileName = f.fileName() ;

		if( !f.isOpen() ){

			if( !f.open( QIODevice::ReadOnly ) ){

				m_log( QString( "Error, Failed To Open File For Reading: %1" ).arg( m_fileName ) ) ;

				return ;
			}
		}

		this->getData( f.readAll() ) ;
	}
	void getData( const QByteArray& e )
	{
		try{
			m_json = nlohmann::json::parse( e.constData() ) ;

		}catch( const std::exception& e ){

			this->logException( e ) ;
			m_passed = false ;

		}catch( ... ){

			this->logException() ;
			m_passed = false ;
		}
	}

	std::vector< std::string > m_defaultStringList ;
	std::string m_defaultString ;
	QString m_fileName ;

	bool m_passed = true ;
	std::function< void( const QString& ) > m_log = []( const QString& e ){ Q_UNUSED( e ) } ;
	const char * m_key ;
	nlohmann::json m_json ;
};

#endif //JSON_H
