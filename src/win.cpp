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

#include "win.h"
#include "utility.h"
#include "mountinfo.h"
#include "settings.h"

#include <utility>
#include <tuple>

#include <QtGlobal>
#include <QSettings>
#include <QString>

namespace SiriKali
{
namespace Windows
{

#ifdef Q_OS_WIN

#include <windows.h>

int terminateProcess( unsigned long pid )
{
	FreeConsole() ;

	if( AttachConsole( pid ) == TRUE ) {

		/*
		 * Add a fake Ctrl-C handler for avoid instant kill in this console
		 * WARNING: do not revert it or current program will also killed
		 */

		SetConsoleCtrlHandler( nullptr,true ) ;

		if( GenerateConsoleCtrlEvent( CTRL_C_EVENT,0 ) == TRUE ){

			return 0 ;
		}
	}

	return 1 ;
}

static auto _msg( DWORD err )
{
	char * s = nullptr ;

	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER ;

	auto m = FormatMessageA( flags,
				nullptr,
				err,
				MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
				reinterpret_cast< char * >( &s ),
				0,
				nullptr ) ;

	return std::make_pair( m > 0,utility2::unique_ptr( s,[]( char * e ){ LocalFree( e ) ; } ) ) ;
}

QString lastError()
{
	auto a = _msg( GetLastError() ) ;

	if( a.first ){

		return a.second.get() ;
	}else{
		return "SiriKali::Win: Failed To Get Last Error" ;
	}
}

static QString _errorMsg( DWORD err,const QString& path )
{
	auto a = _msg( err ) ;

	if( a.first ){

		return "SiriKali::Error: Error On Path \"" + path + "\" : " + QString::fromLatin1( a.second.get() ) ;
	}else{
		return "SiriKali::Error: Unknown Error Has Occurred On Path \"" + path + "\"" ;
	}
}

std::pair< bool,QString > driveHasSupportedFileSystem( const QString& path )
{
	auto aa = path.mid( 0,1 ).toUtf8() ;

	std::array< char,4 >rpath = { aa[ 0 ],':','\\','\0' } ;

	std::array< char,MAX_PATH + 1 > fsname ;

	std::fill( fsname.begin(),fsname.end(),'\0' ) ;

	if( GetVolumeInformationA( rpath.data(),nullptr,0,nullptr,nullptr,nullptr,fsname.data(),fsname.size() ) ) {

		auto m = fsname.data() ;

		for( const auto& it : settings::instance().supportedFileSystemsOnMountPaths() ){

			if( it == m ){

				return { true,QString() } ;
			}
		}

		return { false,QString( "SiriKali::Error: \"%1\" File System On Path \"%2\" Is Not Supported." ).arg( m,path ) } ;
	}else{
		return { false,_errorMsg( GetLastError(),path ) } ;
	}
}

QString applicationDirPath()
{
	std::array< char,4096 > buffer ;

	GetModuleFileNameA( nullptr,buffer.data(),static_cast< DWORD >( buffer.size() ) ) ;

	auto m = QDir::fromNativeSeparators( buffer.data() ) ;
	auto s = m.lastIndexOf( '/' ) ;

	if( s != -1 ){

		m.truncate( s ) ;
	}

	return m ;
}

static QString _readRegistry( const QString&,const QString& )
{
	return {} ;
}

template< typename E,typename ... T >
static QString _readRegistry( const QString& key,const QString& value,const E& first,T&& ... rest )
{
	QSettings settings( "HKEY_LOCAL_MACHINE\\" + key,first ) ;

	auto m = settings.value( value ).toString() ;

	if( m.isEmpty() ){

		return _readRegistry( key,value,std::forward< T >( rest ) ... ) ;
	}else{
		return m ;
	}
}

QString engineInstalledDir( const QString& key,const QString& value )
{
	return _readRegistry( key,
			      value,
			      QSettings::NativeFormat,
			      QSettings::Registry32Format,
			      QSettings::Registry64Format ) ;
}

#else

QString engineInstalledDir( const QString&,const QString& )
{
	return {} ;
}

QString applicationDirPath()
{
	return {} ;
}
QString lastError()
{
	return {} ;
}

int terminateProcess( unsigned long )
{
	return 1 ;
}

std::pair< bool,QString > driveHasSupportedFileSystem( const QString& path )
{
	Q_UNUSED( path )

	return { false,QString() } ;
}

#endif

}

}

QString SiriKali::Windows::engineInstalledDir( const engines::engine& m )
{
	const auto& key   = m.windowsInstallPathRegistryKey() ;
	const auto& value = m.windowsInstallPathRegistryValue() ;

	if( key.isEmpty() || value.isEmpty() ){

		return {} ;
	}else{
		return SiriKali::Windows::engineInstalledDir( key,value ) ;
	}
}

QStringList SiriKali::Windows::engineInstalledDirs()
{
	QStringList s ;

	for( const auto& it : engines::instance().supportedEngines() ){

		auto a = SiriKali::Windows::engineInstalledDir( it.get() ) ;

		if( !a.isEmpty() ){

			s.append( a ) ;
		}
	}

	return s ;
}
