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
		Engine() : m_engine( std::addressof( engines::instance().getUnKnown() ) )
		{
		}
		Engine( const engines::engine& engine,
			const QString& cipherFolder,
			const QString& configFilePath ) :
			m_engine( std::addressof( engine ) ),
			m_configFilePath( configFilePath ),
			m_cipherFolder( cipherFolder )
		{
		}
		Engine( const engines::engine& engine ) :
			m_engine( std::addressof( engine ) )
		{
		}
		Engine( const QString& engine ) :
			m_engine( std::addressof( engines::instance().getByName( engine ) ) )
		{
		}
		Engine( const opts& e ) :
			m_engine( std::addressof( e.engine ) ),
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
		const engines::engine& engine() const
		{
			return *m_engine ;
		}
	private:
		const engines::engine * m_engine ;
		QString m_configFilePath ;
		QString m_cipherFolder ;
	} ;

	siritask::Engine mountEngine( const QString& cipherFolder,
				      const QString& configFilePath,
				      const siritask::Engine& engine = siritask::Engine() ) ;

	utility::result< utility::Task > unmountVolume( const QString& exe,
							const QString& mountPoint,
							bool usePolkit ) ;

	bool unmountVolume( const QString& mountPoint,const QString& unMountCommand,int maxCount ) ;

	bool deleteMountFolder( const QString& ) ;

	class taskResult{
	public:
		taskResult()
		{
		}
		taskResult( bool s,const engines::engine& e ) :
			m_success( s ),m_engine( std::addressof( e ) )
		{
		}
		taskResult( engines::engine::cmdStatus c ) :
			m_success( c == engines::engine::status::success ),
			m_cmdStatus( c ),
			m_engine( std::addressof( engines::instance().getUnKnown() ) )
		{
		}
		taskResult( const engines::engine::cmdStatus& c,const engines::engine& e ) :
			m_success( c == engines::engine::status::success ),
			m_cmdStatus( c ),
			m_engine( std::addressof( e ) )
		{
		}
		bool backendDoesNotAutoRefresh() const
		{
			return !this->engine().autorefreshOnMountUnMount() ;
		}
		bool success() const
		{
			return m_success ;
		}
		const engines::engine::cmdStatus& cmdStatus() const
		{
			return m_cmdStatus ;
		}
		const engines::engine& engine() const
		{
			return *m_engine ;
		}
	private:
		bool m_success ;
		engines::engine::cmdStatus m_cmdStatus ;
		const engines::engine * m_engine ;
	} ;

	siritask::taskResult encryptedFolderUnMount( const QString& cipherFolder,
						     const QString& mountPoint,
						     const QString& fileSystem,
						     int numberOfAttempts ) ;

	siritask::taskResult encryptedFolderMount( const engines::engine::options&,
						   bool = false,
						   const siritask::Engine& = siritask::Engine() ) ;

	siritask::taskResult encryptedFolderCreate( const engines::engine::options& ) ;
}

#endif // SIRITASK_H
