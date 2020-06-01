/*
 *
 *  Copyright (c) 2017
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

#ifndef UTILITY_TWO_H
#define UTILITY_TWO_H

#include <functional>
#include <memory>
#include <type_traits>

namespace utility2
{
	/*
	 * This method takes a function that returns a resource,a function that deletes
	 * the resource and arguments that are to be passed to the function that returns a
	 * resource.
	 *
	 * example usecase of a function:
	 *
	 * auto woof = utility2::unique_rsc( ::fopen,::fclose,"/woof/foo/bar","r" ) ;
	 */
	template< typename Function,typename Deleter,typename ... Arguments >
	auto unique_rsc( Function&& function,Deleter&& deleter,Arguments&& ... args )
	{
		using A = std::remove_pointer_t< std::result_of_t< Function( Arguments&& ... ) > > ;
		using B = std::decay_t< Deleter > ;

		return std::unique_ptr< A,B >( function( std::forward< Arguments >( args ) ... ),
					       std::forward< Deleter >( deleter ) ) ;
	}

	/*
	 * This function takes a type,a deleter for the type and optional arguments the
	 * construction of the object of the given type need.
	 *
	 * example:
	 * auto woof = unique_ptr<Foo>(foo_deleter, arg1, arg2, argN);
	 * auto woof = unique_ptr<Foo>(foo_deleter);
	 *
	 * The deleter must be a function that takes a single argument of type "Foo *".
	 */
	template< typename Type,typename Deleter,typename ... Arguments >
	auto unique_ptr( Deleter&& deleter,Arguments&& ... args )
	{
		auto create_object = []( Arguments&& ... args ){

			if( sizeof ... ( args ) == 0 ){

				return new Type() ;
			}else{
				return new Type( std::forward< Arguments >( args ) ... ) ;
			}
		} ;

		return unique_rsc( std::move( create_object ),
				   std::forward< Deleter >( deleter ),
				   std::forward< Arguments >( args ) ... ) ;
	}

	/*
	 * This function takes a type,a deleter for the type and optional arguments the
	 * construction of the object of the given type need.
	 *
	 * example:
	 * Foo *xxx = new Foo(12,"bar");
	 * auto woof = unique_ptr(xxx, foo_deleter);
	 *
	 * The deleter must be a function that takes a single argument of type "Foo*".
	 *
	 */
	template< typename Type,typename Deleter >
	auto unique_ptr( Type type,Deleter&& deleter )
	{
		return unique_rsc( []( auto arg ){ return arg ; },
				   std::forward< Deleter >( deleter ),type ) ;
	}

	/*
	 * Example use cases of this method
	 *
	 * auto exe  = utility2::unique_qptr< QProcess >() ;
	 * auto exe1 = utility2::unique_qptr< QProcess >( "ls" ) ;
	 */
	template< typename Type,typename ... Arguments >
	auto unique_qptr( Arguments&& ... args )
	{
		return utility2::unique_ptr< Type >( []( Type * e ){ e->deleteLater() ; },
						     std::forward< Arguments >( args ) ... ) ;
	}

	template< typename Type >
	auto unique_qptr( Type e )
	{
		return unique_rsc( []( auto arg ){ return arg ; },
				   []( Type e ){ e->deleteLater() ; },e ) ;
	}

	namespace detail
	{
		template< typename E,typename F,typename G >
		void stringListToStrings( const F& s,G n,G k,E& e )
		{
			if( n < k ){

				e = s.at( n ) ;
			}
		}

		template< typename E,typename F,typename G,typename ... T >
		void stringListToStrings( const E& s,G n,G k,F& e,T& ... t )
		{
			if( n < k ){

				e = s.at( n ) ;
				utility2::detail::stringListToStrings( s,n + 1,k,t ... ) ;
			}
		}
	}

	template< typename E,typename ... F >
	void stringListToStrings( const E& s,F& ... t )
	{
		using type_t = decltype( s.size() ) ;

		utility2::detail::stringListToStrings( s,type_t( 0 ),s.size(),t ... ) ;
	}
}

namespace utility2
{
	class raii
	{
	public:
		raii( std::function< void() > s ) : m_function( std::move( s ) )
		{
		}
		~raii()
		{
			if( m_run ){

				m_function() ;
			}
		}
		void cancel()
		{
			m_run = false ;
		}
	private:
		bool m_run = true ;
		std::function< void() > m_function ;
	};

	template< typename T >
	class result
	{
	public:
		result()
		{
		}
		result( T e ) : m_valid( true ),m_value( std::move( e ) )
		{
		}
		T * operator->()
		{
			return &m_value ;
		}
		const T * operator->() const
		{
			return &m_value ;
		}
		T& operator*()
		{
			return m_value ;
		}
		const T& operator*() const
		{
			return m_value ;
		}
		operator bool()
		{
			return m_valid ;
		}
		bool has_value() const
		{
			return m_valid ;
		}
		T& value()
		{
			return m_value ;
		}
		const T& value() const
		{
			return m_value ;
		}
		T&& RValue()
		{
			return std::move( m_value ) ;
		}
		void set( T value )
		{
			m_value = std::move( value ) ;
			m_valid = true ;
		}
	private:
		bool m_valid = false ;
		T m_value ;
	} ;
}

#endif
