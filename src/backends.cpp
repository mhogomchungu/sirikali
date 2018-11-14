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

#include "backends.h"
#include "utility.h"

class mountOptions{
public:
	enum class separator{ space,equal_sign } ;

	class Options{
	public:
		Options( QString m,QString s ) :
			m_options( std::move( m ) ),m_separator( s )
		{
		}
		bool doesNotContain( const QString& key ) const
		{
			return !this->contains( key ) ;
		}
		bool contains( const QString& key ) const
		{
			return m_options.contains( key ) ;
		}
		void addPair( const QString& key,const QString& value,separator s )
		{
			if( s == separator::space ){

				this->add( key + " " + value ) ;
			}else{
				this->add( key + "=" + value ) ;
			}
		}
		template< typename E >
		void add( E&& e )
		{
			if( m_options.isEmpty() ){

				m_options = std::forward< E >( e ) ;
			}else{
				m_options += m_separator + std::forward< E >( e ) ;
			}
		}
		template< typename E,typename ... T >
		void add( E&& e,T&& ... m )
		{
			this->add( std::forward< E >( e ) ) ;
			this->add( std::forward< T >( m ) ... ) ;
		}
		const QString& get() const
		{
			return m_options ;
		}
	private:
		QString m_options ;
		QString m_separator ;
	};

	class fuseOptions : public Options
	{
	public:
		fuseOptions( QString m ) : Options( std::move( m ),"," )
		{
		}
		void addPair( const QString& key,const QString& value,
			      separator s = separator::equal_sign )
		{
			Options::addPair( key,value,s ) ;
		}
	} ;

	class exeOptions : public Options
	{
	public:
		exeOptions( QString m ) : Options( std::move( m )," " )
		{
		}
		void addPair( const QString& key,const QString& value,
			      separator s = separator::space )
		{
			Options::addPair( key,value,s ) ;
		}
	} ;

	mountOptions( const backEnd::cmdArgsList& e,const QString& f,const QString& g = QString() ) :
		m_cmdArgsList( e ),m_type( f ),m_subType( g.isEmpty() ? "" : ",subtype=" + g )
	{
		for( const auto& it : utility::split( m_cmdArgsList.opt.mountOptions,',' ) ) {

			if( it.startsWith( '-' ) ){

				m_exeOptions += it + " " ;
			}else{
				m_fuseOptions += it + "," ;
			}
		}

		if( m_exeOptions.endsWith( " " ) ){

			m_exeOptions.remove( m_exeOptions.size() - 1,1 ) ;
		}

		if( m_fuseOptions.endsWith( "," ) ){

			m_fuseOptions.remove( m_fuseOptions.size() - 1,1 ) ;
		}

		if( !utility::platformIsLinux() && !m_fuseOptions.contains( "volname=" ) ){

			QString s ;

			if( utility::platformIsOSX() ){

				s = utility::split( e.opt.plainFolder,'/' ).last() ;
			}else{
				s = utility::split( e.opt.cipherFolder,'/' ).last() ;
			}

			if( !s.isEmpty() ){

				if( m_fuseOptions.isEmpty() ){

					m_fuseOptions = "volname=" + utility::Task::makePath( s ) ;
				}else{
					m_fuseOptions += ",volname=" + utility::Task::makePath( s ) ;
				}
			}
		}
	}
	mountOptions::exeOptions exeOptions() const
	{
		return m_exeOptions ;
	}
	mountOptions::fuseOptions fuseOpts() const
	{
		QString e = [ & ](){

			if( m_cmdArgsList.opt.ro ){

				return " -o ro,fsname=%1@%2%3" ;
			}else{
				return " -o rw,fsname=%1@%2%3" ;
			}
		}() ;

		if( m_fuseOptions.isEmpty() ){

			return e.arg( m_type,m_cmdArgsList.cipherFolder,m_subType ) ;
		}else{
			return e.arg( m_type,m_cmdArgsList.cipherFolder,m_subType ) + "," + m_fuseOptions ;
		}
	}
private:
	const backEnd::cmdArgsList& m_cmdArgsList ;
	QString m_exeOptions ;
	QString m_fuseOptions ;
	QString m_type ;
	QString m_subType ;
};

static QString _ecryptfs( const backEnd::cmdArgsList& args )
{
	auto e = QString( "%1 %2 %3 -a %4 %5 %6" ) ;

	auto s = e.arg( args.exe,
			args.create ? args.opt.createOptions : "-o key=passphrase",
			args.opt.ro ? "--readonly" : "",
			args.configFilePath,
			args.cipherFolder,
			args.mountPoint ) ;

	if( args.opt.mountOptions.isEmpty() ){

		if( utility::useSiriPolkit() ){

			return utility::wrap_su( s ) ;
		}else{
			return s ;
		}
	}else{
		if( utility::useSiriPolkit() ){

			return utility::wrap_su( s + " -o " + args.opt.mountOptions ) ;
		}else{
			return s + " -o " + args.opt.mountOptions ;
		}
	}
}

