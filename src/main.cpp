#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(dnscrypt_proxy_icons);
    QApplication a(argc, argv);
    QString name("DNSCryptClient");
    a.setOrganizationName(name);
    a.setApplicationName(name);
    MainWindow w;
    w.hide();

    return a.exec();
}
