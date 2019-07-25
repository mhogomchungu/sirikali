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
#include "win.hpp"

#include <utility>
#include <tuple>

namespace SiriKali{
namespace Windows{
static const char * _backEndTimedOut = "SiriKali::Windows::BackendTimedOut" ;

static void _deleteLater( QObject * e )
{
	e->deleteLater() ;
}

struct Process
{
	Process( const engines::engine::args& args,const QString& e,QProcess * p ) :
		args( args ),engineName( e ),instance( p,_deleteLater )
	{
	}
	Process( Process&& ) = default ;
	Process& operator=( Process&& ) = default ;
	engines::engine::args args ;
	QString engineName ;
	std::unique_ptr< QProcess,void(*)( QObject * ) > instance ;
} ;

struct result
{
	engines::engine::error type ;
	QByteArray outPut ;
} ;

class volumes
{
public:
	static SiriKali::Windows::volumes& get()
	{
		static SiriKali::Windows::volumes s ;
		return s ;
	}
	Task::process::result add( const SiriKali::Windows::opts& ) ;
	Task::process::result remove(const QString& unMountCommand,const QString& mountPoint ) ;
	std::vector< QStringList > commands() const ;
	QString volumeProperties( const QString& mountPath ) ;
	void updateVolumeList( std::function< void() > ) ;
	std::vector< SiriKali::Windows::mountOptions > mountOptions() ;
	bool mountPointTaken( const QString& e ) ;
private:
	std::vector< Process > m_instances ;
	std::function< void() > m_updateVolumeList ;
} ;

bool backEndTimedOut( const QString& e )
{
	return e == SiriKali::Windows::_backEndTimedOut ;
}

}

}

int SiriKali::Windows::terminateProcess( unsigned long pid )
{
	return _terminateProcess( pid ) ;
}

QString SiriKali::Windows::engineInstalledDir( const QString& e )
{
	const auto& m = engines::instance().getByName( e ) ;

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

	for( const auto& it : engines::instance().supported() ){

		auto a = SiriKali::Windows::engineInstalledDir( it ) ;

		if( !a.isEmpty() ){

			s.append( a ) ;
		}
	}

	return s ;
}

std::vector< QStringList > SiriKali::Windows::volumes::commands() const
{
	std::vector< QStringList > s ;

	for( const auto& it : m_instances ){

		auto e = it.instance->arguments() ;

		for( auto& m : e ){

			m = mountinfo::encodeMountPath( m ) ;
		}

		s.emplace_back( std::move( e ) ) ;
	}

	return s ;
}

void SiriKali::Windows::volumes::updateVolumeList( std::function< void() > function )
{
	m_updateVolumeList = std::move( function ) ;
}

template< typename Function >
static SiriKali::Windows::result _read( QProcess& exe,Function function )
{
	QByteArray m ;
	QByteArray s ;

	int counter = 1 ;
	int notRunningCounter = 0 ;
	int maxNotRunningCounter = 2 ;

	engines::engine::error r ;

	while( true ){

		if( exe.state() == QProcess::NotRunning ){

			utility::debug() << "warning, a process is no longer running" ;

			if( notRunningCounter < maxNotRunningCounter ){

				notRunningCounter++ ;
			}else{
				m = m + "\n-------------\n\nSiriKali::Error Backend Disappeared For Some Reason (It Probably Crashed)." ;

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
				counter++ ;
			}else{
				break ;
			}
		}
	}

	auto a = QString::number( counter ) ;

	auto b = "Backend took " + a + " seconds to finish\nGenerated backend output:\n" ;

	utility::debug() << b + m ;

	return { r,std::move( m ) } ;
}

static SiriKali::Windows::result _getProcessOutput( QProcess& exe,const engines::engine& engine )
{
	if( engine.name() == "sshfs" ){

		int counter = 0 ;
		int max = settings::instance().sshfsBackendTimeout() ;

		return _read( exe,[ & ]( const QString& e ){

			if( counter < max ){

				counter++ ;
				return engine.errorCode( e ) ;
			}else{
				return engines::engine::error::Timeout ;
			}
		} ) ;
	}else{
		return _read( exe,[ & ]( const QString& e ){

			return engine.errorCode( e ) ;
		} ) ;
	}
}

static QProcessEnvironment _update_environment( const QString& e )
{
	auto env = utility::systemEnvironment() ;

	auto s = SiriKali::Windows::engineInstalledDir( e ) ;

	if( s.isEmpty() ){

		return env ;
	}else{
		auto a = s + ";" ;
		auto b = s + "\\bin;" ;
		auto c = a + b + env.value( "PATH" ) ;

		env.insert( "PATH",c ) ;
		return env ;
	}
}

Task::process::result SiriKali::Windows::create( const SiriKali::Windows::opts& opts )
{
	if( opts.engine.autoMountsOnCreate() ){

		return SiriKali::Windows::mount( opts ) ;
	}else{
		auto s = utility::unwrap( Task::process::run( opts.args.cmd,opts.password.toLatin1() ) ) ;

		utility::logCommandOutPut( s,opts.args.cmd ) ;

		return s ;
	}
}

static std::pair< Task::process::result,QString > _terminate_process( QProcess& e,
								      const QString& engine,
								      const QString& mountPath = QString(),
								      const QString& unMountCommand = QString() )
{
	QString exe ;

	if( unMountCommand == "sirikali.exe -T" ){

		exe = unMountCommand + QString::number( e.processId() ) ;

	}else if( unMountCommand.isEmpty() || mountPath.isEmpty() ){

		exe = "taskkill /F /PID " + QString::number( e.processId() ) ;
	}else{
		exe = utility::Task::makePath( unMountCommand ) + " " + mountPath ;
	}

	auto m = utility::unwrap( Task::process::run( exe,{},-1,"",_update_environment( engine ) ) ) ;

	if( m.success() ){

		utility::waitForFinished( e ) ;
	}

	return { std::move( m ),std::move( exe ) } ;
}

