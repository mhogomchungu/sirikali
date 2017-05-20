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

#ifndef STRINGLISTTOSTRINGS_H
#define STRINGLISTTOSTRINGS_H

namespace stringListToStrings
{
	template< typename E,typename F,typename G >
	void _private_convert( const F& s,G n,G k,E& e )
	{
		if( n < k ){

			e = s.at( n ) ;
		}
	}

	template< typename E,typename F,typename G,typename ... T >
	void _private_convert( const E& s,G n,G k,F& e,T& ... t )
	{
		if( n < k ){

			e = s.at( n ) ;
			_private_convert( s,n + 1,k,t ... ) ;
		}
	}

	template< typename E,typename ... F >
	void convert( const E& s,F& ... t )
	{
		_private_convert( s,static_cast< decltype( s.size() ) >( 0 ),s.size(),t ... ) ;
	}
}

#endif
