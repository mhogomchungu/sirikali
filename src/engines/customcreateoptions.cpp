#include "customcreateoptions.h"
#include "ui_customcreateoptions.h"

customcreateoptions::customcreateoptions( QWidget * parent,
					std::function< void( const engines::engine::Options& ) > function ) :
	QDialog( parent ),
	m_ui( new Ui::customcreateoptions ),
	m_function( std::move( function ) )
{
	m_ui->setupUi( this ) ;

	connect( m_ui->pbCancel,&QPushButton::clicked,[ this ](){

		this->HideUI() ;
	} ) ;

	connect( m_ui->pbSet,&QPushButton::clicked,[ this ](){

		auto m = m_ui->lineEdit->text() ;

		this->HideUI( { { m } } ) ;
	} ) ;

	this->show() ;
}

customcreateoptions::~customcreateoptions()
{
	delete m_ui ;
}

void customcreateoptions::HideUI( const engines::engine::Options& e )
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
