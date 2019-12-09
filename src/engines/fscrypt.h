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

	engines::engine::status unmount( const QString& cipherFolder,
					 const QString& mountPoint,
					 int maxCount ) const override ;

	QStringList mountInfo( const QStringList& ) const override ;

	const engines::engine& proveEngine( const QString& cipherPath ) const override ;

	void updateVolumeList( const engines::engine::options& ) const override ;

	Task::future< QString >& volumeProperties( const QString& cipherFolder,
						   const QString& mountPoint ) const override ;

	engines::engine::status errorCode( const QString& e,int s ) const override ;

	engines::engine::args command( const QByteArray& password,
				       const engines::engine::cmdArgsList& args ) const override ;

	const QString& installedVersionString() const override ;

	void GUICreateOptionsinstance( QWidget * parent,engines::engine::function ) const override ;
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

	QString userOption() const ;
	engines::exeFullPath m_exeFullPath ;
	engines::version m_version ;
} ;
