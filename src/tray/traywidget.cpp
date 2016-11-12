#include "traywidget.h"

TrayIcon::TrayIcon(QWidget *parent)
  : QSystemTrayIcon(parent)
{
    setIcon(QIcon::fromTheme("DNSCryptClient",
                             QIcon(":/DNSCryptClient.png")));
    setToolTip("DNSCryptClient");
    /*
    startAction = new QAction(QString("Start connection"), this);
    startAction->setIcon (
                QIcon::fromTheme("DNSCryptClient_reload",
                                 QIcon(":/reload.png")));
    stopAction = new QAction(QString("Stop connection"), this);
    stopAction->setIcon (
                QIcon::fromTheme("DNSCryptClient_close",
                                 QIcon(":/close.png")));
    */
    closeAction = new QAction(QString("Exit"), this);
    closeAction->setIcon (
                QIcon::fromTheme("exit", QIcon(":/exit.png")));

    trayIconMenu = new QMenu(parent);
    /*
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(startAction);
    trayIconMenu->addAction(stopAction);
    trayIconMenu->addSeparator();
    */
    trayIconMenu->addAction(closeAction);

    setContextMenu(trayIconMenu);
    setVisible(true);
}
