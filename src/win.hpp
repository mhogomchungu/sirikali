/*
 *
 *  Copyright (c) 2019
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

#include <QtGlobal>

#include <QString>

#ifdef Q_OS_WIN

#include <windows.h>

int poll( struct pollfd * a,int b,int c )
{
	Q_UNUSED( a ) ;
	Q_UNUSED( b ) ;
	Q_UNUSED( c ) ;

	return 0 ;
}

class regOpenKey{
public:
	regOpenKey( const char * subKey,HKEY hkey = HKEY_LOCAL_MACHINE )
	{
		HKEY m ;
		REGSAM wow64 = KEY_QUERY_VALUE | KEY_WOW64_64KEY ;
		REGSAM wow32 = KEY_QUERY_VALUE | KEY_WOW64_32KEY ;

		if( this->success( RegOpenKeyExA,hkey,subKey,0,wow64,&m ) ){

			m_hkey = m ;

		}else if( this->success( RegOpenKeyExA,hkey,subKey,0,wow32,&m ) ){

			m_hkey = m ;
		}else{
			m_hkey = nullptr ;
		}
	}
	regOpenKey( const regOpenKey& ) = delete ;
	regOpenKey& operator=( const regOpenKey& ) = delete ;
	regOpenKey( regOpenKey&& other )
	{
		this->closeKey() ;
		m_hkey = other.m_hkey ;
		other.m_hkey = nullptr ;
	}
	regOpenKey& operator=( regOpenKey&& other )
	{
		this->closeKey() ;
		m_hkey = other.m_hkey ;
		other.m_hkey = nullptr ;
		return *this ;
	}
	operator bool()
	{
		return m_hkey != nullptr ;
	}
	QByteArray getValue( const char * key )
	{
		if( m_hkey != nullptr ){

			DWORD dwType = REG_SZ ;

			std::array< char,4096 > buffer ;

			std::fill( buffer.begin(),buffer.end(),'\0' ) ;

			auto e = reinterpret_cast< BYTE * >( buffer.data() ) ;
			auto m = static_cast< DWORD >( buffer.size() ) ;

			if( this->success( RegQueryValueEx,m_hkey,key,nullptr,&dwType,e,&m ) ){

				return { buffer.data(),static_cast< int >( m ) } ;
			}
		}

		return {} ;
	}
	HKEY handle()
	{
		return m_hkey ;
	}
	~regOpenKey()
	{
		this->closeKey() ;
	}
private:
	template< typename Function,typename ... Args >
	bool success( Function&& function,Args&& ... args )
	{
		return function( std::forward< Args >( args ) ... ) == ERROR_SUCCESS ;
	}
	void closeKey()
	{
		RegCloseKey( m_hkey ) ;
	}
	HKEY m_hkey ;
};

// SiriKali took below code from "https://stackoverflow.com/questions/813086/can-i-send-a-ctrl-c-sigint-to-an-application-on-windows"

// Inspired from http://stackoverflow.com/a/15281070/1529139
// and http://stackoverflow.com/q/40059902/1529139
static bool signalCtrl(DWORD dwProcessId, DWORD dwCtrlEvent)
{
    bool success = false;
    DWORD thisConsoleId = GetCurrentProcessId();
    // Leave current console if it exists
    // (otherwise AttachConsole will return ERROR_ACCESS_DENIED)
    bool consoleDetached = (FreeConsole() != FALSE);

    if (AttachConsole(dwProcessId) != FALSE)
    {
	// Add a fake Ctrl-C handler for avoid instant kill is this console
	// WARNING: do not revert it or current program will be also killed
	SetConsoleCtrlHandler(nullptr, true);
	success = (GenerateConsoleCtrlEvent(dwCtrlEvent, 0) != FALSE);
	FreeConsole();
    }

    if (consoleDetached)
    {
	// Create a new console if previous was deleted by OS
	if (AttachConsole(thisConsoleId) == FALSE)
	{
	    int errorCode = GetLastError();
	    if (errorCode == 31) // 31=ERROR_GEN_FAILURE
	    {
		AllocConsole();
	    }
	}
    }
    return success;
}

static int _terminateProcess( unsigned long pid )
{
	if( signalCtrl( pid,CTRL_C_EVENT ) ){

		return 0 ;
	}else{
		return 1 ;
	}
}

static QString _readRegistry( const char * subKey,const char * key )
{
        return regOpenKey( subKey ).getValue( key ) ;
}

#else

static int _terminateProcess( unsigned long pid )
{
	Q_UNUSED( pid ) ;
	return 1 ;
}

static QString _readRegistry( const char * subKey,const char * key )
{
	Q_UNUSED( subKey ) ;
	Q_UNUSED( key ) ;
	return QString() ;
}

#endif
