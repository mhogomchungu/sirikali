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

class securefs : public engines::engine
{
public:
	securefs() ;

	bool updatable( bool ) const override ;

	void setUpBinary( bool,QStringList&,const QString& ) const override ;

	void removeExtraFiles( const QString& ) const override ;

	bool onlineArchiveFileName( const QString& ) const override ;

	engines::engine::args command( const QByteArray&,const engines::engine::cmdArgsList&,bool ) const override ;

	engines::engine::cmdStatus commandStatus( const engines::engine::commandStatusOpts& ) const override ;

	engines::engine::status errorCode( const QString&,const QString&,int s ) const override ;

	bool canShowVolumeProperties() const override ;

	bool requiresAPassword( const engines::engine::cmdArgsList& ) const override ;

	void GUICreateOptions( const engines::engine::createGUIOptions& ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;

	void updateOptions( engines::engine::cmdArgsList&,bool creating ) const override ;

	void updateOptions( engines::engine::commandOptions& opts,
			    const engines::engine::cmdArgsList& args,
			    bool creating ) const override ;
private:
	bool useLogFile() const ;
	QByteArray extraLogOutput( const engines::engine::args& ) const ;
} ;
