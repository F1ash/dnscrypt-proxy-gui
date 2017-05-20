#include "button_panel.h"
//#include <QTextStream>

ButtonPanel::ButtonPanel(QWidget *parent) :
    QWidget(parent)
{
    start = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_open",
                                 QIcon(":/open.png")),
                "", this);
    start->setFlat(false);
    start->setToolTip("Start proxying");
    start->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    start->setContentsMargins(0, 0, 0, 0);
    stop = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_close",
                                 QIcon(":/close.png")),
                "", this);
    stop->setFlat(false);
    stop->setToolTip("Stop proxying");
    stop->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    stop->setContentsMargins(0, 0, 0, 0);
    stop->setEnabled(false);
    restore = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_restore",
                                 QIcon(":/restore.png")),
                "", this);
    restore->setFlat(false);
    restore->setToolTip("Stop proxying\nRestore system DNS resolver settings");
    restore->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    restore->setContentsMargins(0, 0, 0, 0);
    restore->setEnabled(false);

    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(start);
    baseLayout->addWidget(stop);
    baseLayout->addWidget(restore);
    setLayout(baseLayout);

    connect(start, SIGNAL(released()),
            this, SIGNAL(startProxing()));
    connect(stop, SIGNAL(released()),
            this, SIGNAL(stopProxing()));
    connect(restore, SIGNAL(released()),
            this, SIGNAL(restoreSettings()));
}

/* public slots */
void ButtonPanel::changeAppState(SRV_STATUS state)
{
    //QTextStream s(stdout);
    switch (state) {
    case READY:
        //s<< 0 <<endl;
        break;
    case PROCESSING:
    case ACTIVE:
    case ACTIVATING:
    case DEACTIVATING:
    case RELOADING:
        start->setDisabled(true);
        stop->setEnabled(true);
        restore->setEnabled(true);
        //s<< 1 <<endl;
        break;
    case RESTORED:
        start->setEnabled(true);
        stop->setDisabled(true);
        restore->setDisabled(true);
        //s<< 2 <<endl;
        break;
    case STOP_SLICE:
        start->setDisabled(true);
        stop->setDisabled(true);
        restore->setDisabled(true);
        //s<< 3 <<endl;
        break;
    case FAILED:
    case INACTIVE:
    default:
        start->setEnabled(true);
        stop->setDisabled(true);
        restore->setEnabled(true);
        //s<< 4 <<endl;
        break;
    };
}

/* private slots */
void ButtonPanel::resizeEvent(QResizeEvent *ev)
{
    int h = ev->size().height()-10;
    int w = ev->size().width()/3-9;
    int m = (h<w)? h : w;
    QSize s = QSize(m, m);
    start->setFixedSize(s);
    start->setIconSize(s);
    stop->setFixedSize(s);
    stop->setIconSize(s);
    restore->setFixedSize(s);
    restore->setIconSize(s);
    ev->accept();
}

