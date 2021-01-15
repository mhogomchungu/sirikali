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

#include "settings.h"
#include "systemsignalhandler.h"
#include <memory>

#include <QSocketNotifier>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

// https://doc.qt.io/archives/qt-5.6/unix-signals.html

std::vector< systemSignalHandler::manager>  systemSignalHandler::m_signals ;
std::function< void( systemSignalHandler::signal ) > systemSignalHandler::m_function ;
QObject * systemSignalHandler::m_parent ;

systemSignalHandler::systemSignalHandler( QObject * parent )
{
	m_parent = parent ;
}

void systemSignalHandler::setHandle( std::function< void( systemSignalHandler::signal ) > function )
{
	m_function = std::move( function ) ;
}

systemSignalHandler::manager::manager( systemSignalHandler::signal sig ) : signal( sig )
{
}

int systemSignalHandler::manager::read() const
{
	return sockpair[ 1 ] ;
}

int systemSignalHandler::manager::write() const
{
	return sockpair[ 0 ] ;
}

#ifdef Q_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#include <QSocketNotifier>

void systemSignalHandler::signalHandle( int SIG )
{
	for( const auto& it : m_signals ){

		if( it.signal == static_cast< systemSignalHandler::signal >( SIG ) ){

			char a = 1 ;

			if( write( it.write(),&a,sizeof( a ) ) ){}

			break ;
		}
	}
}

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	m_signals.emplace_back( SIG ) ;

	manager& e = m_signals[ m_signals.size() - 1 ] ;

	socketpair( AF_UNIX,SOCK_STREAM,0,e.sockpair ) ;

	struct sigaction action ;

	sigemptyset( &action.sa_mask ) ;

	action.sa_flags = 0 ;
	action.sa_flags |= SA_RESTART ;

	action.sa_handler = systemSignalHandler::signalHandle ;

	sigaction( static_cast< int >( SIG ),&action,nullptr ) ;

	auto mm = new QSocketNotifier( e.read(),QSocketNotifier::Read,m_parent ) ;

	QObject::connect( mm,&QSocketNotifier::activated,[ SIG,mm ]( int ){

		mm->setEnabled( false ) ;

		for( const auto& it : m_signals ){

			if( it.signal == SIG ){

				char tmp ;

				read( it.read(),&tmp,sizeof( tmp ) ) ;

				m_function( SIG ) ;

				mm->setEnabled( true ) ;

				break ;
			}
		}
	} ) ;
}

#endif

#ifdef Q_OS_WIN

void systemSignalHandler::signalHandle( int SIG )
{
	Q_UNUSED( SIG )
}

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	Q_UNUSED( SIG )
}

#endif

#ifdef Q_OS_MACOS

void systemSignalHandler::signalHandle( int SIG )
{
	Q_UNUSED( SIG )
}

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	Q_UNUSED( SIG )
}

#endif
