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
	static void instance( const engines::engine::createGUIOptions& s )
	{
		new customcreateoptions( s ) ;
	}
	customcreateoptions( const engines::engine::createGUIOptions& s ) ;
	~customcreateoptions() ;
private:
	Ui::customcreateoptions * m_ui ;
	void HideUI( const engines::engine::cOpts& = engines::engine::cOpts() ) ;
	void closeEvent( QCloseEvent * ) ;
	engines::engine::fcreate m_function ;
};

#endif // CUSTOMCREATEOPTIONS_H
