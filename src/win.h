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

int terminateProcess( unsigned long pid ) ;

QString engineInstalledDir( const engines::engine& ) ;
QString engineInstalledDir( const QString& key,const QString& value ) ;

QStringList engineInstalledDirs() ;

QString lastError() ;

std::pair< bool,QString > driveHasSupportedFileSystem( const QString& path ) ;

}

}

#endif
