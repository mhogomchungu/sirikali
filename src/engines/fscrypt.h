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

#include "../engines.h"
#include <QStringList>

class fscrypt : public engines::engine
{
public:
	fscrypt() ;

	engines::engine::status unmount( const engines::engine::unMount& e ) const override ;

	volumeInfo::List mountInfo( const volumeInfo::List& ) const override ;

	engines::engine::ownsCipherFolder ownsCipherPath( const QString& cipherPath,
							  const QString& configPath ) const override ;

	utility2::LOGLEVEL allowLogging( const QStringList& ) const override ;

	bool requiresAPassword( const engines::engine::cmdArgsList& ) const override ;

	void updateOptions( engines::engine::cmdArgsList&,bool ) const override ;

	void updateVolumeList( const engines::engine::cmdArgsList& ) const override ;

	Task::future< QString >& volumeProperties( const QString& cipherFolder,
						   const QString& mountPoint ) const override ;

	engines::engine::status errorCode( const QString& e,int s ) const override ;

	void GUICreateOptions( const engines::engine::createGUIOptions& ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;
private:
	class unlockedVolumeList{
	public:
		unlockedVolumeList() ;
		QStringList getList() const ;
		void addEntry( const QString& ) ;
		void removeEntry( const QString& ) ;
	private:
		void updateList( const QStringList& ) ;
		const QString m_configFilePath ;
		const char * m_keyName = "unlockedList" ;
	} mutable m_unlockedVolumeManager ;

	engines::versionGreaterOrEqual m_versionGreatorOrEqual_0_2_6 ;
} ;
