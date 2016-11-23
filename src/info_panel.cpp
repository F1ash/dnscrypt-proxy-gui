#include "info_panel.h"

InfoPanel::InfoPanel(QWidget *parent) :
    QStackedWidget(parent)
{
    setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    fullName = new QLabel(this);
    description = new QLabel(this);
    location = new QLabel(this);
    servLayout = new QVBoxLayout(this);
    servLayout->addWidget(fullName);
    servLayout->addWidget(description);
    servLayout->addWidget(location);
    servInfo = new QWidget(this);
    servInfo->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servInfo->setLayout(servLayout);

    attention = new QLabel(this);
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
        attention->setText("You may need to restart the network\nand web applications");
        break;
    case RESTORED:
        idx = 1;
        attention->setText("System DNS resolver settings restored.\
\nYou may need to restart the network\nand web applications");
        break;
    case ACTIVATING:
    case INACTIVE:
    case FAILED:
    case DEACTIVATING:
    case RELOADING:
    default:
        break;
    };
    setCurrentIndex(idx);
    if ( idx==1 ) {
        if ( timerId>0 ) killTimer(timerId);
        timerId = startTimer(10000);
        attention->setStyleSheet("QLabel {background-color: gold;}");
    };
}
void InfoPanel::setServerDescription(const QVariantMap &_data)
{
    fullName->setText(_data.value("FullName").toString());
    fullName->setToolTip(_data.value("FullName").toString());
    description->setText(_data.value("Description").toString());
    description->setToolTip(_data.value("Description").toString());
    location->setText(_data.value("Location").toString());
    location->setToolTip(_data.value("Location").toString());
    setCurrentIndex(0);
}

/* private slots */
void InfoPanel::timerEvent(QTimerEvent *ev)
{
    if ( timerId && ev->timerId()==timerId ) {
        killTimer(timerId);
        timerId = 0;
        attention->setStyleSheet("QLabel {background-color: white;}");
    };
    ev->accept();
}
