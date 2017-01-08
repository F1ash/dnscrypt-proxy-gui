#include "server_panel.h"
#include "help_thread.h"
#include "server_info.h"

ServerPanel::ServerPanel(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    servLabel = new QLabel(this);
    servLabel->setPixmap(
                QIcon::fromTheme("DNSCryptClient_start",
                                 QIcon(":/start.png"))
            .pixmap(32));
    servLabel->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servList = new QComboBox(this);
    servList->setDuplicatesEnabled(false);
    servInfo = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_info",
                                 QIcon(":/info.png")),
                "", this);
    servInfo->setFlat(true);
    servInfo->setToolTip("DNSCrypt Server Info");
    servInfo->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    appSettings = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_settings",
                                 QIcon(":/settings.png")),
                "", this);
    appSettings->setFlat(true);
    appSettings->setToolTip("to Application Settings");
    appSettings->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(servLabel, 1);
    baseLayout->addWidget(servList, 10);
    baseLayout->addWidget(servInfo, 1);
    baseLayout->addWidget(appSettings, 1);
    setLayout(baseLayout);
    connect(appSettings, SIGNAL(released()),
            this, SIGNAL(toSettings()));
    connect(servList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(serverDataChanged(int)));
    connect(servInfo, SIGNAL(released()),
            this, SLOT(showServerInfo()));
}

void ServerPanel::setLastServer(const QString &_server)
{
    lastServer = _server;
    HelpThread *hlpThread = new HelpThread(this);
    connect(hlpThread, SIGNAL(newDNSCryptSever(const QVariantMap&)),
            this, SLOT(addServer(const QVariantMap&)));
    connect(hlpThread, SIGNAL(finished()),
            this, SLOT(findLastServer()));
    hlpThread->start();
}
QString ServerPanel::getCurrentServer() const
{
    return servList->currentText();
}
int ServerPanel::getServerListCount() const
{
    return servList->count();
}
void ServerPanel::setNextServer()
{
    int idx = servList->currentIndex() + 1;
    if ( idx==servList->count() ) idx = 0;
    servList->setCurrentIndex(idx);
}
void ServerPanel::setItemIcon(QString name, QString icon_name)
{
    int idx = servList->findText(name, Qt::MatchExactly);
    if ( idx>-1 ) {
        servList->setItemIcon(
                    idx,
                    QIcon::fromTheme(icon_name,
                    QIcon(QString(":/%1.png").arg(icon_name))));
        QVariantMap _map = servList->itemData(idx).toMap();
        _map.insert("Respond", icon_name);
        servList->setItemData(idx, _map);
    };
}
QString ServerPanel::getItemName(int idx) const
{
    return servList->itemText(idx);
}
QString ServerPanel::getRespondIconName(int idx) const
{
    return servList->itemData(idx).toMap()
            .value("Respond").toString();
}

/* public slots */
void ServerPanel::changeAppState(SRV_STATUS state)
{
    switch (state) {
    case PROCESSING:
    case READY:
        break;
    case INACTIVE:
    case RESTORED:
        servList->setEnabled(true);
        break;
    case ACTIVE:
    case ACTIVATING:
    case FAILED:
    case DEACTIVATING:
    case RELOADING:
    default:
        servList->setEnabled(false);
        break;
    }
}

/* private slots */
void ServerPanel::resizeEvent(QResizeEvent *ev)
{
    int h = ev->size().height()-12;
    QSize s = QSize(h, h);
    servLabel->setFixedHeight(h);
    servList->setFixedHeight(h);
    servList->setIconSize(s);
    servInfo->setFixedHeight(h);
    servInfo->setIconSize(s);
    appSettings->setFixedHeight(h);
    appSettings->setIconSize(s);
    ev->accept();
}
void ServerPanel::serverDataChanged(int idx)
{
    Q_UNUSED(idx)
    QVariantMap servData = servList->currentData().toMap();
    emit serverData(servData);
}
void ServerPanel::addServer(const QVariantMap &_data)
{
    servList->addItem(
                QIcon::fromTheme("none"),
                _data.value("Name").toString(),
                _data);
}
void ServerPanel::findLastServer()
{
    servList->setCurrentText(lastServer);
    QVariantMap _data = servList->currentData().toMap();
    emit serverData(_data);
    emit readyForStart();
}
void ServerPanel::showServerInfo()
{
    QVariantMap _map = servList->currentData().toMap();
    ServerInfo *d = new ServerInfo(this);
    d->setServerData(_map);
    d->exec();
    d->deleteLater();
}
