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

#include <QString>
#include <QStringList>
#include <QFile>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class SirikaliJson
{
public:
	using function = std::function< void( const QString& ) > ;

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
	std::vector< int > getIntVector( const char * key ) const
	{
		auto s = m_json.value( key ) ;

		if( s.isArray() ){

			std::vector< int > m ;

			const auto arr = s.toArray() ;

			for( const auto& it : arr ){

				m.emplace_back( it.toInt() ) ;
			}

			return m ;
		}else{
			return {} ;
		}
	}
	QStringList getStringList( const char * key ) const
	{
		auto s = m_json.value( key ) ;

		if( s.isArray() ){

			QStringList m ;

			const auto arr = s.toArray() ;

			for( const auto& it : arr ){

				m.append( it.toString() ) ;
			}

			return m ;
		}else{
			return {} ;
		}
	}
	QString getString( const char * key,const QString& defaultValue ) const
	{
		return m_json.value( key ).toString( defaultValue ) ;
	}
	QString getString( const char * key ) const
	{
		return m_json.value( key ).toString() ;
	}
	QByteArray getByteArray( const char * key,const QByteArray& defaultValue ) const
	{
		return this->getString( key,defaultValue ).toUtf8() ;
	}
	QByteArray getByteArray( const char * key ) const
	{
		return this->getString( key ).toUtf8() ;
	}
	bool getBool( const char * key,bool s = false ) const
	{
		return m_json.value( key ).toBool( s ) ;
	}
	int getInterger( const char * key,int s = 0 ) const
	{
		return m_json.value( key ).toInt( s ) ;
	}
	double getDouble( const char * key,double s = 0.0 ) const
	{
		return m_json.value( key ).toDouble( s ) ;
	}
	QStringList getTags( const char * tag )
	{
		QStringList s ;

		const QJsonArray arr = m_doc.array() ;

		for( const auto& it : arr ){

			if( it.isObject() ){

				auto value = it.toObject().value( tag ) ;

				if( value.isString() ){

					s.append( value.toString() ) ;
				}
			}
		}

		return s ;
	}
	SirikaliJson& operator[]( const char * key )
	{
		m_key = key ;
		return *this ;
	}
	void operator=( const char * value )
	{
		m_json.insert( m_key,value ) ;
	}
	void operator=( const QString& value )
	{
		m_json.insert( m_key,value ) ;
	}
	void operator=( bool value )
	{
		m_json.insert( m_key,value ) ;
	}
	void operator=( const QByteArray& value )
	{
		m_json.insert( m_key,QString( value ) ) ;
	}
	void operator=( const std::vector< int >& value )
	{
		QJsonArray arr ;

		for( const auto& it : value ){

			arr.append( it ) ;
		}

		m_json.insert( m_key,arr ) ;
	}
	void operator=( int value )
	{
		m_json.insert( m_key,QString( value ) ) ;
	}
	void operator=( const QStringList& value )
	{
		QJsonArray arr ;

		for( const auto& it : value ){

			arr.append( it ) ;
		}

		m_json.insert( m_key,arr ) ;
	}
	bool toFile( const QString& path ) const
	{
		QFile file( path ) ;

		if( file.open( QIODevice::WriteOnly ) ){

			file.write( this->structure() ) ;

			return true ;
		}else{
			return false ;
		}
	}
	QByteArray structure() const
	{
		return QJsonDocument( m_json ).toJson( QJsonDocument::JsonFormat::Indented ) ;
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
	void logException( const QString& e ) const
	{
		if( m_fileName.isEmpty() ){

			QString m = "Error, Malformed Json Structure Encountered" ;

			m_log( m + "\n\n" + e ) ;
		}else{
			QString m = QString( "Error, Malformed Json Structure Encountered in File: %1" ).arg( m_fileName ) ;

			m_log( m + "\n\n" + e ) ;
		}
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
		QJsonParseError error ;
		m_doc = QJsonDocument::fromJson( e,&error ) ;

		if( error.error == QJsonParseError::NoError ){

			if( m_doc.isObject() ){

				m_passed = true ;

				m_json = m_doc.object() ;
			}else{
				m_passed = false ;
				this->logException( "Error Reason: Expected an object and got something else" ) ;
			}
		}else{
			m_passed = false ;
			this->logException( "Error Reason: " + error.errorString() ) ;
		}
	}

	QString m_fileName ;
	bool m_passed ;
	std::function< void( const QString& ) > m_log = []( const QString& e ){ Q_UNUSED( e ) } ;
	const char * m_key ;
	QJsonObject m_json ;
	QJsonDocument m_doc ;
};

#endif //JSON_H
