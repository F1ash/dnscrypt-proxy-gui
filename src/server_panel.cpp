#include "server_panel.h"
#include "help_thread.h"
#include "server_info.h"
//#include <QTextStream>

ServerPanel::ServerPanel(QWidget *parent, QString ver) :
    QWidget(parent), serviceVersion(ver)
{
    servLabel = new QLabel(this);
    servLabel->setPixmap(
                QIcon::fromTheme("DNSCryptClient_start",
                                 QIcon(":/start.png"))
            .pixmap(32));
    servLabel->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servLabel->setContentsMargins(0, 0, 0, 0);
    servItems = QList<QStandardItem*>();
    servList = new QComboBox(this);
    servList->setDuplicatesEnabled(false);
    servList->setContextMenuPolicy(Qt::NoContextMenu);
    servList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    servItemModel = new QStandardItemModel(this);
    servList->setModel(servItemModel);
    servList->setContentsMargins(0, 0, 0, 0);
    servInfo = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_info",
                                 QIcon(":/info.png")),
                "", this);
    servInfo->setFlat(false);
    servInfo->setToolTip("DNSCrypt Server Info");
    servInfo->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servInfo->setContentsMargins(0, 0, 0, 0);
    appSettings = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_settings",
                                 QIcon(":/settings.png")),
                "", this);
    appSettings->setFlat(false);
    appSettings->setToolTip("to Application Settings");
    appSettings->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    appSettings->setContentsMargins(0, 0, 0, 0);
    testRespond = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_test",
                                 QIcon(":/test.png")),
                "", this);
    testRespond->setFlat(false);
    testRespond->setToolTip("to Respond test");
    testRespond->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    testRespond->setContentsMargins(0, 0, 0, 0);
    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(servLabel, 1);
    baseLayout->addWidget(servList, 6);
    baseLayout->addWidget(servInfo, 1);
    baseLayout->addWidget(appSettings, 1);
    baseLayout->addWidget(testRespond, 1);

    setContentsMargins(0, 0, 0, 0);
    setLayout(baseLayout);

    connect(appSettings, SIGNAL(released()),
            this, SIGNAL(toSettings()));
    connect(testRespond, SIGNAL(released()),
            this, SIGNAL(toTest()));
    connect(servList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(serverDataChanged(int)));
    connect(servList, SIGNAL(activated(int)),
            this, SIGNAL(serverActivated()));
    connect(servList->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(changeItemState(QModelIndex, QModelIndex)));
    connect(servInfo, SIGNAL(released()),
            this, SLOT(showServerInfo()));
}

void ServerPanel::setServerDataMap(const QVariantMap _map)
{
    listOfServers = _map;
}
void ServerPanel::setLastServer(const QString &_server)
{
    lastServer = _server;
    HelpThread *hlpThread = new HelpThread(this);
    if ( serviceVersion.compare("2")>0 ) {
        hlpThread->setServerDataMap(listOfServers);
    };
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
QString ServerPanel::getCurrentRespondIconName() const
{
    return servList->currentData().toMap()
            .value("Respond").toString();
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
QString ServerPanel::getItemName(int idx) const
{
    return servList->itemText(idx);
}
QString ServerPanel::getRespondIconName(int idx) const
{
    return servList->itemData(idx).toMap()
            .value("Respond", "none").toString();
}
bool ServerPanel::getItemState(int idx) const
{
    return servList->itemData(idx).toMap()
            .value("Enable", true).toBool();
}
bool ServerPanel::serverIsEnabled() const
{
    return getItemState(servList->currentIndex());
}
void ServerPanel::changeServerInfo()
{
    QVariantMap servData = servList->currentData().toMap();
    emit serverData(servData);
}

/* public slots */
void ServerPanel::changeAppState(SRV_STATUS state)
{
    switch (state) {
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
    case PROCESSING:
    default:
        servList->setEnabled(false);
        break;
    }
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

/* private slots */
void ServerPanel::resizeEvent(QResizeEvent *ev)
{
    ev->accept();
    // (widget+combobox+shadow+itemborder\buttonborder)*2
    // (1+1+1+3)*2 = 12
    int h = ev->size().height();
    QSize s = QSize(h-12, h-12);
    servLabel->setMaximumHeight(h);
    //servList->setIconSize(s1);
    servList->setMaximumHeight(h);
    servInfo->setIconSize(s);
    servInfo->setMaximumHeight(h);
    appSettings->setIconSize(s);
    appSettings->setMaximumHeight(h);
    testRespond->setIconSize(s);
    testRespond->setMaximumHeight(h);
}
void ServerPanel::serverDataChanged(int idx)
{
    Q_UNUSED(idx)
    //QVariantMap servData = servList->currentData().toMap();
    //emit serverData(servData);
    changeServerInfo();
}
void ServerPanel::addServer(const QVariantMap &_data)
{
    QStandardItem *_item = new QStandardItem;
    _item->setData(_data.value("Name").toString(), Qt::DisplayRole);
    bool state = _data.value("Enable").toBool();
    if ( state ) {
        _item->setFlags(Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled |
                        Qt::ItemIsSelectable);
    } else {
        _item->setFlags(Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled);
    };
    _item->setData(
                (state)? Qt::Checked : Qt::Unchecked,
                Qt::CheckStateRole);
    QString respondIconName = _data.value("Respond", "none").toString();
    _item->setData(
                QIcon::fromTheme(respondIconName,
                                 QIcon(QString(":/%1.png").arg(respondIconName))),
                Qt::DecorationRole);
    _item->setData("Press 'Blank' key to change state", Qt::ToolTipRole);
    _item->setData(_data, Qt::UserRole);
    servItemModel->insertRow(servItems.count(), _item);
    servItems.append(_item);
    emit checkItem(_data.value("Name").toString(), respondIconName);
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
void ServerPanel::changeItemState(QModelIndex topLeft, QModelIndex bottomRight)
{
    Q_UNUSED(bottomRight);
    QStandardItem* _item = servItems.at(topLeft.row());
    QVariantMap _map = _item->data(Qt::UserRole).toMap();
    bool state = (_item->checkState() == Qt::Checked);
    _map.insert("Enable", state);
    if ( state ) {
        _item->setFlags(Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled |
                        Qt::ItemIsSelectable);
    } else {
        _item->setFlags(Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled);
    };
    _item->setData(_map, Qt::UserRole);
    //QTextStream s(stdout);
    //s<< _item->text()<<" " << _item->checkState() << endl;
}
