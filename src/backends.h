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

#ifndef BACKEND_H
#define BACKEND_H

#include <QString>
#include "siritask.h"

class backEnd
{
public:
	static const backEnd& instance() ;
	static bool supported( const QString& ) ;
	enum class Attempt{ First,Second } ;
	static bool supported( const QString&,Attempt ) ;

	struct cmdArgsList
	{
		const QString& exe ;
		const siritask::options& opt ;
		const QString& configFilePath ;
		const QString& cipherFolder ;
		const QString& mountPoint ;
		const bool create ;
	} ;

	struct entity
	{
		QString type ;
		siritask::status exeNotFound ;
		siritask::status name ;
		std::function< QString( const cmdArgsList& ) > command ;

		QString executableFullPath() const ;
	} ;

	backEnd() ;
	const entity& get( const siritask::options& e ) const ;
private:
	std::array< entity,7 > entities ;
};

#endif
