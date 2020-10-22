/*
 *
 *  Copyright (c) 2014-2015
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
#ifndef VOLUMEENTRYPROPERTIES_H
#define VOLUMEENTRYPROPERTIES_H

#include <QString>
#include <QStringList>

#include <vector>

class volumeInfo{
public:
	using List = std::vector< volumeInfo > ;

	volumeInfo()
	{
	}
	volumeInfo( QString c,QString m,QString f,QString mm,QString mo = QString() ) :
		m_cipherPath( std::move( c ) ),
		m_mountPoint( std::move( m ) ),
		m_fileSystem( std::move( f ) ),
		m_mode( std::move( mm ) ),
		m_mountOptions( std::move( mo ) )
	{
	}
	QStringList toStringList() const
	{
		return { m_cipherPath,
			 m_mountPoint,
			 m_fileSystem,
			 m_mode } ;
	}
	QString toString() const
	{
		QString a = "\"%1\" \"%2\" \"%3\" \"%4\"" ;
		return a.arg( m_cipherPath,m_mountPoint,m_fileSystem,m_mode ) ;
	}
	const QString& cipherPath() const
	{
		return m_cipherPath ;
	}
	const QString& mountPoint() const
	{
		return m_mountPoint ;
	}
	const QString& fileSystem() const
	{
		return m_fileSystem ;
	}
	QString& cipherPath()
	{
		return m_cipherPath ;
	}
	QString& mountPoint()
	{
		return m_mountPoint ;
	}
	QString& fileSystem()
	{
		return m_fileSystem ;
	}
	const QString& mode() const
	{
		return m_mode ;
	}
	const QString& mountOptions() const
	{
		return m_mountOptions ;
	}
	bool isValid() const
	{
		return !this->isNotValid() ;
	}
	bool isNotValid() const
	{
		return m_cipherPath.isEmpty() ;
	}
private:
	QString m_cipherPath ;
	QString m_mountPoint ;
	QString m_fileSystem ;
	QString m_mode ;
	QString m_mountOptions ;
} ;

#endif // VOLUMEENTRYPROPERTIES_H
