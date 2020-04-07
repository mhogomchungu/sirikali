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
	static void instance( QWidget * parent,engines::engine::functionOptions function )
	{
		new customcreateoptions( parent,std::move( function ) ) ;
	}
	customcreateoptions( QWidget * parent,engines::engine::functionOptions ) ;
	~customcreateoptions() ;
private:
	Ui::customcreateoptions * m_ui ;
	void HideUI( const engines::engine::Options& = engines::engine::Options() ) ;
	void closeEvent( QCloseEvent * ) ;
	engines::engine::functionOptions m_function ;
};

#endif // CUSTOMCREATEOPTIONS_H
