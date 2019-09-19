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
#include "utility.h"
#include "json_parser.hpp"

class favorites
{
public:
	enum class type{ sshfs,others } ;

	class triState{
	public:
		triState() : m_state( triState::STATES::UNDEFINED )
		{
		}
		triState( bool e ) : m_state( e ? STATES::TRUE : STATES::FALSE )
		{
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

				state = favorites::triState( s == "true" ? true : false ) ;
			}
		}
	private:
		enum class STATES{ UNDEFINED,TRUE,FALSE } m_state ;
	};

	struct entry
	{
		entry() ;
		entry( const QString& volumePath ) ;

		QString volumePath ;
		QString mountPointPath ;
		QString configFilePath ;
		QString idleTimeOut ;
		QString mountOptions ;
		QString preMountCommand ;
		QString postMountCommand ;
		QString preUnmountCommand ;
		QString postUnmountCommand ;

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

	using volumeList = std::vector< std::pair< favorites::entry,QByteArray > > ;

	enum class error{ ENTRY_ALREADY_EXISTS,FAILED_TO_CREATE_ENTRY,SUCCESS } ;

	error add( const favorites::entry& ) ;

	void replaceFavorite( const favorites::entry&, const favorites::entry& ) ;
	void removeFavoriteEntry( const favorites::entry& ) ;

	std::vector< favorites::entry > readFavorites() const ;

	utility::result< favorites::entry > readFavorite( const QString&,const QString& = QString() ) const ;

	void updateFavorites() ;
};

#endif // FAVORITES_H
