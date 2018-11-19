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

#include "securefs.h"
#include "commandOptions.h"

#include "securefscreateoptions.h"

securefs::securefs() : engines::engine( "securefs" )
{
}

const QStringList& securefs::names() const
{
	return m_names ;
}

const QStringList& securefs::fuseNames() const
{
	return m_fuseNames ;
}

engines::engine::status securefs::notFoundCode() const
{
	return engines::engine::status::securefsNotFound ;
}

QString securefs::command( const engines::engine::cmdArgsList& args ) const
{
	if( args.create ){

		QString e = "%1 create %2 %3 %4" ;
		return e.arg( args.exe,
			      args.opt.createOptions,
			      args.configFilePath,
			      args.cipherFolder ) ;
	}else{
		commandOptions m( args,m_names.first(),m_names.first() ) ;

		QString exe = "%1 mount %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		if( !utility::platformIsWindows() ){

			exeOptions.add( "-b" ) ;
		}

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		return exe.arg( args.exe,
				exeOptions.get(),
				args.cipherFolder,
				args.mountPoint,
				m.fuseOpts().get() ) ;
	}
}

engines::engine::status securefs::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return engines::engine::status::securefsBadPassword ;

	}else if( e.contains( "winfsp" ) ){

		return engines::engine::status::failedToLoadWinfsp ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

bool securefs::setsCipherPath() const
{
	return false ;
}

QString securefs::configFileArgument() const
{
	return "--config" ;
}

QStringList securefs::configFileNames() const
{
	return { ".securefs.json","securefs.json" } ;
}

bool securefs::autoMountsOnCreate() const
{
	return false ;
}

QString securefs::setPassword( const QString& e ) const
{
	return e + "\n" + e ;
}

bool securefs::hasGUICreateOptions() const
{
	return true ;
}

QString securefs::defaultCreateOptions() const
{
	return QString() ;
}

void securefs::GUICreateOptionsinstance( QWidget * parent,
					 std::function< void( const engines::engine::Options& ) > function ) const
{
	securefscreateoptions::instance( parent,std::move( function ) ) ;
}
