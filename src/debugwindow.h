#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class debugWindow;
}

class debugWindow : public QWidget
{
	Q_OBJECT
public:
        explicit debugWindow( QWidget * parent = nullptr ) ;
	~debugWindow();
        void Show() ;
        void Hide() ;
        void UpdateOutPut( const QString& ) ;
        void closeEvent( QCloseEvent * ) ;
private slots:
        void UpdateOutPutSlot( const QString& ) ;
private:
        Ui::debugWindow * m_ui ;
};

#endif // DEBUGWINDOW_H
