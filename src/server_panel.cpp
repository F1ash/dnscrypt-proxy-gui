#include "server_panel.h"

ServerPanel::ServerPanel(QWidget *parent) :
    QWidget(parent)
{
    servLabel = new QLabel(this);
    servLabel->setPixmap(QIcon::fromTheme(
                             "DNSCryptClient_start",
                             QIcon(":/start.png"))
            .pixmap(32));
    servLabel->setMinimumSize(32, 32);
    servList = new QComboBox(this);
    servList->setDuplicatesEnabled(false);
    servInfo = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_info.png",
                                 QIcon(":/info.png")),
                "", this);
    servInfo->setFlat(true);
    servInfo->setToolTip("DNSCrypt Server Info");
    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(servLabel, 1);
    baseLayout->addWidget(servList, 10);
    baseLayout->addWidget(servInfo, 1);
    setLayout(baseLayout);
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
