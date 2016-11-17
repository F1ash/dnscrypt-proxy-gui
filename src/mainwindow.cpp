#include "mainwindow.h"
#include "resolver_entries.h"
#include <private/qdbusutil_p.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    connection(QDBusConnection::systemBus())
{
    setSizePolicy(
                QSizePolicy(
                    QSizePolicy::MinimumExpanding,
                    QSizePolicy::MinimumExpanding));
    setWindowTitle("DNSCryptClient");
    QIcon::setThemeName("DNSCryptClient");
    setWindowIcon(QIcon::fromTheme(
                      "DNSCryptClient",
                      QIcon(":/DNSCryptClient.png")));
    setStyleSheet("QWidget {background-color: white;}");

    runAtStart = false;
    srvStatus = INACTIVE;
    findActiveService = false;
    stopManually = false;
    probeCount = 0;

    serverWdg = new ServerPanel(this);
    buttonsWdg = new ButtonPanel(this);
    infoWdg = new InfoPanel(this);
    baseLayout = new QVBoxLayout(this);
    baseLayout->addWidget(serverWdg, 1);
    baseLayout->addWidget(infoWdg, 3);
    baseLayout->addWidget(buttonsWdg, 5);
    baseWdg = new QWidget(this);
    baseWdg->setLayout(baseLayout);

    appSettings = new AppSettings(this);

    commonWdg = new QStackedWidget(this);
    commonWdg->addWidget(baseWdg);
    commonWdg->addWidget(appSettings);

    setCentralWidget(commonWdg);

    initTrayIcon();

    connect(serverWdg, SIGNAL(toSettings()),
            this, SLOT(toSettings()));
    connect(appSettings, SIGNAL(toBase()),
            this, SLOT(toBase()));
    connect(appSettings, SIGNAL(findActiveServiceStateChanged(bool)),
            this, SLOT(changeFindActiveServiceState(bool)));
    connect(appSettings, SIGNAL(runAtStartStateChanged(bool)),
            this, SLOT(changeRunAtStartState(bool)));
    connect(buttonsWdg, SIGNAL(startProxing()),
            this, SLOT(startService()));
    connect(buttonsWdg, SIGNAL(stopProxing()),
            this, SLOT(stopService()));
    connect(buttonsWdg, SIGNAL(restoreSettings()),
            this, SLOT(restoreSystemSettings()));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            buttonsWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            serverWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            infoWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(serverWdg, SIGNAL(serverData(const QVariantMap&)),
            infoWdg, SLOT(setServerDescription(const QVariantMap&)));
    connect(this, SIGNAL(nextServer(const QString)),
            this, SLOT(probeNextServer(const QString)));

    readSettings();
}

