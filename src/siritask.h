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
	class Engine
	{
	public:
		struct opts{
			const engines::engine& engine ;
			const QString& configFilePath ;
			const QString& cipherFolder ;
		};
		Engine() :
			m_engine( engines::instance().getUnKnown() )
		{
		}
		Engine( const engines::engine& engine ) :
			m_engine( engine )
		{
		}
		Engine( const QString& engine ) :
			m_engine( engines::instance().getByName( engine ) )
		{
		}
		Engine( const siritask::Engine::opts& e ) :
			m_engine( e.engine ),
			m_configFilePath( e.configFilePath ),
			m_cipherFolder( e.cipherFolder )
		{
		}
		const QString& cipherFolder() const
		{
			return m_cipherFolder ;
		}
		const QString& configFilePath() const
		{
			return m_configFilePath ;
		}
		const engines::engine& get() const
		{
			return m_engine.get() ;
		}
	private:
		engines::engine::Wrapper m_engine ;
		QString m_configFilePath ;
		QString m_cipherFolder ;
	} ;

	struct mount{

		const QString& cipherFolder ;
		const QString& configFilePath ;
		const siritask::Engine& engine ;
	};

	siritask::Engine mountEngine( const siritask::mount& ) ;

	utility::result< utility::Task > unmountVolume( const QString& exe,
							const QString& mountPoint,
							bool usePolkit ) ;

	engines::engine::status unmountVolume( const QString& mountPoint,
					       const QString& unMountCommand,
					       int maxCount ) ;

	bool deleteMountFolder( const QString& ) ;

	struct unmount{

		const QString& cipherFolder ;
		const QString& mountPoint ;
		const QString& fileSystem ;
		int numberOfAttempts ;
	};

	engines::engine::cmdStatus encryptedFolderUnMount( const siritask::unmount& ) ;

	engines::engine::cmdStatus encryptedFolderMount( const engines::engine::options&,
							 bool = false,
							 const siritask::Engine& = siritask::Engine() ) ;

	engines::engine::cmdStatus encryptedFolderCreate( const engines::engine::options&,
							  const engines::engine& ) ;
}

#endif // SIRITASK_H
