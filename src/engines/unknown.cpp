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
#include "commandOptions.h"

unknown::unknown() : engines::engine( QString() )
{
}

const QStringList& unknown::names() const
{
	return m_names ;
}

const QStringList& unknown::fuseNames() const
{
	return m_fuseNames ;
}

siritask::status unknown::notFoundCode() const
{
	return siritask::status::unknown ;
}

QString unknown::command( const engines::cmdArgsList& args ) const
{
	Q_UNUSED( args ) ;
	return QString() ;
}

siritask::status unknown::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( e ) ;
	Q_UNUSED( s ) ;
	return siritask::status::backendFail ;
}

bool unknown::setsCipherPath() const
{
	return false ;
}

QString unknown::configFileArgument() const
{
	return QString() ;
}

QStringList unknown::configFileNames() const
{
	return {} ;
}

bool unknown::autoMountsOnCreate() const
{
	return true ;
}

QString unknown::setPassword( const QString& e ) const
{
	Q_UNUSED( e ) ;
	return QString() ;
}

bool unknown::hasGUICreateOptions() const
{
	return false ;
}

QString unknown::defaultCreateOptions() const
{
	return QString() ;
}

void unknown::GUICreateOptionsinstance( QWidget * parent,
				      std::function< void( const engines::engine::Options& ) > function ) const
{
	Q_UNUSED( parent ) ;
	Q_UNUSED( function ) ;
}
