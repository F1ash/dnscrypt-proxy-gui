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
    void                nextServer();

private:
    bool                runAtStart, findActiveService, stopManually;
    int                 probeCount;
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
    QString             currentUnitTranscription;

    void                readSettings();
    void                setSettings();
    void                initTrayIcon();
    void                changeVisibility();
    void                connectToClientService();
    void                disconnectFromClientService();
    void                checkServiceStatus();
    int                 checkSliceStatus();
    void                startServiceProcess();
    void                stopServiceProcess();
    void                stopSliceProcess();
    void                findActiveServiceProcess();
    void                addServerEnrty(const QString&);
    QString             showResolverEntries() const;

private slots:
    void                toSettings();
    void                toBase();
    void                firstServiceStart();
    void                changeFindActiveServiceState(bool);
    void                startService();
    void                stopService();
    void                restoreSystemSettings();
    void                trayIconActivated(QSystemTrayIcon::ActivationReason);
    void                servicePropertyChanged(QDBusMessage);
    void                closeEvent(QCloseEvent*);
    void                receiveServiceStatus(QDBusMessage);
    void                changeAppState(SRV_STATUS);
    void                probeNextServer();
};

#endif // MAINWINDOW_H
