#include "info_panel.h"
//#include <QTextStream>

InfoPanel::InfoPanel(QWidget *parent) :
    QStackedWidget(parent)
{
    setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    st = this->styleSheet();
    fullName = new QLabel(this);
    description = new QLabel(this);
    location = new QLabel(this);
    respond = new QLabel(this);
    srvState = new QLabel(this);
    servLayout = new QVBoxLayout();
    servLayout->addWidget(fullName);
    servLayout->addWidget(description);
    servLayout->addWidget(location);
    servLayout->addWidget(respond);
    servLayout->addWidget(srvState);
    servInfo = new QWidget(this);
    servInfo->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    servInfo->setContentsMargins(0, 0, 0, 0);
    servInfo->setLayout(servLayout);

    attention = new QLabel(this);
    attentLayout = new QVBoxLayout();
    attentLayout->addWidget(attention);
    attentions = new QWidget(this);
    attentions->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    attentions->setContentsMargins(0, 0, 0, 0);
    attentions->setLayout(attentLayout);

    addWidget(servInfo);
    addWidget(attentions);
    setContentsMargins(0, 0, 0, 0);
}

/* public slots */
void InfoPanel::changeAppState(SRV_STATUS state)
{
    //QTextStream s(stdout);
    int idx = 0;
    switch (state) {
    case PROCESSING:
        //s<< "PROCESSING" <<endl;
        idx = 1;
        attention->setStyleSheet("QLabel {background-color: gold;}");
        attention->setText("Processing...");
        break;
    case READY:
        //s<< "READY" <<endl;
        break;
    case ACTIVE:
        //s<< "ACTIVE" <<endl;
        idx = 1;
        attention->setText("You may need to restart the network\nand web applications");
        break;
    case RESTORED:
        //s<< "RESTORED" <<endl;
        idx = 1;
        attention->setText("System DNS resolver settings restored.\
\nYou may need to restart the network\nand web applications");
        break;
    case ACTIVATING:
        //s<< "ACTIVATING" <<endl;
        break;
    case INACTIVE:
        //s<< "INACTIVE" <<endl;
        break;
    case FAILED:
        //s<< "FAILED" <<endl;
        break;
    case DEACTIVATING:
        //s<< "DEACTIVATING" <<endl;
        break;
    case RELOADING:
        //s<< "RELOADING" <<endl;
        break;
    default:
        //s<< "default" <<endl;
        break;
    };
    setCurrentIndex(idx);
    if ( state !=PROCESSING && idx==1 ) {
        if ( timerId>0 ) killTimer(timerId);
        timerId = startTimer(10000);
        attention->setStyleSheet("QLabel {background-color: gold;}");
    };
    //s << "idx=" << idx << " state="<< state<<endl;
}
void InfoPanel::setServerDescription(const QVariantMap &_data)
{
    fullName->setText(_data.value("FullName").toString());
    fullName->setToolTip(_data.value("FullName").toString());
    description->setText(_data.value("Description").toString());
    description->setToolTip(_data.value("Description").toString());
    location->setText(_data.value("Location").toString());
    location->setToolTip(_data.value("Location").toString());
    respond->setText(QString("Current respond from server: %1")
                        .arg(_data.value("Respond").toString()));
    respond->setToolTip(_data.value("Respond").toString());
    srvState->setText(QString("Server is used: %1")
                        .arg(_data.value("Enable").toBool()? "yes":"no"));
    srvState->setToolTip(_data.value("Enable").toBool()? "yes":"no");
    setCurrentIndex(0);
}

/* private slots */
void InfoPanel::timerEvent(QTimerEvent *ev)
{
    if ( timerId && ev->timerId()==timerId ) {
        killTimer(timerId);
        timerId = 0;
        attention->setStyleSheet(st);
        setCurrentIndex(0);
    };
    ev->accept();
}
