/*
 *
 *  Copyright ( c ) 2011-2015
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

#ifndef FAVORITES_H
#define FAVORITES_H

#include <QString>

#include <vector>
#include <forward_list>

#include "utility.h"
#include "json_parser.hpp"

class favorites
{
public:
	class triState{
	public:
		triState() : m_state( triState::STATES::UNDEFINED )
		{
		}
		triState( bool e ) : m_state( e ? STATES::TRUE : STATES::FALSE )
		{
		}
		void operator=( bool e )
		{
			if( e ){

				m_state = STATES::TRUE ;
			}else{
				m_state = STATES::FALSE ;
			}
		}
		void toggle()
		{
			if( m_state == STATES::TRUE ){

				m_state = STATES::FALSE ;

			}else if( m_state == STATES::FALSE ){

				m_state = STATES::TRUE ;
			}
		}
		bool undefined() const
		{
			return m_state == STATES::UNDEFINED ;
		}
		bool defined() const
		{
			return !this->undefined() ;
		}
		operator bool() const
		{
			return this->True() ;
		}
		bool True() const
		{
			return m_state == STATES::TRUE ;
		}
		bool False() const
		{
			return m_state != STATES::TRUE ;
		}
		static void writeTriState( SirikaliJson& json,
					   const favorites::triState& state,
					   const char * entry )
		{
			if( state.defined() ){

				if( state.True() ){

					json[ entry ] = "true" ;
				}else{
					json[ entry ] = "false" ;
				}
			}else{
				json[ entry ] = "" ;
			}
		}
		static void readTriState( SirikaliJson& json,
					  favorites::triState& state,
					  const char * entry )
		{
			auto s = json.getString( entry ) ;

			if( !s.isEmpty() ){

				state = s == "true" ? true : false ;
			}
		}
	private:
		enum class STATES{ UNDEFINED,TRUE,FALSE } m_state ;
	};

	struct entry
	{
		static const favorites::entry& empty()
		{
			static favorites::entry s ;

			return s ;
		}

		entry() ;
		entry( const QString& volumePath,const QString& mountPath = QString() ) ;

		QString volumePath ;
		QString mountPointPath ;
		QString configFilePath ;
		QString keyFile ;
		QString idleTimeOut ;
		QString identityFile ;
		QString identityAgent ;
		QString mountOptions ;
		QString preMountCommand ;
		QString postMountCommand ;
		QString preUnmountCommand ;
		QString postUnmountCommand ;
		QString internalConfigPath ;

		QByteArray password ;

		bool reverseMode = false ;
		bool volumeNeedNoPassword = false ;

		favorites::triState readOnlyMode ;
		favorites::triState autoMount ;
	};

	static favorites& instance()
	{
		static favorites m ;
		return m ;
	}

	class volEntry{
	public:
		volEntry() : m_favorite( std::addressof( favorites::entry::empty() ) )
		{
		}
		volEntry( const QString& e ) :
			m_favorite( this->entry( e ) )
		{
		}
		template< typename P >
		volEntry( favorites::entry&& e,P&& p ) :
			m_favorite( this->entry( std::move( e ) ) ),
			m_password( std::forward< P >( p ) )
		{
		}
		volEntry( favorites::entry&& e ) :
			m_favorite( this->entry( std::move( e ) ) ),
			m_password( std::move( m_favorite->password ) )
		{
		}
		volEntry( const favorites::entry& e ) :
			m_favorite( std::addressof( e ) ),
			m_password( e.password )
		{
		}
		template< typename P >
		volEntry( const favorites::entry& e,P&& s ) :
			m_favorite( std::addressof( e ) ),
			m_password( std::forward< P >( s ) )
		{
		}
		const favorites::entry& favorite() const
		{
			return *m_favorite ;
		}
		const QByteArray& password() const
		{
			return m_password ;
		}
		template< typename T >
		void setPassword( T&& e )
		{
			m_password = std::forward< T >( e ) ;
		}
		void setAutoMount( bool s ) ;
	private:
		const favorites::entry * entry( favorites::entry&& e )
		{
			utility::debug() << "Favorites managing temporary entry: " + e.volumePath ;
			m_tmpFavorite = std::make_unique< favorites::entry >( std::move( e ) ) ;
			return m_tmpFavorite.get() ;
		}
		std::unique_ptr< favorites::entry > m_tmpFavorite ;
		const favorites::entry * m_favorite ;
		QByteArray m_password ;
		static favorites::entry m_static_favorite_entry ;
	};

	using volumeList = std::vector< volEntry > ;

	enum class error{ ENTRY_ALREADY_EXISTS,FAILED_TO_CREATE_ENTRY,SUCCESS } ;

	error add( const favorites::entry& ) ;

	void replaceFavorite( const favorites::entry&, const favorites::entry& ) ;
	void removeFavoriteEntry( const favorites::entry& ) ;

	const std::vector< favorites::entry >& readFavorites() const ;

	template< typename Function,Task::detail::has_bool_return_type< Function,const favorites::entry& > = 0 >
	bool entries( Function&& function ) const
	{
		for( const auto& it : m_favorites ){

			if( function( it ) ){

				return true ;
			}
		}

		return false ;
	}

	template< typename Function,
		  typename NotFoundFunction,
		  Task::detail::has_bool_return_type< Function,const favorites::entry& > = 0 >
	void entries( Function&& function,NotFoundFunction&& notFound ) const
	{
		if( !entries( std::forward<Function>( function ) ) ){

			notFound() ;
		}
	}

	template< typename Function,Task::detail::has_void_return_type< Function,const favorites::entry& > = 0 >
	void entries( Function&& function ) const
	{
		for( const auto& it : m_favorites ){

			function( it ) ;
		}
	}

	utility2::result_ref< const favorites::entry& > readFavoriteByPath( const QString& configPath ) const ;

	utility2::result< favorites::entry > readFavoriteByFileSystemPath( const QString& configPath ) const ;

	utility2::result_ref< const favorites::entry& > readFavorite( const QString& volumePath,
								      const QString& mountPath = QString() ) const ;

	favorites() ;
private:
	void reload() ;
	std::vector< favorites::entry > m_favorites ;
};

#endif // FAVORITES_H
