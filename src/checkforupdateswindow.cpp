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

#include "checkforupdateswindow.h"
#include "ui_checkforupdateswindow.h"

#include <QDebug>
#include <QTextEdit>

#include "settings.h"
#include "sirikali.h"

checkforupdateswindow::checkforupdateswindow( sirikali& parent,utils::network::manager& nm ) :
	QDialog( &parent ),
	m_ui( new Ui::checkforupdateswindow ),
	m_network( nm ),
	m_binPath( engines::defaultBinPath() ),
	m_sirikali( parent )
{
	m_ui->setupUi( this ) ;

	m_ui->pbOK->setEnabled( false ) ;

	m_ui->tableWidget->setEnabled( false ) ;

	m_ui->tableWidget->setMouseTracking( false ) ;

	m_ui->tableWidget->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	this->window()->setWindowTitle( tr( "Version Info" ) ) ;

	this->setFixedSize( this->window()->size() ) ;

	connect( m_ui->pbOK,&QPushButton::clicked,[ this ](){

		this->closeUI() ;
	} ) ;

	auto cm = &QTableWidget::customContextMenuRequested ;

	connect( m_ui->tableWidget,cm,[ this ]( QPoint ){

		auto row = m_ui->tableWidget->currentRow() ;

		if( row != -1 ){

			QMenu m ;

			auto ac = m.addAction( tr( "Update" ) ) ;

			if( m_opts[ row ].engine().updatable( true ) ){

				ac->setEnabled( true ) ;

				connect( ac,&QAction::triggered,[ this,row ](){

					this->update( row ) ;
				} ) ;
			}else{
				ac->setText( tr( "Update Disabled" ) ) ;

				ac->setEnabled( false ) ;
			}

			m.exec( QCursor::pos() ) ;
		}
	} ) ;

	connect( m_ui->tableWidget,
		 &QTableWidget::currentItemChanged,
		 this,
		 &checkforupdateswindow::currentItemChanged ) ;
}

