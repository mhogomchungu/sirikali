/*
 *
 *  Copyright (c) 2014-2015
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

#ifndef SIRITASK_H
#define SIRITASK_H

#include "volumeinfo.h"
#include "task.hpp"
#include "utility.h"
#include "favorites.h"
#include "engines.h"

#include <QVector>
#include <QString>
#include <QStringList>

namespace siritask
{
	struct Engine
	{
		Engine( std::pair< const engines::engine&,QString > m,
			const QString& configFilePath = QString() ) :
			engine( m.first ),configFileName( std::move( m.second ) ),
			configFilePath( configFilePath )
		{
		}
		Engine( const engines::engine& engine ) :
			engine( engine )
		{
		}
		Engine( const engines::engine& engine,
			const QString& configFileName,
			const QString& configFilePath ) :
			engine( engine ),configFileName( configFileName ),
			configFilePath( configFilePath )
		{
		}
		Engine() : engine( engines::instance().getByName( "" ) )
		{
		}

		const engines::engine& engine ;
		QString configFileName ;
		QString configFilePath ;
	} ;

	siritask::Engine mountEngine( const QString& cipherFolder,
				      const QString& configFilePath ) ;

	bool deleteMountFolder( const QString& ) ;

	bool encryptedFolderUnMount( const QString& cipherFolder,
				     const QString& mountPoint,
				     const QString& fileSystem,
				     int numberOfAttempts = 5 ) ;

	engines::engine::cmdStatus encryptedFolderMount( const engines::engine::options&,bool = false ) ;
	engines::engine::cmdStatus encryptedFolderCreate( const engines::engine::options& ) ;
}

#endif // SIRITASK_H
