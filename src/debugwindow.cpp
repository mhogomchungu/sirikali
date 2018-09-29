#include "debugwindow.h"
#include "ui_debugwindow.h"

debugWindow::debugWindow( QWidget * parent ) :
	QWidget( parent ),
	m_ui( new Ui::debugWindow )
{
	m_ui->setupUi( this ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	m_ui->pbClose->setFocus() ;

	connect( m_ui->pbClear,&QPushButton::clicked,[ this ](){

		m_ui->textEdit->setText( QString() ) ;
	} ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){

		this->Hide() ;
	} ) ;
}

debugWindow::~debugWindow()
{
	delete  m_ui ;
}

void debugWindow::Show()
{
	this->show() ;
}

void debugWindow::Hide()
{
	this->hide() ;
}

void debugWindow::UpdateOutPutSlot( const QString& e,bool s )
{
	if( this->isVisible() || s ){

		auto s = m_ui->textEdit->toPlainText() ;
		m_ui->textEdit->setText( s + "\n" + e ) ;
	}
}

void debugWindow::UpdateOutPut( const QString& e,bool m )
{
	QMetaObject::invokeMethod( this,
				   "UpdateOutPutSlot",
				   Qt::QueuedConnection,
				   Q_ARG( QString,e ),
				   Q_ARG( bool,m ) ) ;
}

void debugWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->hide() ;
}
