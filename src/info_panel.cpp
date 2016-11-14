#include "info_panel.h"

InfoPanel::InfoPanel(QWidget *parent) :
    QStackedWidget(parent)
{
    setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    fullName = new QLabel(this);
    location = new QLabel(this);
    servLayout = new QVBoxLayout(this);
    servLayout->addWidget(fullName);
    servLayout->addWidget(location);
    servInfo = new QWidget(this);
    servInfo->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servInfo->setLayout(servLayout);

    attention = new QLabel(this);
    attention->setStyleSheet("QLabel {background-color: gold;}");
    attentLayout = new QVBoxLayout(this);
    attentLayout->addWidget(attention);
    attentions = new QWidget(this);
    attentions->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    attentions->setLayout(attentLayout);

    addWidget(servInfo);
    addWidget(attentions);
}

/* public slots */
void InfoPanel::changeAppState(SRV_STATUS state)
{
    int idx = 0;
    switch (state) {
    case ACTIVE:
        idx = 1;
        attention->setText("You may need to restart the network and web applications");
        break;
    case RESTORED:
        idx = 1;
        attention->setText("System DNS resolver settings restoring");
        break;
    case ACTIVATING:
    case INACTIVE:
    case FAILED:
    case DEACTIVATING:
    case RELOADING:
    default:
        idx = 0;
        break;
    };
    setCurrentIndex(idx);
}
void InfoPanel::setServerDescription(const QVariantMap &_data)
{
    fullName->setText(_data.value("FullName").toString());
    location->setText(_data.value("Location").toString());
}
