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

#include "../backends.h"

class gocryptfs : public backEnd::engine
{
public:
	gocryptfs();
	const QStringList& names() const override ;
	const QStringList& fuseNames() const override ;
	siritask::status notFoundCode() const override ;
	QString command( const backEnd::cmdArgsList& args ) const override ;
	siritask::status errorCode( const QString& e,int s ) const override ;
	bool setsCipherPath() const override ;
	QString configFileArgument() const override ;
	QStringList configFileNames() const override ;
	bool autoMountsOnCreate() const override ;
	QString setPassword( const QString& ) const override ;
	bool hasGUICreateOptions() const override ;
	QString defaultCreateOptions() const override ;
	void GUICreateOptionsinstance( QWidget * parent,std::function< void( const Options& ) > ) const override ;
private:
	QStringList m_names{ "gocryptfs","gocryptfs.reverse" } ;
	QStringList m_fuseNames{ "fuse.gocryptfs","fuse.gocryptfs-reverse" } ;
};
