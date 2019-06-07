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
class favorites
{
public:
	enum class type{ sshfs,others } ;

	class readOnly
	{
	public:
		readOnly() ;
		readOnly( bool e ) ;
		operator bool() const ;
		bool onlyRead() const ;
	private:
		bool m_readOnlyVolume = false ;
		bool m_isSet = false ;
	} ;

	class autoMount
	{
	public:
		autoMount() ;
		autoMount( bool e ) ;
		operator bool() const ;
		bool automount() const ;
		void toggle() ;
	private:
		bool m_autoMountVolume = false ;
		bool m_isSet = false ;
	} ;

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
		favorites::readOnly readOnlyMode ;
		favorites::autoMount autoMount ;
	};

	enum class error{ ENTRY_ALREADY_EXISTS,FAILED_TO_CREATE_ENTRY,SUCCESS } ;

	static error add( const favorites::entry& ) ;
	static void replaceFavorite( const favorites::entry&, const favorites::entry& ) ;
	static void removeFavoriteEntry( const favorites::entry& ) ;
	static std::vector< favorites::entry > readFavorites() ;
	static favorites::entry readFavorite( const QString& ) ;
	static void updateFavorites() ;
};

#endif // FAVORITES_H