void checkforupdateswindow::add( const checkforupdateswindow::args& s )
{
	m_opts.emplace_back( s ) ;

	auto txt = m_opts.back().displayName() ;

	if( s.error.isEmpty() ){

		txt += "\n" + tr( "Installed Version: %1" ).arg( s.installedVersion ) ;
		txt += "\n" + tr( "Latest Version: %1 " ).arg( s.onLineVersion ) ;
	}else{
		txt += "\n" + s.error ;
	}

	auto row = m_ui->tableWidget->rowCount() ;

	m_ui->tableWidget->insertRow( row ) ;

	auto item = new QTableWidgetItem( txt ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	m_ui->tableWidget->setItem( row,0,item ) ;
	m_ui->tableWidget->resizeRowToContents( row ) ;
	m_ui->tableWidget->setCurrentCell( row,0 ) ;
}

void checkforupdateswindow::doneUpdating( bool s )
{
	if( s ){

		m_position = 0 ;

		this->update( m_position ) ;
	}else{
		m_ui->pbOK->setEnabled( true ) ;
		m_ui->tableWidget->setEnabled( true ) ;
	}
}

void checkforupdateswindow::Show()
{
	this->show() ;
	this->raise() ;
	this->setWindowState( Qt::WindowActive ) ;
}

checkforupdateswindow::~checkforupdateswindow()
{
	delete m_ui ;
}

void checkforupdateswindow::removeExtra( int row )
{
	m_opts[ row ].engine().removeExtraFiles( m_binPath ) ;
}

void checkforupdateswindow::downloading( Ctx& ctx,const utils::network::progress& p )
{
	int row = ctx.row() ;

	if( p.finished() ){

		ctx.fileClose() ;

		if( p.success() ){

			auto mm = this->tableText( row ) ;

			const auto& ee = m_opts[ row ].engineName() ;

			mm.replace( tr( "Downloading %1" ).arg( ee ),"" ) ;

			auto m = tr( "Download Complete" ) ;

			this->tableUpdate( row,m + mm ) ;

			if( this->archivePath( ctx.filePath() ) ){

				this->extract( ctx.move() ) ;
			}else{
				this->noNeedToExtract( ctx.move() ) ;
			}
		}else{
			auto m = tr( "Error" ) + "\n" + tr( "Download Failed" ) ;

			this->tableUpdate( row,m ) ;

			m_ui->pbOK->setEnabled( true ) ;
		}

	}else if( p.timeOut() ){

		m_ui->pbOK->setEnabled( true ) ;

		m_ui->tableWidget->setEnabled( true ) ;

		auto m = tr( "Error" ) + "\n" + tr( "Network Timed Out" ) ;

		this->tableUpdate( row,m ) ;

		m_ui->pbOK->setEnabled( true ) ;
	}else{
		ctx.fileWrite( p.data() ) ;

		auto total = p.total() ;

		QString m ;

		if( total == 0 ){

			auto current = m_locale.formattedDataSize( p.received() ) ;

			m = QString( "%1" ).arg( current ) ;
		}else{
			auto perc = double( p.received() )  * 100 / double( total ) ;
			auto totalSize = m_locale.formattedDataSize( total ) ;
			auto current   = m_locale.formattedDataSize( p.received() ) ;
			auto percentage = QString::number( perc,'f',2 ) ;

			m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;
		}

		const auto& mm = m_opts[ row ].displayName() ;

		this->tableUpdate( row,tr( "Downloading %1" ).arg( mm ) + "\n" + m ) ;
	}
}

void checkforupdateswindow::currentItemChanged( QTableWidgetItem * c, QTableWidgetItem * p )
{
	if( c ){

		c->setSelected( true ) ;
	}

	if( p ){

		p->setSelected( false ) ;
	}
}

void checkforupdateswindow::tableUpdate( int row,const QString& s )
{
	if( m_opts[ row ].noError() ){

		m_ui->tableWidget->item( row,0 )->setText( s ) ;
	}
}

QString checkforupdateswindow::tableText( int row )
{
	return m_ui->tableWidget->item( row,0 )->text() ;
}

bool checkforupdateswindow::archivePath( const QString& e )
{
	return e.endsWith( ".zip" ) || e.endsWith( ".gz" ) ;
}

QNetworkRequest checkforupdateswindow::networkRequest( const QString& url )
{
	QNetworkRequest networkRequest( url ) ;
#if QT_VERSION >= QT_VERSION_CHECK( 5,9,0 )
	auto a = QNetworkRequest::RedirectPolicyAttribute ;
	auto b = QNetworkRequest::NoLessSafeRedirectPolicy ;

	networkRequest.setAttribute( a,b ) ;
#else
	#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
		auto c = QNetworkRequest::FollowRedirectsAttribute ;
		networkRequest.setAttribute( c,true ) ;
	#endif
#endif
	return networkRequest ;
}

void checkforupdateswindow::download( int row,
				      const QString& url,
				      const QString& archiveName,
				      const QString& tagName )
{
	class progress
	{
	public:
		progress( checkforupdateswindow * p,const QString& path,int row,const QString& tagName ) :
			m_parent( *p ),m_ctx( row,path,tagName )
		{
		}
		Ctx& ctx()
		{
			return m_ctx ;
		}
		void operator()( const utils::network::progress& p )
		{
			m_parent.downloading( m_ctx,p ) ;
		}
		progress move()
		{
			return std::move( *this ) ;
		}
	private:
		checkforupdateswindow& m_parent ;
		Ctx m_ctx ;
	} ;

	progress p( this,m_binPath + archiveName,row,tagName ) ;

	auto& cc = p.ctx() ;

	cc.fileRemove() ;

	m_ui->tableWidget->setEnabled( false ) ;
	m_ui->pbOK->setEnabled( false ) ;

	if( cc.fileOpen() ){

		m_network.get( this->networkRequest( url ),p.move() ) ;
	}else{
		auto m = cc.filePath() ;

		this->tableUpdate( row,tr( "Failed To Open Path For Writing" ) + "\n" + m ) ;
	}
}

void checkforupdateswindow::extract( Ctx ctx )
{
	int row = ctx.row() ;

	auto mm = QObject::tr( "Extracting archive" ) + "\n" + ctx.filePath() ;

	this->tableUpdate( row,mm ) ;

	auto cexe = utility::platformIsWindows() ? "bsdtar.exe" : "bsdtar" ;

	auto exe = engines::executableNotEngineFullPath( cexe ) ;

	if( exe.isEmpty() ){

		auto m = QObject::tr( "Failed To Extract" ) ;

		auto mm = [](){

			if( utility::platformIsWindows() ){

				return QObject::tr( "Failed To Find \"bsdtar.exe\" Executable" ) ;
			}else{
				return QObject::tr( "Failed To Find \"bsdtar\" Executable" ) ;
			}
		}() ;

		this->tableUpdate( row,m + "\n" + mm ) ;

		m_ui->pbOK->setEnabled( true ) ;
	}else{
		m_opts[ row ].engine().deleteBinPath( m_binPath ) ;

		auto args = QStringList{ "-x","-f",ctx.filePath(),"-C",m_binPath } ;

		utils::qprocess::run( exe,args,ctx.move(),this,&checkforupdateswindow::extracted ) ;
	}
}

void checkforupdateswindow::noNeedToExtract( Ctx ctx )
{
	this->updateComplete( ctx ) ;

	this->goToNext() ;
}

void checkforupdateswindow::updateComplete( const Ctx& ctx )
{
	int row = ctx.row() ;
	const auto& exePath = ctx.filePath() ;

	auto m = QObject::tr( "Update Complete" ) ;

	if( this->archivePath( exePath ) ){

		auto e = m_opts[ row ].engine().setExecutablePermissions( m_binPath ) ;

		this->tableUpdate( row,m + "\n" + e ) ;
	}else{
		this->tableUpdate( row,m + "\n" + exePath ) ;

		QFile f( exePath ) ;

		f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
	}

	m_opts[ row ].engine().setInstalledVersionHack( m_binPath,ctx.tagName() ) ;
}

void checkforupdateswindow::goToNext()
{
	if( m_position != -1 ){

		m_position++ ;

		if( m_position < static_cast< int >( m_opts.size() ) ){

			this->update( m_position ) ;
		}else{
			m_ui->tableWidget->setEnabled( true ) ;

			m_ui->pbOK->setEnabled( true ) ;
		}
	}else{
		m_ui->tableWidget->setEnabled( true ) ;

		m_ui->pbOK->setEnabled( true ) ;
	}
}

void checkforupdateswindow::extracted( Ctx ctx,const utils::qprocess::outPut& p )
{
	int row = ctx.row() ;

	if( p.success() ){

		this->updateComplete( ctx ) ;

		this->removeExtra( row ) ;

		this->goToNext() ;
	}else{
		m_ui->pbOK->setEnabled( true ) ;

		auto m = QObject::tr( "Failed To Extract" ) ;

		const auto& mm = p.stdError ;

		this->tableUpdate( row,m + "\n" + ctx.filePath() + "\n" + mm ) ;
	}

	ctx.fileRemove() ;
}

void checkforupdateswindow::update( int row )
{
	if( m_opts[ row ].noError() ){

		const auto array = m_opts[ row ].data().value( "assets" ).toArray() ;

		auto tag_name = m_opts[ row ].data().value( "tag_name" ).toString() ;

		for( const auto& it : array ){

			auto obj = it.toObject() ;

			auto url = obj.value( "browser_download_url" ).toString() ;

			if( m_opts[ row ].engine().onlineArchiveFileName( url ) ){

				auto m = obj.value( "name" ).toString() ;

				return this->download( row,url,m,tag_name ) ;
			}
		}

		auto mm = tr( "Failed To Find Archive To Download" ) ;

		this->tableUpdate( row,tr( "Error" ) + "\n" + mm ) ;
	}else{
		m_ui->pbOK->setEnabled( true ) ;
	}
}

void checkforupdateswindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->closeUI() ;
}

void checkforupdateswindow::closeUI()
{
	m_sirikali.enableAll() ;
	this->hide() ;
	this->deleteLater() ;

	emit closed() ;
}

QString checkforupdateswindow::locale::formattedDataSize( qint64 s ) const
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,14,0 )
	return m_locale.formattedDataSize( s ) ;
#else
	std::array< const char *,7 > sizes = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" } ;

	qint64  multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL ;

	QString result ;

	for( size_t i = 0 ; i < sizes.size() ; i++,multiplier /= 1024 ){

		if( s < multiplier ){

			continue ;
		}

		if( s % multiplier == 0 ){

			auto a = QString::number( s / multiplier ) ;
			auto b = sizes[ i ] ;

			result = QString( "%1 %2" ).arg( a,b ) ;
		}else{
			auto a = static_cast< double >( s ) / static_cast< double >( multiplier ) ;
			auto b = sizes[ i ] ;
			auto c = QString::number( a,'f',2 ) ;

			result = QString( "%1 %2" ).arg( c,b ) ;
		}

		return result ;
	}

	return {} ;
#endif
}
