#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QSettings>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include "server_panel.h"
#include "button_panel.h"
#include "info_panel.h"
#include "tray/traywidget.h"
#include <kauth.h>
#include <knotification.h>
using namespace KAuth;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void                serviceStateChanged(SRV_STATUS);

private:
    bool                startFlag;
    SRV_STATUS          srvStatus;
    ServerPanel        *serverWdg;
    ButtonPanel        *buttonsWdg;
    InfoPanel          *infoWdg;
    TrayIcon           *trayIcon;
    QVBoxLayout        *baseLayout;
    QWidget            *baseWdg;
    QSettings           settings;
    QDBusConnection     connection;

    void                initTrayIcon();
    void                changeVisibility();
    void                connectToClientService();
    bool                checkServiceStatus();
    void                changeAppState();
    void                startClientProcess();
    void                stopClientProcess();

private slots:
    void                trayIconActivated(QSystemTrayIcon::ActivationReason);
    void                servicePropertyChanged(QDBusMessage);
    void                closeEvent(QCloseEvent*);
    void                startConnection();
    void                stopConnection();
    void                receiveServiceStatus(QDBusMessage);
};

#endif // MAINWINDOW_H
