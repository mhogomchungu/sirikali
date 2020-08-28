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

#ifndef SIRI_WINFSP_H
#define SIRI_WINFSP_H

#include <QtGlobal>

#include <QString>
#include <QStringList>
#include <QList>
#include <vector>
#include <memory>

#include "task.hpp"
#include "siritask.h"
#include "engines.h"
#include "lxqt_wallet.h"
#include "compatibility.hpp"

namespace SiriKali{
namespace Windows{

struct opts{

	bool create ;
	const engines::engine::args& args ;
	const engines::engine::cmdArgsList& options ;
	const engines::engine& engine ;
	const QByteArray& password ;
} ;

Task::process::result run( const SiriKali::Windows::opts& ) ;

Task::process::result unmount( const QStringList& unMountCommand,const QString& mountPath ) ;

QString volumeProperties( const QString& mountPath ) ;

int terminateProcess( unsigned long pid ) ;

QString engineInstalledDir( const engines::engine& ) ;
QString engineInstalledDir( const QString& key,const QString& value ) ;

QStringList engineInstalledDirs() ;

QString lastError() ;

std::pair< bool,QString > driveHasSupportedFileSystem( const QString& path ) ;

bool mountPointTaken( const QString& ) ;

void updateVolumeList( std::function< void() > ) ;

bool backEndTimedOut( const QString& ) ;

struct mountOptions
{
	mountOptions( const QString& a,const QString& b,const QString& c,
		      const QString& d,const QStringList& e ) :
		mode( a ),subtype( b ),cipherFolder( c ),mountPointPath( d ),fuseOptions( e )
	{
	}
	QString mode ;
	QString subtype ;
	QString cipherFolder ;
	QString mountPointPath ;
	QStringList fuseOptions ;
};

std::vector< mountOptions > getMountOptions() ;

}
}

#endif
