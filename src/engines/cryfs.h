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

class cryfs : public engines::engine
{
public:
	cryfs() ;

	bool takesTooLongToUnlock() const override ;

	void updateOptions( engines::engine::cmdArgsList&,bool ) const override ;

	const QProcessEnvironment& getProcessEnvironment() const override ;

	engines::engine::status errorCode( const QString& e,int s ) const override ;

	engines::engine::status passAllRequirenments( const engines::engine::cmdArgsList& opt ) const override ;

	void GUICreateOptions( const createGUIOptions& ) const override ;

	void GUIMountOptions( const mountGUIOptions& ) const override ;
private:
	QProcessEnvironment setEnv() const ;
	const QProcessEnvironment m_env ;
	const engines::versionGreaterOrEqual m_version_greater_or_equal_0_10_0 ;
	const engines::versionGreaterOrEqual m_version_greater_or_equal_0_9_9 ;
} ;
