#ifndef TRAYWIDGET_H
#define TRAYWIDGET_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
//#include <QDebug>

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public :
    explicit TrayIcon(
            QWidget *parent = Q_NULLPTR,
            QString  ver    = "1.x.x");
    QAction     *reinitAction;
    QAction     *closeAction;

private :
    QMenu       *trayIconMenu;
};

#endif
