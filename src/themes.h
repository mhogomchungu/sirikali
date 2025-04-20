/*
 *
 *  Copyright (c) 2021
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

#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QStringList>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QApplication>
#include <QStyleFactory>

#include <vector>

class themes
{
public:
	themes( QApplication& app,const QString& themeConfigPath,const QString& themeName ) ;
private:
	class JObject
	{
	public:
		void insert( const char * key,int a,int b,int c,int d )
		{
			QJsonObject obj ;

			QJsonArray arr ;

			arr.append( a ) ;
			arr.append( b ) ;
			arr.append( c ) ;
			arr.append( d ) ;

			obj.insert( "rgba",std::move( arr ) ) ;

			m_obj.insert( key,std::move( obj ) ) ;
		}
		void insert( const char * key,const char * subkey,const char * value )
		{
			QJsonObject obj ;

			obj.insert( subkey,value ) ;

			m_obj.insert( key,std::move( obj ) ) ;
		}
		void insert( const char * key,const char * value )
		{
			m_obj.insert( key,value ) ;
		}
		operator QJsonObject()
		{
			return std::move( m_obj ) ;
		}
	private:
		QJsonObject m_obj ;
	} ;
	themes::JObject baseTheme() const ;
	QJsonObject darkTheme() const ;
	QJsonObject lightTheme() const ;
	void setTheme( QApplication& app,const QJsonObject& obj ) const ;
	QColor getColor( const QString& e,const QJsonObject& obj ) const ;
} ;

#endif
