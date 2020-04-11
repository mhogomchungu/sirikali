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

		QString fileDialogText ;
		QString checkBoxText    = QObject::tr( "Reverse Mode (Gocryptfs and Encfs Only)." ) ;
		QString configFileTitle = QObject::tr( "Unlock A Cryfs/Gocryptfs/Securefs Volume With Specified Configuration File." ) ;
		QString idleTitle       = QObject::tr( "Automatically Unmount After Specified Minutes of Inactivity Is Reached." ) ;

		enum class KEY{
			USES_KEYFILE_ONLY,
			USES_KEYFILE_AND_CONFIG_FILE,
			USES_CONFIG_FILE_ONLY
		} ;

		options::Options::KEY fileType = options::Options::KEY::USES_CONFIG_FILE_ONLY ;

		bool enableCheckBox = false ;
		bool checkBoxChecked = false ;
		bool enableIdleTime = true ;
		bool enableMountOptions = true ;
		bool enableConfigFile = true ;
		bool setVisiblePushButton_2 = false ;

		using function = std::function< void( engines::engine::options::booleanOptions&,const options::Options& ) > ;

		function updateOptions = []( engines::engine::options::booleanOptions& e,const options::Options& s ){

			Q_UNUSED( e )
			Q_UNUSED( s )
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
