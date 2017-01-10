#include "button_panel.h"
//#include <QTextStream>

ButtonPanel::ButtonPanel(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    start = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_open",
                                 QIcon(":/open.png")),
                "", this);
    start->setFlat(true);
    start->setContentsMargins(0, 0, 0, 0);
    start->setToolTip("Start proxying");
    start->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    stop = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_close",
                                 QIcon(":/close.png")),
                "", this);
    stop->setFlat(true);
    stop->setContentsMargins(0, 0, 0, 0);
    stop->setToolTip("Stop proxying");
    stop->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    stop->setEnabled(false);
    restore = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_restore",
                                 QIcon(":/restore.png")),
                "", this);
    restore->setFlat(true);
    restore->setContentsMargins(0, 0, 0, 0);
    restore->setToolTip("Stop proxying\nRestore system DNS resolver settings");
    restore->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
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
        setEnabled(true);
        break;
    case PROCESSING:
        setEnabled(false);
        break;
    case ACTIVE:
    case ACTIVATING:
    case DEACTIVATING:
    case RELOADING:
        start->setDisabled(true);
        stop->setEnabled(true);
        restore->setEnabled(true);
        break;
    case RESTORED:
        start->setEnabled(true);
        stop->setDisabled(true);
        restore->setDisabled(true);
        break;
    case STOP_SLICE:
        start->setDisabled(true);
        stop->setDisabled(true);
        restore->setDisabled(true);
        break;
    case FAILED:
    case INACTIVE:
    default:
        start->setEnabled(true);
        stop->setDisabled(true);
        restore->setEnabled(true);
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

