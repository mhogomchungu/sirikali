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
	static void instance( QWidget * parent,engines::engine::fCreateOptions function )
	{
		new customcreateoptions( parent,std::move( function ) ) ;
	}
	customcreateoptions( QWidget * parent,engines::engine::fCreateOptions ) ;
	~customcreateoptions() ;
private:
	Ui::customcreateoptions * m_ui ;
	void HideUI( const engines::engine::createOptions& = engines::engine::createOptions() ) ;
	void closeEvent( QCloseEvent * ) ;
	engines::engine::fCreateOptions m_function ;
};

#endif // CUSTOMCREATEOPTIONS_H
