#include "mainwindow.h"
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

    startFlag = true; // for first auto start
    srvStatus = INACTIVE;

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

    readSettings();

    connect(serverWdg, SIGNAL(toSettings()),
            this, SLOT(toSettings()));
    connect(appSettings, SIGNAL(toBase()),
            this, SLOT(toBase()));
}

void MainWindow::readSettings()
{
    restoreGeometry(settings.value("Geometry").toByteArray());
    bool runAtStart = settings.value("RunAtStart").toBool();
    appSettings->setRunAtStart(runAtStart);
    if ( runAtStart ) {
        startClientProcess();
    };
}
void MainWindow::setSettings()
{
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("RunAtStart", appSettings->getRunAtStart());
}

void MainWindow::toSettings()
{
    commonWdg->setCurrentWidget(appSettings);
}
void MainWindow::toBase()
{
    commonWdg->setCurrentWidget(baseWdg);
}
void MainWindow::initTrayIcon()
{
    trayIcon = new TrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    /*
    connect(trayIcon->startAction, SIGNAL(triggered(bool)),
            this, SLOT(startConnection()));
    connect(trayIcon->stopAction, SIGNAL(triggered(bool)),
            this, SLOT(stopConnection()));
     */
    connect(trayIcon->closeAction, SIGNAL(triggered(bool)),
            this, SLOT(close()));
    trayIcon->show();
}
void MainWindow::changeVisibility()
{
    ( this->isVisible() ) ? this->hide() : this->show();
}
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason r)
{
    if ( r==QSystemTrayIcon::Trigger ) changeVisibility();
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
        stopConnection();
        setSettings();
        trayIcon->hide();
        ev->accept();
    };
}
void MainWindow::startConnection()
{
    trayIcon->setIcon(
                QIcon::fromTheme("DNSCryptClient_reload",
                                 QIcon(":/reload.png")));
    startFlag = true;
    checkServiceStatus();
}
void MainWindow::stopConnection()
{
    startFlag = false;
    stopClientProcess();
}
void MainWindow::stopClientProcess()
{
    QVariantMap args;
    args["action"] = "stop";
    Action act("pro.russianfedora.dnscrypt_client.stop");
    act.setHelperId("pro.russianfedora.dnscrypt_client");
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
    };
}
void MainWindow::startClientProcess()
{
    connectToClientService();
    trayIcon->setIcon(
                QIcon::fromTheme("DNSCryptClient_reload",
                                 QIcon(":/reload.png")));
    QVariantMap args;
    Action act;
    switch (srvStatus) {
    //case INACTIVE:
    //    args["action"] = "create";
    //    act.setName("pro.russianfedora.dnscrypt_client.create");
    //    break;
    case INACTIVE:
    case FAILED:
        args["action"] = "start";
        act.setName("pro.russianfedora.dnscrypt_client.start");
        break;
    default:
        return;
    };
    act.setHelperId("pro.russianfedora.dnscrypt_client");
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
                   QString("Session open with exit code: %1\nMSG: %2\nERR: %3")
                   .arg(code).arg(msg).arg(err));
    } else {
        KNotification::event(
                   KNotification::Notification,
                   "DNSCryptClient",
                   QString("ERROR: %1\n%2")
                   .arg(job->error()).arg(job->errorText()));
        trayIcon->setIcon(
                    QIcon::fromTheme("DNSCryptClient_closed",
                                     QIcon(":/closed.png")));
    };
}
bool MainWindow::checkServiceStatus()
{
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
    } else {
        srvStatus = INACTIVE;
    };
    emit serviceStateChanged(srvStatus);
    changeAppState();
}
void MainWindow::changeAppState()
{
    switch ( srvStatus ) {
    case INACTIVE:
    case FAILED:
        if ( startFlag ) {
            trayIcon->setIcon(
                        QIcon::fromTheme("DNSCryptClient_reload",
                                         QIcon(":/reload.png")));
            startClientProcess();
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
        startFlag = false;
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
