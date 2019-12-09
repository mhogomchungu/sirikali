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

#include "unknown.h"

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.hasConfigFile       = false ;
	s.autoMountsOnCreate  = false ;
	s.hasGUICreateOptions = true ;
	s.setsCipherPath      = false ;
	s.supportsMountPathsOnWindows = false ;

	s.configFileArgument = QString() ;

	s.configFileNames = QStringList{} ;

	s.fuseNames = QStringList{} ;
	s.names     = QStringList{} ;

	s.notFoundCode = engines::engine::status::unknown ;

	return s ;
}

unknown::unknown() : engines::engine( _setOptions() )
{
}

engines::engine::args unknown::command( const QByteArray& password,
					const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )
	Q_UNUSED( args )
	return {} ;
}

engines::engine::status unknown::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( e )
	Q_UNUSED( s )
	return engines::engine::status::backendFail ;
}

const QString& unknown::installedVersionString() const
{
	return m_version.get() ;
}

void unknown::GUICreateOptionsinstance( QWidget * parent,engines::engine::function function ) const
{
	Q_UNUSED( parent )
	Q_UNUSED( function )
}
