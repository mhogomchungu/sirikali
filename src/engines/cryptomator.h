/*
 *
 *  Copyright (c) 2020
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

class cryptomator : public engines::engine
{
public:
	cryptomator() ;

	bool updatable( bool ) const override ;

	bool onlineArchiveFileName( const QString& ) const override ;

	bool enginesMatch( const QString& ) const override ;

	void deleteBinPath( const QString& ) const override ;

	void setUpBinary( bool,QStringList&,const QString& ) const override ;

	QString setExecutablePermissions( const QString& ) const override ;

	void changeEnvironment( const QString&,const QStringList& ) const override ;

	engines::engine::args command( const QByteArray&,const engines::engine::cmdArgsList&,bool ) const override ;

	void updateOptions( QStringList&,const engines::engine::cmdArgsList&,bool creating ) const override ;

	engines::engine::ownsCipherFolder ownsCipherPath( const QString& cipherPath,
							  const QString& configPath ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;

	engines::engine::status passAllRequirenments( const engines::engine::cmdArgsList& opt ) const override ;
private:
	const engines::versionGreaterOrEqual m_version_greater_or_equal_0_6_0 ;
} ;
