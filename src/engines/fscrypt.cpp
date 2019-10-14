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

#include "fscrypt.h"
#include "../win.h"
#include <vector>
#include "../settings.h"

static QStringList _encryptedVolumes( const QString& list )
{
	QStringList l ;

	auto s = utility::split( list,'\n' ) ;

	for( int i = s.size() - 1 ; i >= 0 ; i-- ){

		const auto& x =s.at( i ) ;

		if( x.startsWith( "MOUNTPOINT" ) ){

			break ;
		}else{
			auto a = utility::split( x,' ' ) ;

			if( a.size() > 4 ){

				if( a.at( 3 ) == "supported" && a.at( 4 ) == "Yes" ){

					l.append( a.at( 0 ) ) ;
				}
			}
		}
	}

	return l ;
}

struct protector{

	protector( const QString& a,const QString& b ) :
		ptr( a ),comment( b )
	{
	}
	QString ptr ;
	QString comment ;
};

struct policy{

	policy( const QString& a,const QString& b,const QString& c ) :
		poly( a ),unlocked( b ),ptr( c )
	{
	}
	QString poly ;
	QString unlocked ;
	QString ptr ;
	QString comment ;
};

static std::vector< protector > _getProtectors( int& s,const QStringList l )
{
	std::vector< protector > m ;

	for( ; s < l.size() ; s++ ){

		const auto& e = l.at( s ) ;

		if( e.startsWith( "POLICY" ) ){

			s++ ;
			break ;
		}

		auto a = utility::split( e,' ' ) ;

		if( e.size() > 4 ){

			m.emplace_back( a.at( 0 ),a.at( 4 ) ) ;
		}
	}

	return m ;
}

static std::vector< policy > _getPolicies( int& s,const QStringList l )
{
	std::vector< policy > m ;

	for( ; s < l.size() ; s++ ){

		const auto& e = l.at( s ) ;

		auto a = utility::split( e,' ' ) ;

		if( e.size() > 2 ){

			m.emplace_back( a.at( 0 ),a.at( 1 ),a.at( 2 ) ) ;
		}
	}

	return m ;
}

static std::vector< policy > _getPolicies( const QString& m,const QString& exe )
{
	auto e = utility::Task::run( exe + " status " + m ).get().stdOut() ;

	if( e.isEmpty() ){

		return {} ;
	}

	const auto s =  utility::split( e,'\n' ) ;

	int i = 0 ;
	for( ; i < s.size() ; i++  ){

		if( !s.at( i ).startsWith( "PROTECTOR" ) ){

			continue ;
		}else{
			i++ ;
			break ;
		}
	}

	auto protectors = _getProtectors( i,s ) ;
	auto policies   = _getPolicies( i,s ) ;

	for( auto& it : policies ){

		for( const auto& xt : protectors ) {

			if( it.ptr == xt.ptr ){

				it.comment = xt.comment ;
			}
		}
	}

	return policies ;
}

static QString _getFsMode( const QStringList& s,const QString& m )
{
	for( const auto& it : s ){

		if( it.contains( " " + m + " " ) ){

			auto a = utility::split( it,' ' ) ;

			if( a.size() > 6 ){

				const auto& b = a.at( 5 ) ;

				if( b.startsWith( "rw" ) ){

					return "rw" ;

				}else if( b.startsWith( "ro" ) ){

					return "ro" ;
				}else{
					return "-" ;
				}
			}
		}
	}

	return "-" ;
}

static QStringList _fscryptVolumes( const QStringList& s )
{
	auto exe = utility::executableFullPath( "fscrypt" ) ;

	if( exe.isEmpty() ){

		return {} ;
	}

	auto status = utility::Task::run( exe + " status" ).get().stdOut() ;

	if( status.isEmpty() ){

		return {} ;
	}

	const QString w = "x x x:x x %1 %2,x - %3 %4 x" ;

	QStringList l ;

	for( const auto& it : _encryptedVolumes( status ) ){

		auto m = _getPolicies( it,exe ) ;

		for( const auto& xt : m ){

			if( xt.unlocked == "Yes" ){

				auto a = utility::policyString() ;
				auto b = utility::commentString() ;

				auto c = a + xt.poly + "\n" + b + xt.comment ;

				l.append( w.arg( it,_getFsMode( s,it ),"fscrypt",c ) ) ;
			}
		}
	}

	return l ;
}

