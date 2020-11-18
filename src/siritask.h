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
#include "secrets.h"

#include <QVector>
#include <QString>
#include <QStringList>

namespace siritask
{
	bool deleteMountFolder( const QString& ) ;

	struct unmount{

		const QString& cipherFolder ;
		const QString& mountPoint ;
		const engines::engine& engine ;
		int numberOfAttempts ;
	} ;

	struct mount{

		const engines::engine::cmdArgsList& options ;
		bool reUseMP ;
		const engines::engineWithPaths& engine ;
	} ;

	struct create{

		const engines::engine::cmdArgsList& options ;
		const engines::engine& engine ;
	} ;

	engines::engine::cmdStatus encryptedFolderUnMount( const siritask::unmount& ) ;

	engines::engine::cmdStatus encryptedFolderMount( const siritask::mount& ) ;

	engines::engine::cmdStatus encryptedFolderMount( const engines::engine::cmdArgsList& s ) ;

	engines::engine::cmdStatus encryptedFolderCreate( const siritask::create& ) ;
}

#endif // SIRITASK_H
