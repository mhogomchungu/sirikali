/*
 *
 *  Copyright (c) 2022
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

#include <QCoreApplication>
#include <iostream>
#include <QProcess>
#include <QTimer>

#include "task.hpp"

static QString _cmdArgumentValue( const QStringList& l,const QString& arg )
{
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){

		if( l.at( i ) == arg ){

			if( i + 1 < j ){

				return l.at( i + 1 ) ;
			}else{
				return {} ;
			}
		}
	}

	return {} ;
}

static void _checkMounted( const QString& exe,const QByteArray& cf )
{
	auto s = Task::process::run( exe,QStringList{ "-l" } ).await().std_out() ;

	const auto e = s.split( '\n' ) ;

	bool found = false ;

	for( const auto& it : e ){

		if( it.contains( cf ) ){

			found = true ;

			break ;
		}
	}

	if( found ){

		QTimer::singleShot( 2000,[ = ](){

			_checkMounted( exe,cf ) ;
		} ) ;
	}else{
		QCoreApplication::exit( 0 ) ;
	}
}

static void _mount( QCoreApplication&,const QStringList& l )
{
	auto cryptFolder   = _cmdArgumentValue( l,"--cipherPath" ) ;
	auto mountPoint    = _cmdArgumentValue( l,"--mountPath" ) ;
	auto password      = _cmdArgumentValue( l,"--password" ) ;
	auto config        = _cmdArgumentValue( l,"--config" ) ;
	auto cppcryptfsctl = _cmdArgumentValue( l,"--cppcryptfsctl-path" ) ;
	auto mode          = _cmdArgumentValue( l ,"-o" ) ;

	QStringList args{ "-p",password,"-d",mountPoint,"-m",cryptFolder,"-c",config } ;

	if( mode.startsWith( "ro" ) ){

		args.append( "-r" ) ;
	}

	auto m = Task::process::run( cppcryptfsctl,args ).await() ;

	if( m.success() ){

		std::cout << "Mount Success" << std::endl ;

		std::cout << m.std_error().toStdString() << std::endl ;

		auto cf = cryptFolder.toUtf8() ;

		QTimer::singleShot( 2000,[ = ](){

			_checkMounted( cppcryptfsctl,cf ) ;
		} ) ;
	}else{
		auto s = QString::number( m.exit_code() ).toStdString() ;
		std::cout << "Failed To Mount: Error Code: " + s << std::endl ;

		std::cout << m.std_error().toStdString() << std::endl ;
		std::cout << m.std_out().toStdString() << std::endl ;

		QCoreApplication::exit( m.exit_code() ) ;
	}
}

static void _umount( const QStringList& args )
{
	auto mountPoint  = _cmdArgumentValue( args,"--mountPath" ) ;
	auto exe         = _cmdArgumentValue( args,"--cppcryptfsctl-path" ) ;

	auto m = Task::process::run( exe,QStringList{ "-u",mountPoint } ).await() ;

	if( m.success() ){

		std::cout << "Umount Success" << std::endl ;
	}else{
		auto s = QString::number( m.exit_code() ).toStdString() ;
		std::cout << "Failed To Unmount: Error Code: " + s << std::endl ;

		std::cout << m.std_error().toStdString() << std::endl ;
		std::cout << m.std_out().toStdString() << std::endl ;
	}

	QCoreApplication::exit( m.exit_code() ) ;
}

static void _create( const QStringList& args )
{
	auto cipherPath  = _cmdArgumentValue( args,"--cipherPath" ) ;
	auto exe         = _cmdArgumentValue( args,"--cppcryptfsctl-path" ) ;
	auto password    = _cmdArgumentValue( args,"--password" ).toUtf8() ;

	if( password.isEmpty() ){

		std::cout << "Create Failed: Error Code: 1 " << std::endl ;
		std::cout << "Failure Reason: Empty Password" << std::endl ;
	}

	QStringList opts ;

	if( args.contains( "--siv" ) ){

		opts.append( "--siv" ) ;
	}

	if( args.contains( "--plaintext" ) ){

		opts.append( "--plaintext" ) ;
	}

	opts.append( "--init=" + cipherPath ) ;

	auto m = Task::process::run( exe,opts,-1,password ).await() ;

	if( m.success() ){

		std::cout << "Create Success" << std::endl ;
	}else{
		auto s = QString::number( m.exit_code() ).toStdString() ;
		std::cout << "Create Failed: Error Code: " + s << std::endl ;

		std::cout << m.std_error().toStdString() << std::endl ;
		std::cout << m.std_out().toStdString() << std::endl ;
	}

	QCoreApplication::exit( m.exit_code() ) ;
}

static void run( QCoreApplication& app,const QStringList& args )
{
	if( args.contains( "--mount" ) ){

		_mount( app,args ) ;

	}else if( args.contains( "--umount" ) ){

		_umount( args ) ;

	}else if( args.contains( "--create" ) ){

		_create( args ) ;
	}else{
		std::cout << "Unknown command" << std::endl ;

		QCoreApplication::exit( 255 ) ;
	}
}

int main( int argc,char * argv[] )
{
	QCoreApplication a( argc,argv ) ;

	QTimer::singleShot( 0,[ & ](){

		run( a,a.arguments() ) ;
	} ) ;

	return a.exec() ;
}