void MainWindow::readSettings()
{
    restoreGeometry(settings.value("Geometry").toByteArray());
    runAtStart = settings.value("RunAtStart", false).toBool();
    appSettings->setRunAtStartState(runAtStart);
    if ( runAtStart ) {
        startServiceProcess();
    };
    findActiveService = settings.value("FindActiveService", true).toBool();
    appSettings->setFindActiveServiceState(findActiveService);
    QString lastServer = settings.value("LastServer").toString();
    serverWdg->setLastServer(lastServer);
}
void MainWindow::setSettings()
{
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("RunAtStart", runAtStart);
    settings.setValue("FindActiveService", findActiveService);
    settings.setValue("LastServer", serverWdg->getCurrentServer());
}
void MainWindow::initTrayIcon()
{
    trayIcon = new TrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(trayIcon->closeAction, SIGNAL(triggered(bool)),
            this, SLOT(close()));
    trayIcon->show();
}
void MainWindow::changeVisibility()
{
    ( this->isVisible() ) ? this->hide() : this->show();
}
void MainWindow::connectToClientService()
{
    if ( !connection.isConnected() ) return;
    connection = QDBusConnection::systemBus();
    bool connected = connection.connect(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1/unit/DNSCryptClient_2eservice",
                "org.freedesktop.DBus.Properties",
                "PropertiesChanged",
                this,
                SLOT(servicePropertyChanged(QDBusMessage)));
    const QString _state = ( connected )? "Connected" : "Not connected";
    KNotification::event(
                KNotification::Notification,
                "DNSCryptClient",
                QString("%1 to org.freedesktop.systemd1").arg(_state));
}
bool MainWindow::checkServiceStatus()
{
    connectToClientService();
    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1/unit/DNSCryptClient_2eservice",
                "org.freedesktop.DBus.Properties",
                "Get");
    QList<QVariant> _args;
    _args<<"org.freedesktop.systemd1.Unit"<<"ActiveState";
    msg.setArguments(_args);
    bool sent = connection.callWithCallback(
                msg, this, SLOT(receiveServiceStatus(QDBusMessage)));
    return sent;
}
void MainWindow::changeAppState()
{
    switch ( srvStatus ) {
    case INACTIVE:
    case FAILED:
        if ( runAtStart ) {
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_reload",
                                         QIcon(":/reload.png")));
            startServiceProcess();
        } else {
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_closed",
                                         QIcon(":/closed.png")));
        };
        //trayIcon->stopAction->setEnabled(false);
        //trayIcon->startAction->setEnabled(true);
        break;
    case ACTIVE:
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_opened",
                                     QIcon(":/opened.png")));
        //trayIcon->startAction->setEnabled(false);
        //trayIcon->stopAction->setEnabled(true);
        runAtStart = false;
        break;
    case DEACTIVATING:
    case   ACTIVATING:
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_reload",
                                     QIcon(":/reload.png")));
        //trayIcon->stopAction->setEnabled(false);
        //trayIcon->startAction->setEnabled(false);
        break;
    default:
        break;
    };
}
void MainWindow::startServiceProcess()
{
    trayIcon->setIcon(
                QIcon::fromTheme("DNSCryptClient_reload",
                                 QIcon(":/reload.png")));
    QVariantMap args;
    Action act;
    switch (srvStatus) {
    //case INACTIVE:
    //    args["action"] = "create";
    //    act.setName("pro.russianfedora.dnscryptclient.create");
    //    break;
    case INACTIVE:
    case FAILED:
        args["action"] = "create";
        args["server"] = serverWdg->getCurrentServer();
        act.setName("pro.russianfedora.dnscryptclient.create");
        break;
    default:
        return;
    };
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setAutoDelete(true);
    if (job->exec()) {
        QString code        = job->data().value("code").toString();
        QString msg         = job->data().value("msg").toString();
        QString err         = job->data().value("err").toString();
        QString entry       = job->data().value("entry").toString();
        addServerEnrty(entry);
        currentUnitName.clear();
        currentUnitName     = job->data().value("name").toString();
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("Session open with exit code: %1\nMSG: %2\nERR: %3")
                   .arg(code).arg(msg).arg(err));
    } else {
        currentUnitName.clear();
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_closed",
                                     QIcon(":/closed.png")));
        emit serviceStateChanged(srvStatus);
        checkServiceStatus();
    };
}
void MainWindow::stopServiceProcess()
{
    QVariantMap args;
    args["action"] = "stop";
    args["server"] = serverWdg->getCurrentServer();
    Action act("pro.russianfedora.dnscryptclient.stop");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setAutoDelete(true);
    if (job->exec()) {
        QString code = job->data().value("code").toString();
        QString msg  = job->data().value("msg").toString();
        QString err  = job->data().value("err").toString();
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("Session closed with exit code: %1\nMSG: %2\nERR: %3")
                   .arg(code).arg(msg).arg(err));
    } else {
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_close",
                                     QIcon(":/close.png")));
        emit serviceStateChanged(srvStatus);
        checkServiceStatus();
    };
}
void MainWindow::findActiveServiceProcess()
{
    if ( srvStatus==FAILED || srvStatus==INACTIVE ) {
        if ( ++probeCount <= serverWdg->getServerListCount() ) {
            serverWdg->setNextServer();
            QString _nextServer = serverWdg->getCurrentServer();
            emit nextServer(_nextServer);
        } else {
            KNotification::event(
                        KNotification::Notification,
                        "DNSCryptClient",
                        "All servers probed and failed");
        };
    } else if ( srvStatus==ACTIVE || srvStatus==RESTORED ) {
        probeCount = 0;
    }
}
void MainWindow::addServerEnrty(const QString &entry)
{
    if ( entry.isEmpty() ) return;
    if ( resolverEntries.contains(entry) ) return;
    resolverEntries.append(entry);
}
QString MainWindow::showResolverEntries() const
{
    ResolverEntries *d = new ResolverEntries(serverWdg);
    d->setEntries(resolverEntries);
    d->exec();
    QString ret = d->getEntry();
    d->deleteLater();
    return ret;
}