Task::future< QStringList >& fscrypt::fscryptVolumes( const QStringList& s )
{
	return Task::run( [ & ](){ return _fscryptVolumes( s ) ; } ) ;
}

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = false ;
	s.backendRequireMountPath     = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = false ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "fscrypt" ;
	s.incorrectPasswordText = "incorrect key provided" ;
	s.configFileNames       = QStringList{ ".fscrypt" } ;
	s.fuseNames             = QStringList{ "fscrypt" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "fscrypt" } ;
	s.volumePropertiesCommands = QStringList{ "fscrypt status %{plainFolder}" } ;
	s.notFoundCode             = engines::engine::status::fscryptNotFound ;

	return s ;
}

fscrypt::fscrypt() :
	engines::engine( _setOptions() ),
	m_requirePolkitOnUnmount( settings::instance().requirePolkitOnFsCryptUnmount() )
{
}

bool fscrypt::unmount( const QString& cipherFolder,
		       const QString& mountPoint,
		       int maxCount) const
{
	Q_UNUSED( cipherFolder )

	auto exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return false ;
	}

	if( m_requirePolkitOnUnmount ){

		if( !utility::enablePolkit() ){

			return false ;
		}

		exe += " purge " + mountPoint + " --force " + this->userOption() ;
	}else{
		exe += " purge " + mountPoint + " --force --drop-caches=false " + this->userOption() ;
	}

	for( int i = 0 ; i < maxCount ; i++ ){

		if( utility::Task::run( exe,m_requirePolkitOnUnmount ).get().success() ){

			return true ;
		}
	}

	return false ;
}

const engines::engine& fscrypt::proveEngine( const QString& cipherPath ) const
{
	if( !utility::platformIsLinux() ){

		return engines::instance().getUnKnown() ;
	}

	auto exe = this->executableFullPath() ;

	if( exe.isEmpty() ){

		return engines::instance().getUnKnown() ;
	}else{
		auto m = utility::Task::makePath( cipherPath ) ;

		auto s = utility::unwrap( utility::Task::run( exe + " status " + m ) ).success() ;

		if( s ){
			return *this ;
		}else{
			return engines::instance().getUnKnown() ;
		}
	}
}

Task::future< QString >& fscrypt::volumeProperties( const QString& cipherFolder,
						    const QString& mountPoint) const
{
	return Task::run( [ = ](){

		auto exe = this->executableFullPath() ;

		if( !exe.isEmpty() ){

			auto a = utility::split( cipherFolder,':' ) ;

			auto s = utility::Task::makePath( mountPoint ) ;

			if( a.size() > 1 ){

				exe = utility::Task::makePath( exe ) ;

				a = utility::split( a.at( 1 ) ) ;

				exe += " metadata dump --policy=" + s + ":" + a.at( 0 ) ;

				auto e = utility::unwrap( utility::Task::run( exe ) ) ;

				if( e.success() ){

					return QString( e.stdOut() ) ;
				}
			}
		}

		return QString() ;
	} ) ;
}

engines::engine::args fscrypt::command( const QString& password,
					const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	auto e = QString( "%1 %2 %3" ) ;

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.create ){

		auto m = "encrypt --source=custom_passphrase --name=\"%1\" --quiet %2" ;
		auto n = utility::split( args.opt.cipherFolder,'/' ).last() ;

		auto s = QString( m ).arg( n,this->userOption() ) ;
		exeOptions.add( s ) ;
	}else{
		exeOptions.add( QString( "unlock --quiet %1" ).arg( this->userOption() ) ) ;
	}

	auto cmd = e.arg( args.exe,
			  exeOptions.get(),
			  args.cipherFolder ) ;

	return { args,m,cmd } ;
}

engines::engine::status fscrypt::errorCode( const QString& e,int s ) const
{
	Q_UNUSED( s )

	if( e.contains( this->incorrectPasswordText() ) ){

		return engines::engine::status::fscryptBadPassword ;
	}else{
		return engines::engine::status::backendFail ;
	}
}

QString fscrypt::installedVersionString() const
{
	return QString() ;
}

void fscrypt::GUICreateOptionsinstance( QWidget *,engines::engine::function ) const
{
	//fscryptcreateoptions::instance( parent,std::move( function ) ) ;
}

#ifdef Q_OS_WIN

QString fscrypt::userOption() const
{
	return QString() ;
}

#else

#include <pwd.h>

QString fscrypt::userOption() const
{
	return QString( "--user=\"%1\"" ).arg( getpwuid( getuid() )->pw_name ) ;
}

#endif