static siritask::status _error_code_ecryptfs( const QString& e,int )
{
	if( e.contains( "operation not permitted" ) ){

		return siritask::status::ecrypfsBadExePermissions ;

	}else if( e.contains( "error: mount failed" ) ){

		return siritask::status::ecryptfs ;
	}else{
		return siritask::status::backendFail ;
	}
}

static QString _gocryptfs( const backEnd::cmdArgsList& args )
{
	if( args.create ){

		QString e = "%1 %2 %3" ;

		mountOptions m( args,"gocryptfs" ) ;

		auto exeOptions = m.exeOptions() ;

		exeOptions.add( "--init","-q",args.opt.createOptions ) ;

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		if( args.opt.reverseMode ){

			exeOptions.add( "-reverse" ) ;
		}

		return e.arg( args.exe,exeOptions.get(),args.cipherFolder ) ;
	}else{
		mountOptions m( args,"gocryptfs" ) ;

		QString e = "%1 %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		exeOptions.add( "-q" ) ;

		if( args.opt.reverseMode ){

			exeOptions.add( "-reverse" ) ;
		}

		if( !args.opt.idleTimeout.isEmpty() ){

			exeOptions.addPair( "-idle",args.opt.idleTimeout ) ;
		}

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		return e.arg( args.exe,
			      exeOptions.get(),
			      args.cipherFolder,
			      args.mountPoint,
			      m.fuseOpts().get() ) ;
	}
}

static siritask::status _error_code_gocryptfs( const QString& e,int m )
{
	/*
	 * This error code was added in gocryptfs 1.2.1
	 */
	if( m == 12 ){

		return siritask::status::gocryptfs ;

	}else if( e.contains( "password" ) ){

		return siritask::status::gocryptfs ;
	}else{
		return siritask::status::backendFail ;
	}
}

static QString _securefs( const backEnd::cmdArgsList& args )
{
	if( args.create ){

		QString e = "%1 create %2 %3 %4" ;
		return e.arg( args.exe,
			      args.opt.createOptions,
			      args.configFilePath,
			      args.cipherFolder ) ;
	}else{
		mountOptions m( args,"securefs","securefs" ) ;

		QString exe = "%1 mount %2 %3 %4 %5" ;

		auto exeOptions = m.exeOptions() ;

		if( !utility::platformIsWindows() ){

			exeOptions.add( "-b" ) ;
		}

		if( !args.configFilePath.isEmpty() ){

			exeOptions.add( args.configFilePath ) ;
		}

		return exe.arg( args.exe,
				exeOptions.get(),
				args.cipherFolder,
				args.mountPoint,
				m.fuseOpts().get() ) ;
	}
}

static siritask::status _error_code_securefs( const QString& e,int s )
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return siritask::status::securefs ;

	}else if( e.contains( "winfsp" ) ){

		return siritask::status::failedToLoadWinfsp ;
	}else{
		return siritask::status::backendFail ;
	}
}

static QString _cryfs( const backEnd::cmdArgsList& args )
{
	auto separator = [](){

		/*
		 * declaring this variable as static to force this function to be called only
		 * once.
		 */
		static auto m = utility::backendIsLessThan( "cryfs","0.10" ).get() ;

		if( m && m.value() ){

			return "--" ;
		}else{
			return "" ;
		}
	}() ;

	auto e = QString( "%1 %2 %3 %4 %5 %6" ) ;

	mountOptions m( args,"cryfs","cryfs" ) ;

	auto exeOptions = m.exeOptions() ;

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( "--unmount-idle",args.opt.idleTimeout ) ;
	}

	if( args.create ){

		exeOptions.add( args.opt.createOptions ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	return e.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      separator,
		      m.fuseOpts().get() ) ;
}

static siritask::status _error_code_cryfs( const QString& e,int s )
{
	/*
	 * Error codes are here: https://github.com/cryfs/cryfs/blob/develop/src/cryfs/ErrorCodes.h
	 *
	 * Valid for cryfs > 0.9.8
	 */

	if( s == 11 ){

		return siritask::status::cryfs ;

	}else if( s == 14 ){

		return siritask::status::cryfsMigrateFileSystem ;
	}else{
		/*
		 * Falling back to parsing strings
		 */

		if( e.contains( "password" ) ){

			return siritask::status::cryfs ;

		}else if( e.contains( "this filesystem is for cryfs" ) &&
			  e.contains( "it has to be migrated" ) ){

			return siritask::status::cryfsMigrateFileSystem ;
		}else{
			return siritask::status::backendFail ;
		}
	}
}

