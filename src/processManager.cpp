/*
 *
 *  Copyright (c) 2020
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

#include "processManager.h"

static const char * _backEndTimedOut = "BackendTimedOut" ;

enum class encode{ True,False } ;
static QString _make_path( QString e,encode s )
{
	if( e.startsWith( "\"" ) ){

		e.remove( 0,1 ) ;
	}

	if( e.endsWith( "\"" ) ){

		e.truncate( e.size() -1 ) ;
	}

	if( s == encode::True ){

		return engines::engine::encodeMountPath( e ) ;
	}else{
		return e ;
	}
}

template< typename Function >
static processManager::result _read( QProcess& exe,const engines::engine& engine,Function function )
{
	QByteArray m ;
	QByteArray s ;

	int notRunningCounter = 0 ;
	int maxNotRunningCounter = 2 ;

	engines::engine::error r ;

	auto raii = utility::unlockIntervalReporter::object( engine.likeSsh() ) ;

	while( true ){

		if( exe.state() == QProcess::NotRunning ){

			utility::debug() << "warning, a process is no longer running" ;

			if( notRunningCounter < maxNotRunningCounter ){

				notRunningCounter++ ;
			}else{
				m = m + "\n-------------\n\nSiriKali::Error Backend Disappeared Unexpectedly." ;

				r = engines::engine::error::Failed ;

				break ;
			}
		}

		s = exe.readAllStandardOutput() ;

		if( !s.isEmpty() ){

			m += s ;
		}

		if( m.contains( "cygfuse: " ) || m.contains( "Cannot create WinFsp-FUSE file system:" ) ){

			r = engines::engine::error::Failed ;

			break ;
		}else{
			r = function( m ) ;

			if( r == engines::engine::error::Continue ){

				utility::Task::suspendForOneSecond() ;
			}else{
				break ;
			}
		}
	}

	return { r,std::move( m ) } ;
}

processManager::result processManager::getProcessOutput( QProcess& exe,const engines::engine& engine )
{
	int timeOut = engine.backendTimeout() ;

	if( timeOut > 0 ){

		int counter = 0 ;

		return _read( exe,engine,[ & ]( const QString& e ){

			if( counter < timeOut ){

				counter++ ;
				return engine.errorCode( e ) ;
			}else{
				return engines::engine::error::Timeout ;
			}
		} ) ;
	}else{
		return _read( exe,engine,[ & ]( const QString& e ){

			return engine.errorCode( e ) ;
		} ) ;
	}
}

static Task::process::result _error( const QByteArray& e )
{
	return Task::process::result( e,QByteArray(),-1,0,true ) ;
}

Task::process::result processManager::run( const processManager::opts& s )
{
	if( s.create ){

		if( s.engine.autoMountsOnCreate() ){

			return this->add( s ) ;
		}else{
			utility::logger logger ;

			logger.showText( s.args.cmd,s.args.cmd_args ) ;

			const auto& env = s.engine.getProcessEnvironment() ;

			auto& e = Task::process::run( s.args.cmd,s.args.cmd_args,-1,s.password,env ) ;

			auto m = utility::unwrap( e ) ;

			logger.showText( m ) ;

			return m ;
		}
	}else{
		return this->add( s ) ;
	}
}

bool processManager::backEndTimedOut( const QString& e )
{
	return e == _backEndTimedOut ;
}

static void _change_env( QProcess& exe,const processManager::opts& opts )
{
#if QT_VERSION > QT_VERSION_CHECK( 6,0,0 )
#ifdef Q_OS_WIN
	exe.setCreateProcessArgumentsModifier( [ & ]( QProcess::CreateProcessArguments * ){

		opts.engine.changeEnvironment( opts.args.cmd,opts.args.cmd_args ) ;
	} ) ;
#else
	exe.setChildProcessModifier( [ & ]{

		opts.engine.changeEnvironment( opts.args.cmd,opts.args.cmd_args ) ;
	} ) ;
#endif
#else
	Q_UNUSED( exe )
	Q_UNUSED( opts )
#endif
}

Task::process::result processManager::add( const processManager::opts& opts )
{
	auto e = opts.engine.prepareBackend() ;

	if( !e.isEmpty() ){

		return _error( e ) ;
	}

	auto exe = utility2::unique_qptr< QProcess >() ;

	_change_env( *exe,opts ) ;

	exe->setProcessEnvironment( opts.engine.getProcessEnvironment() ) ;
	exe->setProcessChannelMode( QProcess::MergedChannels ) ;
	exe->start( opts.args.cmd,opts.args.cmd_args ) ;
	exe->waitForStarted() ;
	exe->write( opts.password + "\n" ) ;
	exe->closeWriteChannel() ;

	utility::logger logger ;

	logger.showText( opts.args.cmd,opts.args.cmd_args ) ;

	auto s = this->getResult( std::move( exe ),opts,_error,_backEndTimedOut ) ;

	logger.showText( s ) ;

	return s ;
}

Task::process::result processManager::remove( const QString& mountPoint,int attempts )
{
	for( auto it = m_instances.begin() ; it != m_instances.end() ; it++ ){

		const auto& s = *it ;

		if( s.mountPoint() == mountPoint ){

			auto m = s.engine().terminateProcess( attempts,{ s.exe(),s.engine(),s.mountPoint() } ) ;

			if( m.result.success() ) {

				auto& exe = s.exe() ;

				for( int i = 0 ; i < 10 ; i++ ){

					if( exe.waitForFinished( 100 ) ){

						break ;
					}
				}

				m_volumeList->update() ;

				m_instances.erase( it ) ;

				return 0 ;
			}else{
				return m.result ;
			}
		}
	}

	return { "","Failed To Terminate Unknown Process",1,0,true } ; ;
}

std::vector<QStringList> processManager::commands() const
{
	std::vector< QStringList > s ;

	this->ProcessEntries( [ & ]( const processManager::Process& p ){

		auto e = p.exe().arguments() ;

		for( auto& m : e ){

			m = engines::engine::encodeMountPath( m ) ;
		}

		s.emplace_back( std::move( e ) ) ;

		return false ;
	} ) ;

	return s ;
}

std::vector< engines::engine::Wrapper > processManager::enginesList() const
{
	std::vector< engines::engine::Wrapper > m ;

	this->ProcessEntries( [ & ]( const processManager::Process& p ){

		m.emplace_back( p.engine() ) ;

		return false ;
	} ) ;

	return m ;
}

QString processManager::volumeProperties( const QString& mm ) const
{
	QString m ;

	this->ProcessEntries( [ & ]( const processManager::Process& e ){

		if( mm == e.mountPoint() ){

			m = QObject::tr( "Mount Options:\n\n" ) ;

			const auto& s = e.arguments() ;

			if( s.contains( "--password" ) ){

				auto e = s ;

				for( int i = 0 ; i < e.size() ; i++ ){

					const auto& it = e[ i ] ;

					if( it != "-o" ){

						if( it == "--password" && i + 1 < e.size() ){

							e[ i + 1 ] = "<redacted>" ;
						}

						m += it + "\n" ;
					}
				}
			}else{
				for( const auto& it : s ){

					if( it != "-o" ){

						m += it + "\n" ;
					}
				}
			}

			return true ;
		}else{
			return false ;
		}
	} ) ;

	return m ;
}



std::vector< processManager::mountOpts > processManager::mountOptions() const
{
	std::vector< processManager::mountOpts > mOpts ;

	this->ProcessEntries( [ & ]( const processManager::Process& p ){

		const auto& m = p.args() ;

		auto a = _make_path( m.cipherPath,encode::True ) ;
		auto b = _make_path( m.mountPath,encode::True ) ;

		mOpts.emplace_back( m.mode,m.subtype,a,b,m.fuseOptions,p.engine() ) ;

		return false ;
	} ) ;

	return mOpts ;
}

bool processManager::mountPointTaken( const QString& ee ) const
{
	bool m = false ;

	auto e = "\"" + QDir::toNativeSeparators( ee ) + "\"" ;

	this->ProcessEntries( [ & ]( const processManager::Process& p ){

		if( QDir::toNativeSeparators( p.mountPoint() ) == e ){

			m = true ;
			return true ;
		}else{
			return false ;
		}
	} ) ;

	return m ;
}

void processManager::removeInActive()
{
	struct meaw
	{
		bool operator()( const processManager::Process& p ) const
		{
			return p.notRunning() ;
		}
	};

	auto begin = m_instances.begin() ;
	auto end   = m_instances.end() ;

	m_instances.erase( std::remove_if( begin,end,meaw() ),end ) ;
}
