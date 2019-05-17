#ifndef CUSTOMCREATEOPTIONS_H
#define CUSTOMCREATEOPTIONS_H

#include <QDialog>

#include "../engines.h"

namespace Ui {
class customcreateoptions;
}

class customcreateoptions : public QDialog
{
	Q_OBJECT
public:
	static void instance( QWidget * parent,std::function< void( const engines::engine::Options& ) > function )
	{
		new customcreateoptions( parent,std::move( function ) ) ;
	}
	customcreateoptions( QWidget * parent,std::function< void( const engines::engine::Options& ) > ) ;
	~customcreateoptions() ;
private:
	Ui::customcreateoptions * m_ui ;
	void HideUI( const engines::engine::Options& = engines::engine::Options() ) ;
	void closeEvent( QCloseEvent * ) ;
	std::function< void( const engines::engine::Options& ) > m_function ;
};

#endif // CUSTOMCREATEOPTIONS_H
