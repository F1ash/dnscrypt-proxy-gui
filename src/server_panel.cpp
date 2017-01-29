#include "server_panel.h"
#include "help_thread.h"
#include "server_info.h"
//#include <QTextStream>

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
    servItems = QList<QStandardItem*>();
    servList = new QComboBox(this);
    servList->setDuplicatesEnabled(false);
    servList->setContextMenuPolicy(Qt::NoContextMenu);
    servItemModel = new QStandardItemModel(this);
    servList->setModel(servItemModel);
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
    testRespond = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_test",
                                 QIcon(":/test.png")),
                "", this);
    testRespond->setFlat(true);
    testRespond->setToolTip("to Respond test");
    testRespond->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(servLabel, 1);
    baseLayout->addWidget(servList, 10);
    baseLayout->addWidget(servInfo, 1);
    baseLayout->addWidget(appSettings, 1);
    baseLayout->addWidget(testRespond, 1);
    setLayout(baseLayout);
    connect(appSettings, SIGNAL(released()),
            this, SIGNAL(toSettings()));
    connect(testRespond, SIGNAL(released()),
            this, SIGNAL(toTest()));
    connect(servList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(serverDataChanged(int)));
    connect(servList->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(changeItemState(QModelIndex, QModelIndex)));
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
