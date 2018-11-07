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

#include "systemsignalhandler.h"

#ifdef Q_OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#include <memory>

#include <QSocketNotifier>

static int sighupFd[ 2 ] ;
static int sigtermFd[ 2 ] ;

static void setup_unix_signal_handlers()
{
	struct sigaction hup ;
	struct sigaction term ;

	hup.sa_handler = []( int q ){

		Q_UNUSED( q ) ;

		char a = 1 ;

		::write( sighupFd[ 0 ],&a,sizeof( a ) ) ;
	} ;

	sigemptyset( &hup.sa_mask ) ;
	hup.sa_flags = 0 ;
	hup.sa_flags |= SA_RESTART ;

	sigaction( SIGHUP,&hup,nullptr ) ;

	term.sa_handler = []( int q ){

		Q_UNUSED( q ) ;

		char a = 1 ;

		::write( sigtermFd[ 0 ],&a,sizeof( a ) ) ;
	} ;

	sigemptyset( &term.sa_mask ) ;
	term.sa_flags |= SA_RESTART ;

	sigaction( SIGTERM,&term,nullptr ) ;
}

systemSignalHandler::systemSignalHandler( QObject * parent )
	: m_parent( parent )
{
	setup_unix_signal_handlers() ;

	::socketpair( AF_UNIX,SOCK_STREAM,0,sighupFd ) ;

	::socketpair(AF_UNIX,SOCK_STREAM,0,sigtermFd ) ;

	auto snHup = new QSocketNotifier( sighupFd[ 1 ],QSocketNotifier::Read,m_parent ) ;

	QObject::connect( snHup,&QSocketNotifier::activated,[ snHup,this ]( int ){
#if 1
		snHup->setEnabled( false ) ;
		m_function( signal::hup ) ;
#else
		snHup->setEnabled( false ) ;

		char tmp ;

		::read( sighupFd[ 1 ],&tmp,sizeof( tmp ) ) ;

		m_function( signal::hup ) ;

		snHup->setEnabled( true ) ;
#endif
	} ) ;

	auto snTerm = new QSocketNotifier( sigtermFd[ 1 ],QSocketNotifier::Read,m_parent ) ;

	QObject::connect( snTerm,&QSocketNotifier::activated,[ snTerm,this ]( int ){
#if 1
		snTerm->setEnabled( false ) ;
		m_function( signal::term ) ;
#else
		snTerm->setEnabled( false ) ;

		char tmp ;

		::read( sighupFd[ 1 ],&tmp,sizeof( tmp ) ) ;

		m_function( signal::term ) ;

		snTerm->setEnabled( true ) ;
#endif
	} ) ;
}

void systemSignalHandler::setAction( std::function< void( signal ) > function )
{
	m_function = std::move( function ) ;
}

#else

systemSignalHandler::systemSignalHandler( QObject * parent )
	: m_parent( parent )
{
}

void systemSignalHandler::setAction( std::function< void( signal ) > function )
{
	Q_UNUSED( function ) ;
}

#endif