/* private slots */
void MainWindow::toSettings()
{
    commonWdg->setCurrentWidget(appSettings);
}
void MainWindow::toBase()
{
    commonWdg->setCurrentWidget(baseWdg);
}
void MainWindow::changeFindActiveServiceState(bool state)
{
    findActiveService = state;
}
void MainWindow::changeRunAtStartState(bool state)
{
    runAtStart = state;
}
void MainWindow::startService()
{
    startServiceProcess();
}
void MainWindow::stopService()
{
    probeCount = 0;
    stopManually = true;
    stopServiceProcess();
}
void MainWindow::restoreSystemSettings()
{
    stopService();
    QString selectedEntry = showResolverEntries();
    if ( selectedEntry.isEmpty() ) {
        selectedEntry = "nameserver 8.8.8.8\n";
    };
    QVariantMap args;
    args["action"] = "restore";
    args["entry"]  = selectedEntry;
    Action act("pro.russianfedora.dnscryptclient.restore");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setAutoDelete(true);
    if (job->exec()) {
        QString code = job->data().value("code").toString();
        QString msg  = job->data().value("msg").toString();
        QString err  = job->data().value("err").toString();
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("Restore exit code: %1\nMSG: %2\nERR: %3")
                   .arg(code).arg(msg).arg(err));
        srvStatus = RESTORED;
    } else {
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
    };
    emit serviceStateChanged(srvStatus);
}
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason r)
{
    if ( r==QSystemTrayIcon::Trigger ) changeVisibility();
}
void MainWindow::servicePropertyChanged(QDBusMessage message)
{
    QList<QVariant> args = message.arguments();
    if ( args.first().toString()!=
         "org.freedesktop.systemd1.Unit" ) return;
    checkServiceStatus();
}
void MainWindow::closeEvent(QCloseEvent *ev)
{
    if ( ev->type()==QEvent::Close ) {
        stopServiceProcess();
        setSettings();
        trayIcon->hide();
        ev->accept();
    };
}
void MainWindow::receiveServiceStatus(QDBusMessage _msg)
{
    QList<QVariant> args = _msg.arguments();
    if ( args.length()!=1 ) return;
    QVariant arg = _msg.arguments().first();
    QString str = QDBusUtil::argumentToString(arg);
    QStringList l = str.split('"');
    if ( l.length()<3 ) return;
    QString status = l.at(1);
    KNotification::event(
                KNotification::Notification,
                "DNSCryptClient",
                QString("DNSCryptClient is %1.").arg(status));
    if        ( status=="inactive" ) {
        srvStatus = INACTIVE;
    } else if ( status=="active" ) {
        srvStatus = ACTIVE;
    } else if ( status=="failed" ) {
        srvStatus = FAILED;
    } else if ( status=="activating" ) {
        srvStatus = ACTIVATING;
    } else if ( status=="deactivating" ) {
        srvStatus = DEACTIVATING;
    } else if ( status=="reloading" ) {
        srvStatus = RELOADING;
    } else {
        srvStatus = INACTIVE;
    };
    emit serviceStateChanged(srvStatus);
    changeAppState();
    if ( !stopManually && findActiveService ) {
        findActiveServiceProcess();
    };
    stopManually = false;
}
void MainWindow::probeNextServer(const QString _nextServer)
{
    // set next server to service parameter
    startServiceProcess();
}
