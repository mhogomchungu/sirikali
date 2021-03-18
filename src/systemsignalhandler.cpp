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
#include <unistd.h>
#include <signal.h>

/*
 * How to handle signals in unix: https://doc.qt.io/archives/qt-5.6/unix-signals.html
 */

struct signalHandler
{
	struct socketPairs
	{
		socketPairs( systemSignalHandler::signal sig )
		{
			signal = sig ;
		}
		int reader() const
		{
			return sockpair[ 1 ] ;
		}
		int writer() const
		{
			return sockpair[ 0 ] ;
		}

		systemSignalHandler::signal signal ;
		int sockpair[ 2 ] ;
	} ;

	static signalHandler& instance()
	{
		static signalHandler m ;

		return m ;
	}
	static void function( int SIG )
	{
		for( const auto& it : signalHandler::instance().signalsManager ){

			if( it.signal == static_cast< systemSignalHandler::signal >( SIG ) ){

				char a = 1 ;

				if( write( it.writer(),&a,sizeof( a ) ) ){}

				break ;
			}
		}
	}
	socketPairs& addSignal( systemSignalHandler::signal SIG )
	{
		signalsManager.emplace_back( SIG ) ;
		return signalsManager[ signalsManager.size() - 1 ] ;
	}
	std::function< void( systemSignalHandler::signal ) > callback ;
	std::vector< signalHandler::socketPairs > signalsManager ;
	QObject * parent ;
} ;

systemSignalHandler::systemSignalHandler( QObject * parent )
{
	signalHandler::instance().parent = parent ;
}

void systemSignalHandler::setHandle( std::function< void( systemSignalHandler::signal ) > function )
{
	signalHandler::instance().callback = std::move( function ) ;
}

#ifdef Q_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#include <QSocketNotifier>

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	auto& sHandler = signalHandler::instance() ;

	auto& e = sHandler.addSignal( SIG ) ;

	socketpair( AF_UNIX,SOCK_STREAM,0,e.sockpair ) ;

	struct sigaction action ;

	sigemptyset( &action.sa_mask ) ;

	action.sa_flags = 0 ;
	action.sa_flags |= SA_RESTART ;

	action.sa_handler = signalHandler::function ;

	sigaction( static_cast< int >( SIG ),&action,nullptr ) ;

	auto mm = new QSocketNotifier( e.reader(),QSocketNotifier::Read,sHandler.parent ) ;

	QObject::connect( mm,&QSocketNotifier::activated,[ SIG,mm ]( int s ){

		Q_UNUSED( s )

		auto& signalHandler = signalHandler::instance() ;

		mm->setEnabled( false ) ;

		for( const auto& it : signalHandler.signalsManager ){

			if( it.signal == SIG ){

				char tmp ;

				read( it.reader(),&tmp,sizeof( tmp ) ) ;

				signalHandler.callback( SIG ) ;

				mm->setEnabled( true ) ;

				break ;
			}
		}
	} ) ;
}

#endif

#ifdef Q_OS_WIN

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	Q_UNUSED( SIG )
}

#endif

#ifdef Q_OS_MACOS

void systemSignalHandler::addSignal( systemSignalHandler::signal SIG )
{
	Q_UNUSED( SIG )
}

#endif
