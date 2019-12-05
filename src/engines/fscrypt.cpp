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
#include "../settings.h"
#include "../mountinfo.h"
#include <vector>

static const char * _spaceToken = " " ;

static QStringList _encrypted_volumes( const QString& list )
{
	QStringList l ;

	auto s = utility::split( list,'\n' ) ;

	for( int i = s.size() - 1 ; i >= 0 ; i-- ){

		const auto& x =s.at( i ) ;

		if( x.startsWith( "MOUNTPOINT" ) ){

			break ;
		}else{
			auto a = utility::split( x,_spaceToken ) ;

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

static std::vector< protector > _get_protectors( int& s,const QStringList& l )
{
	std::vector< protector > m ;

	for( ; s < l.size() ; s++ ){

		const auto& e = l.at( s ) ;

		if( e.startsWith( "POLICY" ) ){

			s++ ;
			break ;
		}

		auto a = utility::split( e,_spaceToken ) ;

		if( e.size() > 4 ){

			m.emplace_back( a.at( 0 ),a.at( 4 ) ) ;
		}
	}

	return m ;
}

static std::vector< policy > _get_policies( int& s,const QStringList& l )
{
	std::vector< policy > m ;

	for( ; s < l.size() ; s++ ){

		const auto& e = l.at( s ) ;

		auto a = utility::split( e,_spaceToken ) ;

		if( a.size() == 3 ){

			m.emplace_back( a.at( 0 ),a.at( 1 ),a.at( 2 ) ) ;

		}else if( a.size() == 5 ){

			m.emplace_back( a.at( 0 ),a.at( 1 ),a.at( 4 ) ) ;
		}
	}

	return m ;
}

static std::vector< policy > _get_policies( const QString& m,const QString& exe )
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

	auto protectors = _get_protectors( i,s ) ;
	auto policies   = _get_policies( i,s ) ;

	for( auto& it : policies ){

		for( const auto& xt : protectors ) {

			if( it.ptr == xt.ptr ){

				it.comment = xt.comment ;
			}
		}
	}

	return policies ;
}

static QString _get_fs_mode( const QStringList& s,const QString& m )
{
	for( const auto& it : s ){

		if( it.contains( " " + m + " " ) ){

			auto a = utility::split( it,_spaceToken ) ;

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

static QStringList _fscrypt_mount_points( const QString& exe )
{
	auto status = utility::unwrap( utility::Task::run( exe + " status" ) ).stdOut() ;

	if( status.isEmpty() ){

		return {} ;
	}

	return _encrypted_volumes( status ) ;
}

static QStringList _fscrypt_volumes( const QStringList& s,const QString& exe,const QStringList& n )
{
	QStringList l ;

	for( const auto& it : _fscrypt_mount_points( exe ) ){

		auto m = _get_policies( it,exe ) ;

		for( const auto& xt : m ){

			if( !xt.comment.startsWith( "\"volname=" ) ){

				continue ;
			}

			auto cf = utility::policyString() + xt.poly ;
			auto md = _get_fs_mode( s,it ) ;
			auto mp = xt.comment ;
			mp.remove( 0,9 ) ;
			mp.remove( mp.length() - 1,1 ) ;

			if( utility::pathNotExists( mp ) ){

				continue ;
			}

			if( xt.unlocked == "Yes" ){

				l.append( mountinfo::mountProperties( mp,md,n.at( 0 ),cf ) ) ;

			}else if( xt.unlocked.startsWith( "Partially" ) ){

				l.append( mountinfo::mountProperties( mp,md,n.at( 1 ),cf ) ) ;
			}
		}
	}

	return l ;
}

static QString _mount_point( const QString& e,const QString& exe )
{
	for( const auto& it : _fscrypt_mount_points( exe ) ){

		if( e.startsWith( it ) ){

			return it ;
		}
	}

	return QString() ;
}

static QString _volume_properties( const QString& cipherFolder,
				   const QString& mountPoint,
				   const QString& e )
{
	if( e.isEmpty() ){

		return QString() ;
	}

	auto a = utility::split( cipherFolder,':' ) ;

	if( a.size() < 2 ){

		return QString() ;
	}

	auto s = utility::Task::makePath( mountPoint ) ;

	auto exe = utility::Task::makePath( e ) ;

	auto m = _mount_point( mountPoint,exe ) ;

	if( m.isEmpty() ){

		return QString() ;
	}

	exe += " metadata dump --policy=" + m + ":" + a.at( 1 ) ;

	auto r = utility::unwrap( utility::Task::run( exe ) ) ;

	if( r.success() ){

		return r.stdOut() ;
	}else{
		return QString() ;
	}
}

static engines::engine::BaseOptions _setOptions()
{
	engines::engine::BaseOptions s ;

	s.backendTimeout              = 0 ;
	s.takesTooLongToUnlock        = false ;
	s.supportsMountPathsOnWindows = true ;
	s.autorefreshOnMountUnMount   = false ;
	s.backendRequireMountPath     = false ;
	s.requiresPolkit        = false ;
	s.customBackend         = false ;
	s.requiresAPassword     = true ;
	s.hasConfigFile         = false ;
	s.autoMountsOnCreate    = true ;
	s.hasGUICreateOptions   = false ;
	s.setsCipherPath        = true ;
	s.passwordFormat        = "%{password}" ;
	s.executableName        = "fscrypt" ;
	s.incorrectPasswordText = "incorrect key provided" ;
	s.configFileNames       = QStringList{ ".fscrypt" } ;
	s.fuseNames             = QStringList{ "fscrypt","fscrypt*" } ;
	s.failedToMountList     = QStringList{ "Error" } ;
	s.names                 = QStringList{ "fscrypt","fscrypt*" } ;
	s.volumePropertiesCommands = QStringList{ "fscrypt status %{plainFolder}" } ;
	s.notFoundCode             = engines::engine::status::fscryptNotFound ;

	return s ;
}

fscrypt::fscrypt() : engines::engine( _setOptions() )
{
}

engines::engine::status fscrypt::unmount( const QString& cipherFolder,
					  const QString& mountPoint,
					  int maxCount) const
{
	Q_UNUSED( cipherFolder )

	auto exe = this->executablePath() ;

	if( exe.isEmpty() ){

		return engines::engine::status::failedToUnMount ;
	}

	exe = utility::Task::makePath( exe ) ;

	exe += " lock " + mountPoint + " " + this->userOption() ;

	for( int i = 0 ; i < maxCount ; i++ ){

		auto s = utility::unwrap( utility::Task::run( exe,false ) ) ;

		if( s.success() ){

			return engines::engine::status::success ;

		}else if( s.stdError().contains( "Directory was incompletely locked because some files are still open" ) ){

			return engines::engine::status::fscryptPartialVolumeClose ;
		}
	}

	return engines::engine::status::failedToUnMount ;
}

QStringList fscrypt::mountInfo( const QStringList& a ) const
{
	return Task::await( [ & ](){

		return _fscrypt_volumes( a,this->executablePath(),this->names() ) ;
	} ) ;
}

const engines::engine& fscrypt::proveEngine( const QString& cipherPath ) const
{
	if( !utility::platformIsLinux() ){

		return engines::instance().getUnKnown() ;
	}

	auto exe = this->executablePath() ;

	if( exe.isEmpty() ){

		return engines::instance().getUnKnown() ;
	}else{
		auto m = utility::Task::makePath( cipherPath ) ;

		exe = utility::Task::makePath( exe ) ;

		auto s = utility::unwrap( utility::Task::run( exe + " status " + m ) ).success() ;

		if( s ){

			return *this ;
		}else{
			return engines::instance().getUnKnown() ;
		}
	}
}

bool fscrypt::volumeFailedRequirenment( const engines::engine::options& e ) const
{
	Q_UNUSED( e )
	return false ;
}

Task::future< QString >& fscrypt::volumeProperties( const QString& cipherFolder,
						    const QString& mountPoint ) const
{
	return Task::run( [ = ](){

		return _volume_properties( cipherFolder,mountPoint,this->executablePath() ) ;
	} ) ;
}

engines::engine::args fscrypt::command( const QByteArray& password,
					const engines::engine::cmdArgsList& args ) const
{
	Q_UNUSED( password )

	auto e = QString( "%1 %2 %3" ) ;

	engines::engine::commandOptions m( args,this->name(),this->name() ) ;

	auto exeOptions = m.exeOptions() ;

	if( args.create ){

		auto m = "encrypt --source=custom_passphrase --name=volname=\"%1\" --quiet %2" ;
		auto s = QString( m ).arg( args.opt.cipherFolder,this->userOption() ) ;
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

const QString& fscrypt::executablePath() const
{
	if( m_exe.isEmpty() ){

		m_exe = this->executableFullPath() ;
	}

	return m_exe ;
}

#endif

