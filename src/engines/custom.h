/*
 *
 *  Copyright (c) 2019
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


#ifndef CUSTOM_BACKEND_H
#define CUSTOM_BACKEND_H

#include "../engines.h"

#include <QStringList>

#include <vector>
#include <memory>

class custom : public engines::engine
{
public:
	static void addEngines( std::vector< std::unique_ptr< engines::engine > >& ) ;

	custom( engines::engine::BaseOptions baseOpts ) ;

	engines::engine::status errorCode( const QString& e,int s ) const override ;

	void GUICreateOptions( const engines::engine::createGUIOptions& ) const override ;
private:
	engines::version m_version ;
} ;

#endif
