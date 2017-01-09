#include "test_widget.h"

TestWidget::TestWidget(QWidget *parent) :
    QWidget(parent)
{
    processing = false;
    info = new QLabel(this);
    progress = new QProgressBar(this);
    start = new QPushButton("Start", this);
    stop = new QPushButton("Stop", this);
    buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(start, 0, Qt::AlignRight);
    buttonLayout->addWidget(stop, 0, Qt::AlignRight);
    buttons = new QWidget(this);
    buttons->setLayout(buttonLayout);

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(info);
    commonLayout->addWidget(progress);
    commonLayout->addWidget(buttons);
    setLayout(commonLayout);

    connect(start, SIGNAL(released()),
            this, SLOT(testStarted()));
    connect(stop, SIGNAL(released()),
            this, SLOT(testFinished()));
}

/* private slots */
void TestWidget::testStarted()
{
    processing = true;
}
void TestWidget::testFinished()
{
    processing = false;
}

/* public slots */
void TestWidget::changeAppState(SRV_STATUS state)
{
    switch (state) {
    case READY:
        start->setEnabled(true);
        break;
    case PROCESSING:
        start->setEnabled(false);
        break;
    case ACTIVE:
        buttons->setDisabled(true);
        info->setText("To use necessary to stop service.");
        break;
    case INACTIVE:
    case FAILED:
        buttons->setEnabled(true);
        info->setText("For get correct result\n\
need to stop all internet activity.");
        break;
    case ACTIVATING:
    case DEACTIVATING:
    case RELOADING:
    case RESTORED:
    case STOP_SLICE:
    default:
        break;
    };
}
