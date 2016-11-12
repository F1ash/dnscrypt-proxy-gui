#include "info_panel.h"

InfoPanel::InfoPanel(QWidget *parent) :
    QStackedWidget(parent)
{
    name = new QLabel(this);
    location = new QLabel(this);
    servLayout = new QVBoxLayout(this);
    servLayout->addWidget(name);
    servLayout->addWidget(location);
    servInfo = new QWidget(this);
    servInfo->setLayout(servLayout);

    attention = new QLabel(this);
    attention->setStyleSheet("QLabel {background-color: gold;}");
    attentLayout = new QVBoxLayout(this);
    attentLayout->addWidget(attention);
    attentions = new QWidget(this);
    attentions->setLayout(attentLayout);

    addWidget(servInfo);
    addWidget(attentions);
}

/* public slots */
void InfoPanel::changeAppState(SRV_STATUS state)
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
