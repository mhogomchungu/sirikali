/*
 *
 *  Copyright (c) 2018
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMANDOPTIONS_H
#define COMMANDOPTIONS_H

#include "../engines.h"

class commandOptions{
public:
	enum class separator{ space,equal_sign } ;

	class Options{
	public:
		Options( QString m,QString s ) :
			m_options( std::move( m ) ),m_separator( s )
		{
		}
		bool doesNotContain( const QString& key ) const
		{
			return !this->contains( key ) ;
		}
		bool contains( const QString& key ) const
		{
			return m_options.contains( key ) ;
		}
		void addPair( const QString& key,const QString& value,separator s )
		{
			if( s == separator::space ){

				this->add( key + " " + value ) ;
			}else{
				this->add( key + "=" + value ) ;
			}
		}
		template< typename E >
		void add( E&& e )
		{
			if( m_options.isEmpty() ){

				m_options = std::forward< E >( e ) ;
			}else{
				m_options += m_separator + std::forward< E >( e ) ;
			}
		}
		template< typename E,typename ... T >
		void add( E&& e,T&& ... m )
		{
			this->add( std::forward< E >( e ) ) ;
			this->add( std::forward< T >( m ) ... ) ;
		}
		const QString& get() const
		{
			return m_options ;
		}
	private:
		QString m_options ;
		QString m_separator ;
	};

	class fuseOptions : public Options
	{
	public:
		fuseOptions( QString m ) : Options( std::move( m ),"," )
		{
		}
		void addPair( const QString& key,const QString& value,
			      separator s = separator::equal_sign )
		{
			Options::addPair( key,value,s ) ;
		}
	} ;

	class exeOptions : public Options
	{
	public:
		exeOptions( QString m ) : Options( std::move( m )," " )
		{
		}
		void addPair( const QString& key,const QString& value,
			      separator s = separator::space )
		{
			Options::addPair( key,value,s ) ;
		}
	} ;

	commandOptions( const engines::cmdArgsList& e,const QString& f,const QString& g = QString() ) :
		m_cmdArgsList( e ),m_type( f ),m_subType( g.isEmpty() ? "" : ",subtype=" + g )
	{
		for( const auto& it : utility::split( m_cmdArgsList.opt.mountOptions,',' ) ) {

			if( it.startsWith( '-' ) ){

				m_exeOptions += it + " " ;
			}else{
				m_fuseOptions += it + "," ;
			}
		}

		if( m_exeOptions.endsWith( " " ) ){

			m_exeOptions.remove( m_exeOptions.size() - 1,1 ) ;
		}

		if( m_fuseOptions.endsWith( "," ) ){

			m_fuseOptions.remove( m_fuseOptions.size() - 1,1 ) ;
		}

		if( !utility::platformIsLinux() && !m_fuseOptions.contains( "volname=" ) ){

			QString s ;

			if( utility::platformIsOSX() ){

				s = utility::split( e.opt.plainFolder,'/' ).last() ;
			}else{
				s = utility::split( e.opt.cipherFolder,'/' ).last() ;
			}

			if( !s.isEmpty() ){

				if( m_fuseOptions.isEmpty() ){

					m_fuseOptions = "volname=" + utility::Task::makePath( s ) ;
				}else{
					m_fuseOptions += ",volname=" + utility::Task::makePath( s ) ;
				}
			}
		}
	}
	commandOptions::exeOptions exeOptions() const
	{
		return m_exeOptions ;
	}
	commandOptions::fuseOptions fuseOpts() const
	{
		QString e = [ & ](){

			if( m_cmdArgsList.opt.ro ){

				return " -o ro,fsname=%1@%2%3" ;
			}else{
				return " -o rw,fsname=%1@%2%3" ;
			}
		}() ;

		if( m_fuseOptions.isEmpty() ){

			return e.arg( m_type,m_cmdArgsList.cipherFolder,m_subType ) ;
		}else{
			return e.arg( m_type,m_cmdArgsList.cipherFolder,m_subType ) + "," + m_fuseOptions ;
		}
	}
private:
	const engines::cmdArgsList& m_cmdArgsList ;
	QString m_exeOptions ;
	QString m_fuseOptions ;
	QString m_type ;
	QString m_subType ;
};

#endif
