#include "mainwindow.h"
#include "resolver_entries.h"
#include <private/qdbusutil_p.h>
#include <QRegExp>
#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
//#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    connection(QDBusConnection::systemBus())
{
    //const QSize d = QApplication::desktop()->screenGeometry().size();
    //setMaximumHeight(d.height()/2);
    //setMaximumWidth(d.width()/2);
    setWindowTitle("DNSCryptClient");
    QIcon::setThemeName("DNSCryptClient");
    setWindowIcon(QIcon::fromTheme(
                      "DNSCryptClient",
                      QIcon(":/DNSCryptClient.png")));
    //setStyleSheet("QWidget {background-color: white;}");

    runAtStart = false;
    srvStatus = INACTIVE;
    findActiveService = false;
    stopManually = false;
    restoreFlag = false;
    restoreAtClose = false;
    stopForChangeUnits = false;
    probeCount = 0;

    serverWdg = new ServerPanel(this);
    buttonsWdg = new ButtonPanel(this);
    infoWdg = new InfoPanel(this);
    baseLayout = new QVBoxLayout();
    baseLayout->addWidget(serverWdg, 1);
    baseLayout->addWidget(infoWdg, 3);
    baseLayout->addWidget(buttonsWdg, 5);
    baseLayout->addStretch(-1);
    baseWdg = new QWidget(this);
    baseWdg->setContentsMargins(0, 0, 0, 0);
    baseWdg->setLayout(baseLayout);

    appSettings = new AppSettings(this);
    testRespond = new TestRespond(this);

    commonWdg = new QStackedWidget(this);
    commonWdg->addWidget(baseWdg);
    commonWdg->addWidget(appSettings);
    commonWdg->addWidget(testRespond);
    commonWdg->setContentsMargins(0, 0, 0, 0);

    setContentsMargins(0, 0, 0, 0);
    setCentralWidget(commonWdg);

    initTrayIcon();

    connect(serverWdg, SIGNAL(toSettings()),
            this, SLOT(toSettings()));
    connect(serverWdg, SIGNAL(toTest()),
            this, SLOT(toTest()));
    connect(appSettings, SIGNAL(toBase()),
            this, SLOT(toBase()));
    connect(testRespond, SIGNAL(toBase()),
            this, SLOT(toBase()));
    connect(testRespond->testWdg, SIGNAL(started()),
            this, SLOT(testStarted()));
    connect(testRespond->testWdg, SIGNAL(finished()),
            this, SLOT(testFinished()));
    connect(testRespond->testWdg, SIGNAL(serverRespondIcon(QString,QString)),
            serverWdg, SLOT(setItemIcon(QString,QString)));
    connect(appSettings, SIGNAL(findActiveServiceStateChanged(bool)),
            this, SLOT(changeFindActiveServiceState(bool)));
    connect(appSettings, SIGNAL(useFastOnlyStateChanged(bool)),
            this, SLOT(changeUseFastOnlyState(bool)));
    connect(appSettings, SIGNAL(restoreAtCloseChanged(bool)),
            this, SLOT(changeRestoreAtCloseState(bool)));
    connect(buttonsWdg, SIGNAL(startProxing()),
            this, SLOT(startService()));
    connect(buttonsWdg, SIGNAL(stopProxing()),
            this, SLOT(stopService()));
    connect(buttonsWdg, SIGNAL(restoreSettings()),
            this, SLOT(restoreSystemSettings()));
    connect(serverWdg, SIGNAL(serverData(const QVariantMap&)),
            infoWdg, SLOT(setServerDescription(const QVariantMap&)));
    connect(serverWdg, SIGNAL(serverActivated()),
            this, SLOT(startService()));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            serverWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            buttonsWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            infoWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            testRespond->testWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            this, SLOT(changeAppState(SRV_STATUS)));
    connect(serverWdg, SIGNAL(readyForStart()),
            this, SLOT(firstServiceStart()));
    connect(serverWdg, SIGNAL(checkItem(QString,QString)),
            this, SLOT(checkRespondSettings(QString,QString)));
    connect(this, SIGNAL(nextServer()),
            this, SLOT(probeNextServer()));
    connect(appSettings, SIGNAL(jobPortChanged(int)),
            this, SLOT(changeJobPort(int)));
    connect(appSettings, SIGNAL(testPortChanged(int)),
            this, SLOT(changeTestPort(int)));
    connect(appSettings, SIGNAL(userChanged(QString)),
            this, SLOT(changeUserName(QString)));
    connect(appSettings, SIGNAL(stopSystemdAppUnits()),
            this, SLOT(stopSystemdAppUnits()));
    connect(appSettings, SIGNAL(changeUnitsFinished()),
            this, SLOT(changeUnitsFinished()));

    setStatusBar(new QStatusBar(this));
    getDNSCryptProxyServiceVersion();
    //readSettings();
}

