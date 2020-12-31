#include "traywidget.h"

TrayIcon::TrayIcon(
        QWidget *parent,
        QString  ver)
  : QSystemTrayIcon(parent)
{
    setIcon(QIcon::fromTheme("DNSCryptClient",
                             QIcon(":/DNSCryptClient.png")));
    setToolTip("DNSCryptClient");

    reinitAction = new QAction(QString("Re-initializate"), this);
        reinitAction->setIcon (
                    QIcon::fromTheme("reload", QIcon(":/reload.png")));

    closeAction = new QAction(QString("Exit"), this);
    closeAction->setIcon (
                QIcon::fromTheme("exit", QIcon(":/exit.png")));

    trayIconMenu = new QMenu(parent);
    trayIconMenu->addAction(reinitAction);
    reinitAction->setVisible(false);
    if ( ver.compare("2")>0 ) {
        trayIconMenu->addSeparator();
        reinitAction->setVisible(true);
    };
    trayIconMenu->addAction(closeAction);

    setContextMenu(trayIconMenu);
    setVisible(true);
}
