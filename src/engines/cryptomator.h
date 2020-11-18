/*
 *
 *  Copyright (c) 2020
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

#include "../engines.h"

struct cryptomator : public engines::engine
{
	cryptomator() ;

	terminate_result terminateProcess( const terminate_process& e ) const override ;

	engines::engine::ownsCipherFolder ownsCipherPath( const QString& cipherPath,
							  const QString& configPath ) const override ;

	engines::engine::status errorCode( const QString& e,int s ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;

	void updateOptions( QStringList&,
	                    const engines::engine::cmdArgsList& args,
	                    bool creating ) const override ;
} ;
