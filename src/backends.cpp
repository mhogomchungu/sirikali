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

	class fuseOptions{
	public:
		fuseOptions()
		{
		}
		fuseOptions( QString m ) : m_options( std::move( m ) )
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
		void addPair( const QString& key,const QString& value,separator s = separator::equal_sign )
		{
			if( s == separator::space ){

				this->add( key + " " + value ) ;
			}else{
				this->add( key + "=" + value ) ;
			}
		}
		template< typename E >
		void add( const E& e )
		{
			mountOptions::add( m_options,",",e ) ;
		}
		template< typename E,typename ... T >
		void add( const E& e,T&& ... m )
		{
			mountOptions::add( m_options,",",e,std::forward< T >( m ) ... ) ;
		}
		const QString& get() const
		{
			return m_options ;
		}
	private:
		QString m_options ;
	} ;

	class exeOptions{
	public:
		exeOptions()
		{
		}
		exeOptions( QString m ) : m_options( std::move( m ) )
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
		void addPair( const QString& key,const QString& value,separator s = separator::space )
		{
			if( s == separator::space ){

				this->add( key + " " + value ) ;
			}else{
				this->add( key + "=" + value ) ;
			}
		}
		template< typename E >
		void add( const E& e )
		{
			mountOptions::add( m_options," ",e ) ;
		}
		template< typename E,typename ... T >
		void add( const E& e,T&& ... m )
		{
			mountOptions::add( m_options," ",e,std::forward< T >( m ) ... ) ;
		}
		const QString& get() const
		{
			return m_options ;
		}
	private:
		QString m_options ;
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

	template< typename E >
	static void add( QString& options,const QString& separator,E&& e )
	{
		if( options.isEmpty() ){

			options = e ;
		}else{
			options += separator + e ;
		}
	}
	template< typename E,typename ... T >
	static void add( QString& options,const QString& separator,E&& e,T&& ... m )
	{
		mountOptions::add( options,separator,std::forward< E >( e ) ) ;
		mountOptions::add( options,separator,std::forward< T >( m ) ... ) ;
	}
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
		{ "gocryptfs",cs::gocryptfsNotFound,cs::gocryptfs,_gocryptfs },
		{ "securefs",cs::securefsNotFound,cs::securefs,_securefs },
		{ "cryfs",cs::cryfsNotFound,cs::cryfs,_cryfs },
		{ "encfs",cs::encfsNotFound,cs::encfs,_encfs },
		{ "sshfs",cs::sshfsNotFound,cs::sshfs,_sshfs },
		{ "ecryptfs",cs::ecryptfs_simpleNotFound,cs::ecryptfs,_ecryptfs },
		{ "ecryptfs-simple",cs::ecryptfs_simpleNotFound,cs::ecryptfs,_ecryptfs } } } )
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
					[]( const cmdArgsList& ){ return QString() ; } } ;
	return shouldNotGetHere ;
}

QString backEnd::entity::executableFullPath() const
{
	return utility::executableFullPath( type ) ;
}
