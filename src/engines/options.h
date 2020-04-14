/*
 *
 *  Copyright (c) 2012-2015
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QAction>
#include <QCloseEvent>
#include <QCheckBox>

#include <functional>

#include "../engines.h"

namespace Ui {
class options;
}

class options : public QDialog
{
	Q_OBJECT
public:
	struct Options{

		QString keyFile ;
		QString idle ;
		QString configFilePath ;
		QString mountOptions ;		

		bool enableCheckBox = true ;
		bool checkBoxChecked = true ;
		bool enableIdleTime = true ;
		bool enableMountOptions = true ;
		bool enableConfigFile = true ;
		bool enableKeyFile = true ;

		QString checkBoxText ;
		QString keyFileTitle ;

		using boolOpts = engines::engine::options::booleanOptions ;

		using function = std::function< boolOpts( const options::Options& ) > ;

		function updateOptions = []( const options::Options& s ){

			Q_UNUSED( s )

			return boolOpts() ;
		} ;
	};

	static options& instance( QWidget * parent,
				  const engines::engine& engine,
				  bool r,
				  const engines::engine::mountOptions& l,
				  engines::engine::fMountOptions e )
	{
		return *( new options( parent,engine,r,l,std::move( e ) ) ) ;
	}

	options( QWidget * parent,
		 const engines::engine&,
		 bool,
		 const engines::engine::mountOptions&,
		 engines::engine::fMountOptions ) ;
	~options() ;
	void ShowUI() ;
	Options& GUIOptions() ;
private slots:
        void pushButton( void ) ;
	void pushButton_2( void ) ;
	void pbSet( void ) ;
	void pbCancel( void ) ;
private:
	void Hide( const engines::engine::mountOptions& = engines::engine::mountOptions() ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::options * m_ui ;
	const engines::engine& m_engine ;
	bool m_create ;
	std::function< void( const engines::engine::mountOptions& ) > m_setOptions ;
	QWidget * m_parentWidget ;
	Options m_setGUIOptions ;
};

#endif // OPTIONS_H
