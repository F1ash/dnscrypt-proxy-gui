#include "button_panel.h"

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
    start->setToolTip("Start proxing");
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
    stop->setToolTip("Stop proxing");
    stop->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    restore = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_restore",
                                 QIcon(":/restore.png")),
                "", this);
    restore->setFlat(true);
    restore->setContentsMargins(0, 0, 0, 0);
    restore->setToolTip("Stop proxing\nRestore system DNS resolver settings");
    restore->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));

    baseLayout = new QHBoxLayout(this);
    baseLayout->addWidget(start);
    baseLayout->addWidget(stop);
    baseLayout->addWidget(restore);
    setLayout(baseLayout);
}

/* public slots */
void ButtonPanel::changeAppState(SRV_STATUS state)
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

