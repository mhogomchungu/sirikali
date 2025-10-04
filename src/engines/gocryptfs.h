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

class gocryptfs : public engines::engine
{
public:
	gocryptfs() ;

	explicit gocryptfs( const QString& ) ;

	bool updatable( bool ) const override ;

	void deleteBinPath( const QString& ) const override ;

	void setUpBinary( bool,QStringList&,const QString& ) const override ;

	bool enginesMatch( const QString& ) const override ;

	bool onlineArchiveFileName( const QString& ) const override ;

	void removeExtraFiles( const QString& ) const override ;

	QString installedVersionHack( const QString& ) const override ;

	void setInstalledVersionHack( const QString&,const QString& ) const override ;

	void updateOptions( engines::engine::cmdArgsList&,bool ) const override ;

	engines::engine::status errorCode( const QString& e,const QString& err,int s ) const override ;

	engine::engine::error errorCode( const QString& e ) const override ;

	void GUICreateOptions( const engines::engine::createGUIOptions& ) const override ;

	void GUIMountOptions( const engines::engine::mountGUIOptions& ) const override ;

	void parseVolumeProperties( QString& ) const override ;

	void updateExecutablePaths() const override ;

	void aboutToExit() const override ;

	const QStringList& windowsUnmountCommand() const override ;

	QByteArray prepareBackend() const override ;

	args command( const QByteArray& password,
		      const engines::engine::cmdArgsList& args,
		      bool create ) const override ;
private:
	mutable QString m_sirikaliCppcryptfsExe ;
	mutable QString m_cppcryptfsctl ;
	mutable QString m_cppcryptfs ;
	mutable QStringList m_windowsUnmountCommand ;
	mutable QString m_cppcryptfsPid ;
	QString getcppcryptfsPid() const ;
} ;