static QString _encfs( const backEnd::cmdArgsList& args )
{
	QString e = "%1 %2 %3 %4 %5" ;

	mountOptions m( args,"encfs","encfs" ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.create ){

		exeOptions.add( args.opt.createOptions,"--stdinpass","--standard" ) ;
	}else{
		exeOptions.add( "--stdinpass" ) ;
	}

	if( args.opt.reverseMode ){

		exeOptions.add( "--reverse" ) ;
	}

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;
	}

	if( !args.configFilePath.isEmpty() ){

		exeOptions.add( args.configFilePath ) ;
	}

	if( !args.opt.idleTimeout.isEmpty() ){

		exeOptions.addPair( "-i",args.opt.idleTimeout ) ;
	}

	return e.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      m.fuseOpts().get() ) ;
}

static siritask::status _error_code_encfs( const QString& e,int s )
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return siritask::status::encfs ;

	}else if( e.contains( "winfsp" ) ){

		return siritask::status::failedToLoadWinfsp ;
	}else{
		return siritask::status::backendFail ;
	}
}

static QString _sshfs( const backEnd::cmdArgsList& args )
{
	mountOptions m( args,"sshfs","sshfs" ) ;

	auto fuseOptions = m.fuseOpts() ;

	if( !args.opt.key.isEmpty() ){

		fuseOptions.add( "password_stdin" ) ;
	}

	if( fuseOptions.doesNotContain( "StrictHostKeyChecking=" ) ){

		fuseOptions.addPair( "StrictHostKeyChecking","no" ) ;
	}

	auto exeOptions = m.exeOptions() ;

	if( utility::platformIsWindows() ){

		exeOptions.add( "-f" ) ;
	}

	QString s = "%1 %2 %3 %4 %5" ;

	return s.arg( args.exe,
		      exeOptions.get(),
		      args.cipherFolder,
		      args.mountPoint,
		      fuseOptions.get() ) ;
}

static siritask::status _error_code_sshfs( const QString& e,int s )
{
	Q_UNUSED( s ) ;

	if( e.contains( "password" ) ){

		return siritask::status::sshfs ;

	}else if( e.contains( "winfsp" ) ){

		return siritask::status::failedToLoadWinfsp ;
	}else{
		return siritask::status::backendFail ;
	}
}

static siritask::status _error_code_unknown_backend( const QString& e,int s )
{
	Q_UNUSED( e ) ;
	Q_UNUSED( s ) ;
	return siritask::status::backendFail ;
}

static QString _unknown_backend( const backEnd::cmdArgsList& args )
{
	Q_UNUSED( args ) ;
	return QString() ;
}

const backEnd& backEnd::instance()
{
	static backEnd v ;
	return v ;
}

bool backEnd::supported( const QString& e )
{
	return utility::containsAtleastOne( e," fuse.cryfs ",
					    " fuse.encfs ",
					    " fuse.sshfs ",
					    " fuse.gocryptfs ",
					    " fuse.gocryptfs-reverse ",
					    " fuse.securefs ",
					    " ecryptfs " ) ;
}

bool backEnd::supported( const QString& e,backEnd::Attempt m )
{
	if( m == Attempt::First ){

		return utility::startsWithAtLeastOne( e,"encfs@",
						      "cryfs@",
						      "securefs@",
						      "gocryptfs@",
						      "sshfs@" ) ;
	}else{
		return utility::equalsAtleastOne( e,"fuse.gocryptfs",
						  "fuse.gocryptfs-reverse",
						  "ecryptfs",
						  "fuse.sshfs" ) ;
	}
}

using cs = siritask::status ;

backEnd::backEnd() :
	entities( { {
		{ "gocryptfs",cs::gocryptfsNotFound,cs::gocryptfs,_gocryptfs,_error_code_gocryptfs },
		{ "securefs",cs::securefsNotFound,cs::securefs,_securefs,_error_code_securefs },
		{ "cryfs",cs::cryfsNotFound,cs::cryfs,_cryfs,_error_code_cryfs },
		{ "encfs",cs::encfsNotFound,cs::encfs,_encfs,_error_code_encfs },
		{ "sshfs",cs::sshfsNotFound,cs::sshfs,_sshfs,_error_code_sshfs },
		{ "ecryptfs",cs::ecryptfs_simpleNotFound,cs::ecryptfs,_ecryptfs,_error_code_ecryptfs },
		{ "ecryptfs-simple",cs::ecryptfs_simpleNotFound,cs::ecryptfs,_ecryptfs,_error_code_ecryptfs } } } )
{
}

const backEnd::entity& backEnd::get( const siritask::options& e ) const
{
	const auto& t = e.type ;

	for( size_t i = 0 ; i < entities.size() ; i++ ){

		if( t == entities[ i ].type ){

			return entities[ i ] ;
		}
	}

	static entity shouldNotGetHere{ "",
					cs::unknown,
					cs::unknown,
					_unknown_backend,
					_error_code_unknown_backend } ;
	return shouldNotGetHere ;
}

QString backEnd::entity::executableFullPath() const
{
	return utility::executableFullPath( type ) ;
}
