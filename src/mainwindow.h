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
#include "app_settings.h"
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
    bool                runAtStart, findActiveService, stopManually;
    SRV_STATUS          srvStatus;
    ServerPanel        *serverWdg;
    ButtonPanel        *buttonsWdg;
    InfoPanel          *infoWdg;
    TrayIcon           *trayIcon;
    QVBoxLayout        *baseLayout;
    QWidget            *baseWdg;
    AppSettings        *appSettings;
    QStackedWidget     *commonWdg;
    QSettings           settings;
    QDBusConnection     connection;
    QStringList         resolverEntries;

    void                readSettings();
    void                setSettings();
    void                initTrayIcon();
    void                changeVisibility();
    void                connectToClientService();
    bool                checkServiceStatus();
    void                changeAppState();
    void                startServiceProcess();
    void                stopServiceProcess();
    void                findActiveServiceProcess();
    void                addServerEnrty(const QString&);

private slots:
    void                toSettings();
    void                toBase();
    void                changeFindActiveServiceState(bool);
    void                changeRunAtStartState(bool);
    void                startService();
    void                stopService();
    void                restoreSystemSettings();
    void                trayIconActivated(QSystemTrayIcon::ActivationReason);
    void                servicePropertyChanged(QDBusMessage);
    void                closeEvent(QCloseEvent*);
    void                receiveServiceStatus(QDBusMessage);
};

#endif // MAINWINDOW_H