void MainWindow::readSettings()
{
    QByteArray _geometry = settings.value("Geometry").toByteArray();
    runAtStart = settings.value("RunAtStart", false).toBool();
    appSettings->setRunAtStartState(runAtStart);
    unhideAtStart = settings.value("UnhideAtStart", true).toBool();
    appSettings->setUnhideAtStartState(unhideAtStart);
    findActiveService = settings.value("FindActiveService", true).toBool();
    appSettings->setFindActiveServiceState(findActiveService);
    useFastOnly = settings.value("UseFastOnly", false).toBool();
    appSettings->setUseFastOnlyState(useFastOnly);
    restoreAtClose = settings.value("RestoreAtClose", true).toBool();
    appSettings->setRestoreAtClose(restoreAtClose);
    QString lastServer = settings.value("LastServer").toString();
    serverWdg->setLastServer(lastServer);
    jobPort = settings.value("JobPort", JOB_PORT).toInt();
    appSettings->jobPort->setPort(jobPort);
    testPort = settings.value("TestPort", TEST_PORT).toInt();
    appSettings->testPort->setPort(testPort);
    testRespond->testWdg->setTestPort(testPort);
    asUser = settings.value("AsUser").toString();
    appSettings->setUserName(asUser);
    settings.beginGroup("Entries");
    foreach ( QString _key, settings.allKeys() ) {
        resolverEntries.append(settings.value(_key).toString());
    };
    settings.endGroup();
    if ( _geometry.isEmpty() ) {
        adjustSize();
    } else {
        restoreGeometry(_geometry);
    };
}
void MainWindow::setSettings()
{
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("RunAtStart", appSettings->getRunAtStartState());
    settings.setValue("UnhideAtStart", appSettings->getUnhideAtStartState());
    settings.setValue("JobPort", jobPort);
    settings.setValue("TestPort", testPort);
    settings.setValue("AsUser", asUser);
    settings.setValue("FindActiveService", findActiveService);
    settings.setValue("UseFastOnly", useFastOnly);
    settings.setValue("LastServer", serverWdg->getCurrentServer());
    settings.setValue("RestoreAtClose", restoreAtClose);
    settings.beginGroup("Entries");
    uint i = 0;
    foreach ( QString _val, resolverEntries ) {
        ++i;
        settings.setValue(QString::number(i), _val);
    };
    settings.endGroup();
    settings.beginGroup("Responds");
    settings.remove("");
    for ( int i=0; i<serverWdg->getServerListCount(); i++ ) {
        QString name, respondIconName;
        name = serverWdg->getItemName(i);
        respondIconName = serverWdg->getRespondIconName(i);
        settings.setValue(name, respondIconName);
    };
    settings.endGroup();
    settings.beginGroup("Enables");
    settings.remove("");
    for ( int i=0; i<serverWdg->getServerListCount(); i++ ) {
        QString name; bool state = false;
        name = serverWdg->getItemName(i);
        state = serverWdg->getItemState(i);
        settings.setValue(name, state);
    };
    settings.endGroup();
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
    if ( !connection.isConnected() ) {
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("Not connected to org.freedesktop.systemd1"));
        return;
    };
    connection = QDBusConnection::systemBus();
    currentUnitTranscription.clear();
    currentUnitTranscription = serverWdg->getCurrentServer()
            .replace("-", "_2d").replace(".", "_2e");
    QString dbusPath = QString("DNSCryptClient_40%1_2eservice")
            .arg(currentUnitTranscription);
    bool connected = connection.connect(
                "org.freedesktop.systemd1",
                QString("/org/freedesktop/systemd1/unit/%1").arg(dbusPath),
                "org.freedesktop.DBus.Properties",
                "PropertiesChanged",
                this,
                SLOT(servicePropertyChanged(QDBusMessage)));
    const QString _state = ( connected )? "Connected" : "Not connected";
    KNotification::event(
                KNotification::Notification,
                "DNSCryptClient",
                QString("%1 for monitoring status of service").arg(_state));
}
void MainWindow::disconnectFromClientService()
{
    if ( !connection.isConnected() ) {
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("Not connected to org.freedesktop.systemd1"));
        return;
    };
    connection = QDBusConnection::systemBus();
    QString dbusPath = QString("DNSCryptClient_40%1_2eservice")
            .arg(currentUnitTranscription);
    bool disconnected = connection.disconnect(
                "org.freedesktop.systemd1",
                QString("/org/freedesktop/systemd1/unit/%1").arg(dbusPath),
                "org.freedesktop.DBus.Properties",
                "PropertiesChanged",
                this,
                SLOT(servicePropertyChanged(QDBusMessage)));
    const QString _state = ( disconnected )? "" : "not";
    KNotification::event(
                KNotification::Notification,
                "DNSCryptClient",
                QString("Monitoring %1 disconnected.").arg(_state));
}
void MainWindow::checkServiceStatus()
{
    QString dbusPath = QString("DNSCryptClient_40%1_2eservice")
            .arg(currentUnitTranscription);
    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                QString("/org/freedesktop/systemd1/unit/%1").arg(dbusPath),
                "org.freedesktop.DBus.Properties",
                "Get");
    QVariantList _args;
    _args<<"org.freedesktop.systemd1.Unit"<<"ActiveState";
    msg.setArguments(_args);
    bool sent = connection.callWithCallback(
                msg, this, SLOT(receiveServiceStatus(QDBusMessage)));
    //return sent;
}
int  MainWindow::checkSliceStatus()
{
    //QTextStream s(stdout);
    int ret = -2;
    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1/unit/system_2dDNSCryptClient_2eslice",
                "org.freedesktop.DBus.Properties",
                "Get");
    QVariantList _args;

    _args<<"org.freedesktop.systemd1.Unit"<<"ActiveState";
    msg.setArguments(_args);
    QDBusMessage answer = connection.call(msg);
    if ( answer.arguments().length()!=1 ) return ret;
    QVariant arg = answer.arguments().first();
    QString str = QDBusUtil::argumentToString(arg);
    //s << str << endl;
    QStringList l = str.split('"');
    if ( l.length()<3 ) return ret;
    QString status = l.at(1);
    if        ( status=="inactive" ) {
        // not active proxy
        return 0;
    } else if ( status=="failed" ) {
        // service failed; to STOP_SLICE
        return 0;
    } else if ( status=="active" ) {
        // if active then check task count
        _args.clear();
        _args<<"org.freedesktop.systemd1.Slice"<<"TasksCurrent";
        msg.setArguments(_args);
        answer = connection.call(msg);
        qulonglong currTasks;
        QRegExp rx("[0-1]{1}");
        QStringList captured;
        switch (answer.type()) {
        case QDBusMessage::ReplyMessage:
            arg = answer.arguments().first();
            str = QDBusUtil::argumentToString(arg);
            //s << str << endl;
            rx.indexIn(str);
            captured = rx.capturedTexts();
            //foreach (QString _cap, captured) {
            //    s << _cap << endl;
            //};
            if ( !captured.isEmpty() ) {
                bool ok;
                currTasks = captured.first().toULongLong(&ok);
                if        ( ok && currTasks==0 ) {
                    // not active proxy
                    ret = 0;
                } else if ( ok && currTasks==1 ) {
                    // one active proxy
                    ret = 1;
                } else {
                    // TasksCurrent not [0-1]; to STOP_SLICE
                    ret = -1;
                };
            } else {
                // TasksCurrent not [0-1]; to STOP_SLICE
                ret = -1;
            };
            break;
        case QDBusMessage::ErrorMessage:
        default:
            ret = -1;
            break;
        };
    } else {
        ret = -2; // slice not exist
    };
    //s << "ret "<<ret << endl;
    return ret;
}
void MainWindow::startServiceProcess()
{
    QVariantMap args;
    args["action"] = "start";
    args["port"]   = jobPort;
    args["server"] = serverWdg->getCurrentServer();
    Action act("pro.russianfedora.dnscryptclient.start");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(startServiceJobFinished(KJob*)));
    job->start();
}
void MainWindow::stopServiceProcess()
{
    // unused;
    // for stop and remove instantiated services used 'stopSliceProcess'
    disconnectFromClientService();
    QVariantMap args;
    args["action"] = "stop";
    args["server"] = serverWdg->getCurrentServer();
    Action act("pro.russianfedora.dnscryptclient.stop");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(stopServiceJobFinished(KJob*)));
    job->start();
}
void MainWindow::stopSliceProcess()
{
    disconnectFromClientService();
    QVariantMap args;
    args["action"] = "stopslice";
    Action act("pro.russianfedora.dnscryptclient.stopslice");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(stopsliceJobFinished(KJob*)));
    job->start();
}
void MainWindow::restoreSettingsProcess()
{
    restoreFlag = false;
    QString selectedEntry = showResolverEntries();
    if ( selectedEntry.isEmpty() ) {
        emit serviceStateChanged(RESTORED);
        return;
    };
    QVariantMap args;
    args["action"] = "restore";
    args["entry"]  = selectedEntry;
    Action act("pro.russianfedora.dnscryptclient.restore");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(finished(KJob*)),
            this, SLOT(restoreSettingsProcessFinished(KJob*)));
    job->start();
}
void MainWindow::findActiveServiceProcess()
{
    if ( srvStatus==FAILED || srvStatus==INACTIVE ) {
        if ( ++probeCount <= serverWdg->getServerListCount() ) {
            serverWdg->setNextServer();
            emit nextServer();
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
QString MainWindow::showResolverEntries()
{
    ResolverEntries *d = new ResolverEntries(serverWdg);
    d->setEntries(resolverEntries);
    d->exec();
    QString ret = d->getEntry();
    resolverEntries.clear();
    resolverEntries.append(d->getEntries());
    d->deleteLater();
    return ret;
}

/* private slots */
void MainWindow::toSettings()
{
    commonWdg->setCurrentWidget(appSettings);
    appSettings->jobPort->setPort(jobPort);
    appSettings->testPort->setPort(testPort);
    appSettings->applyNewPortsBtn->setEnabled(false);
}
void MainWindow::toTest()
{
    commonWdg->setCurrentWidget(testRespond);
}
void MainWindow::toBase()
{
    commonWdg->setCurrentWidget(baseWdg);
}
void MainWindow::testStarted()
{
    stopService();
    emit serviceStateChanged(PROCESSING);
    trayIcon->setIcon(
                QIcon::fromTheme("DNSCryptClient_test",
                                 QIcon(":/test.png")));
    trayIcon->setToolTip(
                QString("%1\n%2").arg(windowTitle()).arg("--testing--"));
}
void MainWindow::testFinished()
{
    /*
    emit serviceStateChanged(READY);
    trayIcon->setIcon(
                QIcon::fromTheme("DNSCryptClient_closed",
                                 QIcon(":/closed.png")));
    trayIcon->setToolTip(QString("%1\n%2")
                         .arg(windowTitle())
                         .arg("--tested--"));
    stopService();
    */

    //QTextStream s(stdout);
    if ( srvStatus==FAILED   ||
         srvStatus==INACTIVE ||
         srvStatus==READY ) {
        if ( stopForChangeUnits ) {
            stopForChangeUnits = false;
            appSettings->runChangeUnits();
            //s << "runChangeUnits ";
        };
    };
    //s << "testFinished" << endl;
}
void MainWindow::checkRespondSettings(const QString name, const QString icon)
{
    settings.beginGroup("Responds");
    if ( !settings.allKeys().contains(name) ) {
        settings.setValue(name, icon);
    };
    settings.endGroup();
}
void MainWindow::firstServiceStart()
{
    settings.beginGroup("Responds");
    testRespond->testWdg->setServerList(settings.allKeys());
    //foreach ( QString _key, settings.allKeys() ) {
    //    QString respondIconName =
    //            settings.value(_key).toString();
    //    if ( respondIconName.isEmpty() ) respondIconName.append("none");
    //    serverWdg->setItemIcon( _key, respondIconName );
    //};
    settings.endGroup();
    // check and change job\test ports to according with settings
    // if application was updated or reinstalled
    appSettings->runChangeUnits();
}
void MainWindow::startServiceJobFinished(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
        QString code        = job->data().value("code").toString();
        //QString msg         = job->data().value("msg").toString();
        //QString err         = job->data().value("err").toString();
        QString entry       = job->data().value("entry").toString();
        QString answ        = job->data().value("answ").toString();
        QString resp_icon   = job->data().value("resp").toString();
        //QTextStream s(stdout);
        //s << "dns entries  : " << answ << endl;
        //s << "response time: " << resp_icon << endl;
        //KNotification::event(
        //           KNotification::Notification,
        //           "DNSCryptClient",
        //           QString("Session open with exit code: %1\nMSG: %2\nERR: %3")
        //           .arg(code).arg(msg).arg(err));
        addServerEnrty(entry);
        if ( code.toInt()==0 ) {
            if ( answ.toInt()<1 ) {
                serverWdg->setItemIcon(
                            serverWdg->getCurrentServer(), "none");
                serverWdg->changeServerInfo();
                emit serviceStateChanged(STOP_SLICE);
                return;
            };
            serverWdg->setItemIcon(
                        serverWdg->getCurrentServer(), resp_icon);
        } else {
            serverWdg->setItemIcon(
                        serverWdg->getCurrentServer(), "none");
        };
    } else {
        //QTextStream s(stdout);
        //s << "action failed" << endl;
        //KNotification::event(
        //           KNotification::Notification,
        //           "DNSCryptClient",
        //           QString("Start Service ERROR: %1\n%2")
        //           .arg(job->error()).arg(job->errorText()));
        //trayIcon->setIcon(
        //            QIcon::fromTheme("DNSCryptClient_closed",
        //                             QIcon(":/closed.png")));
        serverWdg->setItemIcon(
                    serverWdg->getCurrentServer(), "none");
        emit serviceStateChanged(STOP_SLICE);
        return;
    };
    serverWdg->changeServerInfo();
    switch (checkSliceStatus()) {
    case  1:
        emit serviceStateChanged(ACTIVE);
        break;
    case -2:
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case -1:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::stopServiceJobFinished(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
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
                   QString("Stop status unknown."));
    };
    switch (checkSliceStatus()) {
    case -2:
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:
    case -1:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::restoreSettingsProcessFinished(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
        QString code = job->data().value("code").toString();
        QString msg  = job->data().value("msg").toString();
        QString err  = job->data().value("err").toString();
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("Restore exit code: %1\nMSG: %2\nERR: %3")
                   .arg(code).arg(msg).arg((err.isEmpty())? "None" : err));
        if ( code.toInt()!=-1 ) srvStatus = RESTORED;
    } else {
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: restore status unknown."));
    };
    emit serviceStateChanged(srvStatus);
}
void MainWindow::stopsliceJobFinished(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
        //QString code = job->data().value("code").toString();
        //QString msg  = job->data().value("msg").toString();
        //QString err  = job->data().value("err").toString();
        //KNotification::event(
        //           KNotification::Notification,
        //           "DNSCryptClient",
        //           QString("Slice closed with exit code: %1\nMSG: %2\nERR: %3")
        //           .arg(code).arg(msg).arg(err));
    } else {
        //KNotification::event(
        //           KNotification::Notification,
        //           "DNSCryptClient",
        //           QString("Stop Slice ERROR: %1\n%2")
        //           .arg(job->error()).arg(job->errorText()));
        //trayIcon->setIcon(
        //            QIcon::fromTheme("DNSCryptClient_close",
        //                             QIcon(":/close.png")));
    };
    switch (checkSliceStatus()) {
    case -2:
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:
    case -1:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::changeFindActiveServiceState(bool state)
{
    findActiveService = state;
}
void MainWindow::changeUseFastOnlyState(bool state)
{
    useFastOnly = state;
}
void MainWindow::changeRestoreAtCloseState(bool state)
{
    restoreAtClose = state;
}
void MainWindow::changeJobPort(int port)
{
    jobPort = port;
    //QTextStream s(stdout);
    //s << "job port" << jobPort << endl;
}
void MainWindow::changeTestPort(int port)
{
    testPort = port;
    testRespond->testWdg->setTestPort(testPort);
    //QTextStream s(stdout);
    //s << "test port" << testPort << endl;
}
void MainWindow::changeUserName(QString _user)
{
    asUser = _user;
}
void MainWindow::startService()
{
    probeCount = 0;
    stopManually = false;
    switch (checkSliceStatus()) {
    case -2:
    case  0:     // ready for start
        if ( serverWdg->serverIsEnabled() ) {
            startServiceProcess();
            emit serviceStateChanged(PROCESSING);
        } else {
            KNotification::event(
                        KNotification::Notification,
                        "DNSCryptClient",
                        QString("%1 isn't enabled in list.")
                        .arg(serverWdg->getCurrentServer()));
            emit serviceStateChanged(STOP_SLICE);
        };
        break;
    case  1:    // incorrectly for start;
    case -1:    // errored answer
    default:    // need to restart the slice and proxying
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::stopService()
{
    probeCount = 0;
    stopManually = true;
    switch (checkSliceStatus()) {
    case -2:
    case  0:    // stop slice or service unnecessary
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:    // need to stop
    case -1:    // errored answer
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::restoreSystemSettings()
{
    probeCount = 0;
    restoreFlag = true;
    stopManually = true;
    stopSliceProcess();
}
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason r)
{
    if ( r==QSystemTrayIcon::Trigger ) changeVisibility();
}
void MainWindow::servicePropertyChanged(QDBusMessage message)
{
    QVariantList args = message.arguments();
    if ( args.first().toString()!=
         "org.freedesktop.systemd1.Unit" ) return;
    checkServiceStatus();
}
void MainWindow::closeEvent(QCloseEvent *ev)
{
    ev->accept();
    stopManually = true;
    testRespond->testWdg->stopTest();
    if ( restoreAtClose && srvStatus!=RESTORED ) {
        show();
        stopSliceProcess();
        restoreSettingsProcess();
    } else {
        stopSliceProcess();
    };
    setSettings();
    trayIcon->hide();
}
void MainWindow::receiveServiceStatus(QDBusMessage _msg)
{
    QVariantList args = _msg.arguments();
    if ( args.length()!=1 ) return;
    QVariant arg = _msg.arguments().first();
    QString str = QDBusUtil::argumentToString(arg);
    QStringList l = str.split('"');
    if ( l.length()<3 ) return;
    QString status = l.at(1);

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
}
void MainWindow::changeAppState(SRV_STATUS status)
{
    SRV_STATUS prevSrvStatus = srvStatus;
    if ( status!=READY && status!=PROCESSING ) srvStatus = status;
    //QTextStream s(stdout);
    switch ( status ) {
        case INACTIVE:
        case FAILED:
            if ( prevSrvStatus!=srvStatus ) {
                KNotification::event(
                            KNotification::Notification,
                            "DNSCryptClient",
                            QString("DNSCryptClient service is %1.")
                            .arg((status==FAILED)? "failed" : "inactive"));
            };
            //s << "INACTIVE/FAILED" << endl;
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_closed",
                                         QIcon(":/closed.png")));
            trayIcon->setToolTip(QString("%1\n%2")
                                 .arg(windowTitle())
                                 .arg("--stopped--"));
            if ( !stopManually && findActiveService ) {
                findActiveServiceProcess();
                emit serviceStateChanged(PROCESSING);
            } else if ( restoreFlag ) {
                emit serviceStateChanged(PROCESSING);
                restoreSettingsProcess();
            } else {
                emit serviceStateChanged(READY);
            };
            if ( stopForChangeUnits ) {
                if ( !testRespond->testWdg->isActive() ) {
                    stopForChangeUnits = false;
                    appSettings->runChangeUnits();
                    //s << "runChangeUnits ";
                };
                //s << "srvStatus inactive" << endl;
            };
            break;
        case ACTIVE:
            if ( prevSrvStatus!=srvStatus ) {
                KNotification::event(
                            KNotification::Notification,
                            "DNSCryptClient",
                            QString("DNSCryptClient service is active."));
            };
            connectToClientService();
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_opened",
                                         QIcon(":/opened.png")));
            trayIcon->setToolTip(QString("%1\n%2")
                                 .arg(windowTitle())
                                 .arg(serverWdg->getCurrentServer()));
            //s << "ACTIVE" << endl;
            emit serviceStateChanged(READY);
            break;
        case DEACTIVATING:
        case   ACTIVATING:
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_reload",
                                         QIcon(":/reload.png")));
            //s << "DEACTIVATING/ACTIVATING" << endl;
            break;
        case STOP_SLICE:
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient",
                                         QIcon(":/DNSCryptClient.png")));
            // need to restart the slice and proxying
            //s << "STOP_SLICE" << endl;
            emit serviceStateChanged(PROCESSING);
            stopSliceProcess();
            break;
        case RESTORED:
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_restore",
                                         QIcon(":/restore.png")));
            trayIcon->setToolTip(QString("%1\n%2")
                                 .arg(windowTitle())
                                 .arg("--restored--"));
            emit serviceStateChanged(READY);
            break;
        default:
            break;
    };
}
void MainWindow::probeNextServer()
{
    if ( serverWdg->serverIsEnabled() ) {
        if ( useFastOnly ) {
            if ( 0!=serverWdg->getCurrentRespondIconName().compare("fast") ) {
                if ( findActiveService ) {
                    findActiveServiceProcess();
                };
                return;
            };
        };
        startServiceProcess();
    } else if ( findActiveService ) {
        findActiveServiceProcess();
    };
}
void MainWindow::stopSystemdAppUnits()
{
    //QTextStream s(stdout);
    //s << "stopSystemdAppUnits" << endl;
    stopForChangeUnits = true;
    buttonsWdg->setEnabled(false);
    testRespond->testWdg->setEnabled(false);
    stopService();
    testRespond->testWdg->stopTest();
}
void MainWindow::changeUnitsFinished()
{
    buttonsWdg->setEnabled(true);
    testRespond->testWdg->setEnabled(true);
    this->show();
    if ( !unhideAtStart ) changeVisibility();

    // for first service start; can be changed.
    if ( runAtStart ) {
        runAtStart = false;
        startService();
    };
}

void MainWindow::getDNSCryptProxyServiceVersion()
{
    QVariantMap args;
    args["action"] = "getVersion";
    Action act("pro.russianfedora.dnscryptclienttest.getversion");
    act.setHelperId("pro.russianfedora.dnscryptclienttest");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(getVersionJobFinished(KJob*)));
    job->start();
}
void MainWindow::getVersionJobFinished(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
        serviceVersion = job->data().value("version").toString();
    };
    //QTextStream s(stdout);
    //s<<"service version "<<serviceVersion<<endl;
    statusBar()->showMessage(
                QString("DNSCrypt-proxy service version %1").arg(serviceVersion));
}
