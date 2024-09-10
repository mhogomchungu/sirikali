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

#ifndef CHECKFORUPDATESWINDOW_H
#define CHECKFORUPDATESWINDOW_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QMenu>
#include <QFile>
#include <QLocale>
#include <QJsonObject>
#include <QTableWidgetItem>

#include <utility>
#include <functional>

#include "utils/network_access_manager.hpp"
#include "utils/qprocess.hpp"

namespace Ui {
class checkforupdateswindow;
}

class checkforupdateswindow : public QDialog
{
	Q_OBJECT
public:
	static bool updatable( const QString& ) ;

	using functions = std::pair< std::function< void() >,std::function< void() > > ;

	static checkforupdateswindow& instance( QWidget * parent,
						functions& function,
						utils::network::manager& nm )
	{
		return *( new checkforupdateswindow( parent,function,nm ) ) ;
	}
	checkforupdateswindow( QWidget * parent,functions&,utils::network::manager& ) ;
	struct args
	{
		QString executableName ;
		QString engineName ;
		QString installedVersion ;
		QString onLineVersion ;
		QString executableFullPath ;
		QString url ;
		QString error ;
		QJsonObject data ;
	} ;
	void add( const checkforupdateswindow::args& ) ;
	void done( bool ) ;
	void Show() ;
	~checkforupdateswindow() ;
signals:
	void closed() ;
private:
	class Ctx
	{
	public:
		Ctx( int row,const QString& path ) :
			m_row( row ),m_file( std::make_unique< QFile >( path ) )
		{
		}
		void fileClose()
		{
			m_file->close() ;
		}
		bool fileOpen()
		{
			return m_file->open( QIODevice::WriteOnly ) ;
		}
		void fileWrite( const QByteArray& data )
		{
			m_file->write( data ) ;
		}
		QString filePath()
		{
			return m_file->fileName() ;
		}
		Ctx move()
		{
			return std::move( *this ) ;
		}
		void fileRemove()
		{
			if( m_file->exists() ){

				m_file->remove() ;
			}
		}
		int row()
		{
			return m_row ;
		}
	private:
		int m_row ;
		std::unique_ptr< QFile > m_file ;
	} ;
	bool canBeUpdated( int ) ;
	void removeExtra( int ) ;
	void downloading( Ctx&,const utils::network::progress& ) ;
	void extracted( Ctx,const utils::qprocess::outPut& ) ;
	void currentItemChanged( QTableWidgetItem * ,QTableWidgetItem * ) ;
	void tableUpdate( int row,const QString& ) ;
	QString tableText( int row ) ;
	void download( int,const QString&,const QString& ) ;
	void extract( Ctx ) ;
	void update( int ) ;
	void closeEvent( QCloseEvent * ) override ;
	void closeUI() ;
	QNetworkRequest networkRequest( const QString& url ) ;

	Ui::checkforupdateswindow * m_ui ;
	functions& m_functions ;
	class opts
	{
	public:
		opts( const QJsonObject& s,const QString& m ) :
			m_data( s ),m_name( m )
		{
		}
		const QJsonObject& data() const
		{
			return m_data ;
		}
		const QString& name() const
		{
			return m_name ;
		}
	private:
		QJsonObject m_data ;
		QString m_name ;
	} ;

	class locale
	{
	public:
		QString formattedDataSize( qint64 ) const ;
		static QString secondsToString( int s )
		{
			if( s < 3600 ){

				return QTime( 0,0,0,0 ).addSecs( s ).toString( "mm:ss" ) ;
			}else{
				return QTime( 0,0,0,0 ).addSecs( s ).toString( "hh:mm:ss" ) ;
			}
		}
	private:
		QLocale m_locale ;
	};

	std::vector< opts > m_opts ;
	utils::network::manager& m_network ;
	locale m_locale ;
	QString m_binPath ;
	bool m_autoUpdate ;
	int m_position = -1 ;
};

#endif // CHECKFORUPDATESWINDOW_H
