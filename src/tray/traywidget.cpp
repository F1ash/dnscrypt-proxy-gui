#include "traywidget.h"

TrayIcon::TrayIcon(QWidget *parent)
  : QSystemTrayIcon(parent)
{
    setIcon(QIcon::fromTheme("DNSCryptClient",
                             QIcon(":/DNSCryptClient.png")));
    setToolTip("DNSCryptClient");

    closeAction = new QAction(QString("Exit"), this);
    closeAction->setIcon (
                QIcon::fromTheme("exit", QIcon(":/exit.png")));

    trayIconMenu = new QMenu(parent);
    trayIconMenu->addAction(closeAction);

    setContextMenu(trayIconMenu);
    setVisible(true);
}
