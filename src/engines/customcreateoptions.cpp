#include "customcreateoptions.h"
#include "ui_customcreateoptions.h"

customcreateoptions::customcreateoptions( const engines::engine::createGUIOptions& s ) :
	QDialog( s.parent ),
	m_ui( new Ui::customcreateoptions ),
	m_function( s.fCreateOptions )
{
	m_ui->setupUi( this ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->pbSet,&QPushButton::clicked,[ this ](){

		auto m = m_ui->lineEdit->text() ;

		this->HideUI( { m,QString(),QString() } ) ;
	} ) ;

	this->show() ;
}

customcreateoptions::~customcreateoptions()
{
	delete m_ui ;
}

void customcreateoptions::HideUI( const engines::engine::cOpts& e )
{
	this->hide() ;
	m_function( e ) ;
	this->deleteLater() ;
}

void customcreateoptions::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}
