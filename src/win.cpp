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

#include <QString>

namespace SiriKali{
namespace Windows{

#ifdef Q_OS_WIN

#include <windows.h>

static int _terminateProcess( unsigned long pid )
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

static HKEY _reg_open_key( const char * subKey )
{
	HKEY hkey = HKEY_LOCAL_MACHINE ;
	HKEY m ;
	REGSAM wow64 = KEY_QUERY_VALUE | KEY_WOW64_64KEY ;
	REGSAM wow32 = KEY_QUERY_VALUE | KEY_WOW64_32KEY ;
	unsigned long x = 0 ;

	if( RegOpenKeyExA( hkey,subKey,x,wow64,&m ) == ERROR_SUCCESS ){

		return m ;

	}else if( RegOpenKeyExA( hkey,subKey,x,wow32,&m ) == ERROR_SUCCESS ){

		return m ;
	}else{
		return nullptr ;
	}
}

static void _reg_close_key( HKEY hkey )
{
	if( hkey != nullptr ){

		RegCloseKey( hkey ) ;
	}
}

static QByteArray _reg_get_value( HKEY hkey,const char * key )
{
	if( hkey != nullptr ){

		DWORD dwType = REG_SZ ;

		std::array< char,4096 > buffer ;

		std::fill( buffer.begin(),buffer.end(),'\0' ) ;

		auto e = reinterpret_cast< BYTE * >( buffer.data() ) ;
		auto m = static_cast< DWORD >( buffer.size() ) ;

		if( RegQueryValueEx( hkey,key,nullptr,&dwType,e,&m ) == ERROR_SUCCESS ){

			return { buffer.data(),static_cast< int >( m ) } ;
		}
	}

	return {} ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	auto s = utility2::unique_rsc( _reg_open_key,_reg_close_key,subKey ) ;

	return _reg_get_value( s.get(),key ) ;
}

static auto _msg( DWORD err )
{
	TCHAR * s = nullptr ;

	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER ;

	auto m = FormatMessage( flags,
				nullptr,
				err,
				MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
				reinterpret_cast< TCHAR * >( &s ),
				0,
				nullptr ) ;

	return std::make_pair( m > 0,utility2::unique_ptr( s,[]( TCHAR * e ){ LocalFree( e ) ; } ) ) ;
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
	auto a = path.mid( 0,1 ).toLatin1()[ 0 ] ;

	std::array< TCHAR,4 >rpath = { a,':','\\','\0' } ;

	std::array< TCHAR,MAX_PATH + 1 > fsname ;

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

#else

QString lastError()
{
	return {} ;
}

static int _terminateProcess( unsigned long pid )
{
	Q_UNUSED( pid )
	return 1 ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	Q_UNUSED( subKey )
	Q_UNUSED( key )
	return QString() ;
}

std::pair< bool,QString > driveHasSupportedFileSystem( const QString& path )
{
	Q_UNUSED( path )

	return { false,QString() } ;
}

#endif

}

}

int SiriKali::Windows::terminateProcess( unsigned long pid )
{
	return _terminateProcess( pid ) ;
}

QString SiriKali::Windows::engineInstalledDir( const engines::engine& m )
{
	const auto& key   = m.windowsInstallPathRegistryKey() ;
	const auto& value = m.windowsInstallPathRegistryValue() ;

	if( key.isEmpty() || value.isEmpty() ){

		return {} ;
	}

	return SiriKali::Windows::engineInstalledDir( key,value ) ;
}

QString SiriKali::Windows::engineInstalledDir( const QString& key,const QString& value )
{
	return _readRegistry( key.toLatin1().constData(),value.toLatin1().constData() ) ;
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
