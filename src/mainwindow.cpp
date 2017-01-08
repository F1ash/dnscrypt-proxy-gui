#include "mainwindow.h"
#include "resolver_entries.h"
#include <private/qdbusutil_p.h>
#include <QRegExp>

QString getRespondIconName(qreal r)
{
    if        ( 0<r && r<=0.3 ) {
        return "fast";
    } else if ( 0.3<r && r<=0.7 ) {
        return "middle";
    } else if ( 0.7<r && r<10.0 ) {
        return "slow";
    };
    return "none";
}

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
    restoreFlag = false;
    restoreAtClose = false;
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
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            serverWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            buttonsWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            infoWdg, SLOT(changeAppState(SRV_STATUS)));
    connect(this, SIGNAL(serviceStateChanged(SRV_STATUS)),
            this, SLOT(changeAppState(SRV_STATUS)));
    connect(serverWdg, SIGNAL(readyForStart()),
            this, SLOT(firstServiceStart()));
    connect(this, SIGNAL(nextServer()),
            this, SLOT(probeNextServer()));

    readSettings();
}

void MainWindow::readSettings()
{
    restoreGeometry(settings.value("Geometry").toByteArray());
    runAtStart = settings.value("RunAtStart", false).toBool();
    appSettings->setRunAtStartState(runAtStart);
    findActiveService = settings.value("FindActiveService", true).toBool();
    appSettings->setFindActiveServiceState(findActiveService);
    restoreAtClose = settings.value("RestoreAtClose", true).toBool();
    appSettings->setRestoreAtClose(restoreAtClose);
    QString lastServer = settings.value("LastServer").toString();
    serverWdg->setLastServer(lastServer);
    settings.beginGroup("Entries");
    foreach ( QString _key, settings.allKeys() ) {
        resolverEntries.append(settings.value(_key).toString());
    };
    settings.endGroup();
}
void MainWindow::setSettings()
{
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("RunAtStart", appSettings->getRunAtStartState());
    settings.setValue("FindActiveService", findActiveService);
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
    int ret = -1;
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
        return -2;
    } else if ( status=="active" ) {
        // if active then check task count
        _args.clear();
        _args<<"org.freedesktop.systemd1.Slice"<<"TasksCurrent";
        msg.setArguments(_args);
        answer = connection.call(msg);
        int currTasks;
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
                ret = -2;
            };
            break;
        case QDBusMessage::ErrorMessage:
        default:
            ret = -2;
            break;
        };
    };
    //s << "ret "<<ret << endl;
    return ret;
}
void MainWindow::startServiceProcess()
{
    QVariantMap args;
    args["action"] = "start";
    args["server"] = serverWdg->getCurrentServer();
    Action act("pro.russianfedora.dnscryptclient.start");
    act.setHelperId("pro.russianfedora.dnscryptclient");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setAutoDelete(true);
    if ( job->exec() ) {
        QString code        = job->data().value("code").toString();
        //QString msg         = job->data().value("msg").toString();
        //QString err         = job->data().value("err").toString();
        QString entry       = job->data().value("entry").toString();
        QString answ        = job->data().value("answ").toString();
        QString resp_time   = job->data().value("time").toString();
        QTextStream s(stdout);
        s << "dns entries  : " << answ << endl;
        s << "response time: " << qreal(resp_time.toULong())/1000000 << endl;
        //KNotification::event(
        //           KNotification::Notification,
        //           "DNSCryptClient",
        //           QString("Session open with exit code: %1\nMSG: %2\nERR: %3")
        //           .arg(code).arg(msg).arg(err));
        if ( code.toInt()==0 ) {
            addServerEnrty(entry);
            if ( answ.toInt()<1 ) {
                serverWdg->setItemIcon(
                            serverWdg->getCurrentServer(), "none");
                emit serviceStateChanged(STOP_SLICE);
                return;
            };
            serverWdg->setItemIcon(
                        serverWdg->getCurrentServer(),
                        getRespondIconName(qreal(resp_time.toULong())/1000000));
        } else {
            serverWdg->setItemIcon(
                        serverWdg->getCurrentServer(), "none");
        };
    } else {
        QTextStream s(stdout);
        s << "action failed" << endl;
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
    switch (checkSliceStatus()) {
    case  1:
        emit serviceStateChanged(ACTIVE);
        break;
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case -1:
    case -2:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
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
                   QString("Stop Service ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_close",
                                     QIcon(":/close.png")));
    };
    switch (checkSliceStatus()) {
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:
    case -1:
    case -2:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
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
    job->setAutoDelete(true);
    if (job->exec()) {
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
    case  0:
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:
    case -1:
    case -2:
    default:
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::restoreSettingsProcess()
{
    restoreFlag = false;
    QString selectedEntry = showResolverEntries();
    if ( selectedEntry.isEmpty() ) {
        emit serviceStateChanged(READY);
        return;
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
                   .arg(code).arg(msg).arg((err.isEmpty())? "None" : err));
        if ( code.toInt()!=-1 ) srvStatus = RESTORED;
    } else {
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
    };
    emit serviceStateChanged(srvStatus);
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
}
void MainWindow::toBase()
{
    commonWdg->setCurrentWidget(baseWdg);
}
void MainWindow::firstServiceStart()
{
    settings.beginGroup("Responds");
    foreach ( QString _key, settings.allKeys() ) {
        QString respondIconName =
                settings.value(_key).toString();
        if ( respondIconName.isEmpty() ) respondIconName.append("none");
        serverWdg->setItemIcon( _key, respondIconName );
    };
    settings.endGroup();
    if ( runAtStart ) {
        runAtStart = false;
        startService();
    };
}
void MainWindow::changeFindActiveServiceState(bool state)
{
    findActiveService = state;
}
void MainWindow::changeRestoreAtCloseState(bool state)
{
    restoreAtClose = state;
}
void MainWindow::startService()
{
    probeCount = 0;
    stopManually = false;
    switch (checkSliceStatus()) {
    case  0:     // ready for start
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:    // incorrectly for start;
    case -2:    // errored answer
    case -1:
    default:    //  need to restart the slice and proxying
        emit serviceStateChanged(STOP_SLICE);
        break;
    };
}
void MainWindow::stopService()
{
    probeCount = 0;
    stopManually = true;
    switch (checkSliceStatus()) {
    case  0:    // stop slice or service unnecessary
        emit serviceStateChanged(INACTIVE);
        break;
    case  1:    // need to stop
    case -2:    // errored answer
    case -1:
    default:    // need to restart the slice and proxying
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
    if ( ev->type()==QEvent::Close ) {
        if ( restoreAtClose && srvStatus!=RESTORED ) {
            show();
            stopSliceProcess();
            restoreSettingsProcess();
        } else {
            stopSliceProcess();
        };
        setSettings();
        trayIcon->hide();
    };
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
    if ( status!=READY && status!=PROCESSING ) srvStatus = status;
    //QTextStream s(stdout);
    switch ( status ) {
    case INACTIVE:
    case FAILED:
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("DNSCryptClient service is inactive."));
        //s << "INACTIVE/FAILED" << endl;
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_closed",
                                     QIcon(":/closed.png")));
        trayIcon->setToolTip(QString("%1\n%2")
                             .arg(windowTitle())
                             .arg("--stopped--"));
        if ( !stopManually && findActiveService ) {
            //setStyleSheet("QWidget {background-color: red;}");
            emit serviceStateChanged(PROCESSING);
            findActiveServiceProcess();
        } else if ( restoreFlag ) {
            //setStyleSheet("QWidget {background-color: red;}");
            emit serviceStateChanged(PROCESSING);
            restoreSettingsProcess();
        } else {
            //setStyleSheet("QWidget {background-color: green;}");
            emit serviceStateChanged(READY);
        };
        break;
    case ACTIVE:
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("DNSCryptClient service is active."));
        connectToClientService();
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_opened",
                                     QIcon(":/opened.png")));
        trayIcon->setToolTip(QString("%1\n%2")
                             .arg(windowTitle())
                             .arg(serverWdg->getCurrentServer()));
        //s << "ACTIVE" << endl;
        //setStyleSheet("QWidget {background-color: green;}");
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
        //setStyleSheet("QWidget {background-color: red;}");
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
        //setStyleSheet("QWidget {background-color: green;}");
        emit serviceStateChanged(READY);
    default:
        break;
    };
}
void MainWindow::probeNextServer()
{
    startServiceProcess();
}
