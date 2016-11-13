#include "server_panel.h"

ServerPanel::ServerPanel(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    servLabel = new QLabel(this);
    servLabel->setPixmap(QIcon::fromTheme(
                             "DNSCryptClient_start",
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
}

/* public slots */
void ServerPanel::changeAppState(SRV_STATUS state)
{
    switch (state) {
    case INACTIVE:

        break;
    case ACTIVE:

        break;
    case ACTIVATING:

        break;
    case FAILED:

        break;
    case DEACTIVATING:

        break;
    case RELOADING:

        break;
    default:
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
