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

#include <vector>
#include <memory>
#include <functional>

#include <QString>
#include <QStringList>
#include <QWidget>

#include "siritask.h"

class backEnd
{
public:
	static const backEnd& instance() ;
	static QStringList supported() ;

	struct cmdArgsList
	{
		const QString& exe ;
		const siritask::options& opt ;
		const QString& configFilePath ;
		const QString& cipherFolder ;
		const QString& mountPoint ;
		const bool create ;
	} ;

	class engine
	{
	public:
		struct Options{
			Options( QStringList s,bool r ) :
				options( std::move( s ) ),reverseMode( r ),success( true )
			{
			}
			Options( QStringList s ) :
				options( std::move( s ) ),reverseMode( false ),success( true )
			{
			}
			Options( bool r ) :
				reverseMode( r ),success( true )
			{
			}
			Options() : success( false )
			{
			}
			QStringList options ;
			bool reverseMode ;
			bool success ;
		} ;

		engine( const QString& name ) ;
		QString executableFullPath() const ;
		bool isInstalled() const ;
		bool isNotInstalled() const ;
		bool unknown() const ;		
		virtual ~engine() ;
		virtual QString defaultCreateOptions() const = 0 ;
		virtual QString configFileArgument() const = 0 ;
		virtual QString setPassword( const QString& ) const = 0 ;
		virtual QString command( const backEnd::cmdArgsList& args ) const = 0 ;
		virtual siritask::status errorCode( const QString& e,int s ) const = 0 ;
		virtual siritask::status notFoundCode() const = 0 ;		
		virtual const QStringList& names() const = 0 ;
		virtual const QStringList& fuseNames() const = 0 ;
		virtual bool setsCipherPath() const = 0 ;
		virtual bool autoMountsOnCreate() const = 0 ;
		virtual bool hasGUICreateOptions() const = 0 ;
		virtual QStringList configFileNames() const = 0 ;
		virtual void GUICreateOptionsinstance( QWidget * parent,
						       std::function< void( const Options& ) > )  const = 0 ;
	private:
		QString m_name ;
	};

	backEnd() ;
	const engine& getByName( const siritask::options& e ) const ;
	const engine& getByName( const QString& e ) const ;
	const engine& getByFuseName( const QString& e ) const ;
private:
	std::vector< std::unique_ptr< backEnd::engine > > m_backends ;
};

#endif