Task::process::result SiriKali::Windows::volumes::add( const SiriKali::Windows::opts& opts )
{
	auto exe = utility2::unique_qptr< QProcess >() ;

	exe->setProcessEnvironment( _update_environment( opts.engine.name() ) ) ;
	exe->setProcessChannelMode( QProcess::MergedChannels ) ;
	exe->start( opts.args.cmd ) ;
	exe->waitForStarted() ;
	exe->write( opts.password.toLatin1() + "\n" ) ;
	exe->closeWriteChannel() ;

	auto m = _getProcessOutput( *exe,opts.engine ) ;

	auto s = [ & ](){

		if( m.type == engines::engine::error::Timeout ){

			_terminate_process( *exe,opts.engine.name() ) ;

			return Task::process::result( SiriKali::Windows::_backEndTimedOut,
						      QByteArray(),
						      -1,
						      0,
						      true ) ;

		}else if( m.type == engines::engine::error::Success ){

			exe->closeReadChannel( QProcess::StandardError ) ;
			exe->closeReadChannel( QProcess::StandardOutput ) ;

			m_instances.emplace_back( opts.args,opts.engine.name(),exe.release() ) ;

			m_updateVolumeList() ;

			return Task::process::result( 0 ) ;
		}else{
			utility::waitForFinished( *exe ) ;

			QByteArray stdOut ;
			QByteArray stdError ;

			if( opts.options.type == "encfs" ){

				stdOut = m.outPut ;
			}else{
				stdError = m.outPut ;
			}

			return Task::process::result( stdOut,
						      stdError,
						      exe->exitCode(),
						      exe->exitStatus(),
						      true ) ;
		}
	}() ;

	utility::logCommandOutPut( s,opts.args.cmd ) ;

	return s ;
}

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

		return mountinfo::encodeMountPath( e ) ;
	}else{
		return e ;
	}
}

Task::process::result SiriKali::Windows::volumes::remove( const QString& unMountCommand,
							  const QString& mountPoint )
{
	for( auto it = m_instances.begin() ; it != m_instances.end() ; it++ ){

		const auto& s = *it ;

		if( s.args.mountPath == mountPoint ){

			auto& p = s.instance ;
			auto m = _terminate_process( *p,s.engineName,s.args.mountPath,unMountCommand ) ;

			auto r = [ & ](){

				if( m.first.success() ) {

					m_instances.erase( it ) ;

					m_updateVolumeList() ;

					return Task::process::result( 0 ) ;
				}else{
					return Task::process::result( "","Failed To Terminate A Process",1,0,true ) ;
				}
			}() ;

			utility::logCommandOutPut( r,m.second ) ;

			return r ;
		}
	}

	return Task::process::result() ;
}

QString SiriKali::Windows::volumes::volumeProperties( const QString& mm )
{
	auto mountPath = "\"" + mm + "\"" ;

	for( size_t i = 0 ; i < m_instances.size() ; i++ ){

		const auto& e = m_instances[ i ] ;

		if( mountPath == e.args.mountPath ){

			auto m = QObject::tr( "Mount Options:\n\n" ) ;

			for( const auto& it : e.instance->arguments() ){

				if( it != "-o" ){

					m += it + "\n" ;
				}
			}

			return m ;
		}
	}

	return QString() ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::volumes::mountOptions()
{
	std::vector< SiriKali::Windows::mountOptions > mOpts ;

	auto _remove_inactive = [ & ]{

		for( auto it = m_instances.begin() ; it != m_instances.end() ; it++ ){

			if( ( *it ).instance->state() == QProcess::NotRunning ){

				m_instances.erase( it ) ;

				return true ;
			}
		}

		return false ;
	} ;

	while( _remove_inactive() ){}

	for( const auto& it : m_instances ){

		const auto& m = it.args ;

		auto a = _make_path( m.cipherPath,encode::True ) ;
		auto b = _make_path( m.mountPath,encode::True ) ;

		mOpts.emplace_back( m.mode,m.subtype,a,b,m.fuseOptions ) ;
	}

	return mOpts ;
}

bool SiriKali::Windows::volumes::mountPointTaken( const QString& ee )
{
	auto e = "\"" + QDir::toNativeSeparators( ee ) + "\"" ;

	for( const auto& it : m_instances ){

		if( QDir::toNativeSeparators( it.args.mountPath ) == e ){

			return true ;
		}
	}

	return false ;
}

QString SiriKali::Windows::volumeProperties( const QString& mountPath )
{
	return volumes::get().volumeProperties( mountPath ) ;
}

std::vector< SiriKali::Windows::mountOptions > SiriKali::Windows::getMountOptions()
{
	return volumes::get().mountOptions() ;
}

Task::process::result SiriKali::Windows::unmount( const QString& unMountCommand,const QString& m )
{
	return volumes::get().remove( unMountCommand,m ) ;
}

Task::process::result SiriKali::Windows::mount( const SiriKali::Windows::opts& opts )
{
	return volumes::get().add( opts ) ;
}

void SiriKali::Windows::updateVolumeList( std::function< void() > function )
{
	volumes::get().updateVolumeList( std::move( function ) ) ;
}

bool SiriKali::Windows::mountPointTaken( const QString& e )
{
	return volumes::get().mountPointTaken( e ) ;
}
