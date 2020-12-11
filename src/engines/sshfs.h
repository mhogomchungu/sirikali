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

#include "../engines.h"

class sshfs : public engines::engine
{
public:
	sshfs() ;

	bool requiresAPassword( const engines::engine::cmdArgsList& ) const override ;

	engine::engine::status passAllRequirenments( const engines::engine::cmdArgsList& opt ) const override ;

	const QProcessEnvironment& getProcessEnvironment() const override ;

	engine::engine::error errorCode( const QString& ) const override ;

	void updateOptions( engines::engine::commandOptions& opts,
			    const engines::engine::cmdArgsList& args,
			    bool creating ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;
private:
	mutable QProcessEnvironment m_environment ;
	const engines::versionGreaterOrEqual m_version_greater_or_equal_minimum ;
	const QString m_sshAuthSock ;
} ;
