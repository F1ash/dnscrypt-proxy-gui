#include "app_settings.h"

AppSettings::AppSettings(QWidget *parent) :
    QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setLabel = new QLabel(this);
    setLabel->setPixmap(QIcon::fromTheme(
                            "DNSCryptClient_settings",
                            QIcon(":/settings.png"))
                        .pixmap(32));
    setLabel->setSizePolicy(
               QSizePolicy(
                   QSizePolicy::Ignored,
                   QSizePolicy::Ignored));
    setLabel->setContentsMargins(0, 0, 0, 0);
    nameLabel = new QLabel(this);
    nameLabel->setText("Application Settings");
    nameLabel->setSizePolicy(
               QSizePolicy(
                   QSizePolicy::Ignored,
                   QSizePolicy::Ignored));
    nameLabel->setContentsMargins(0, 0, 0, 0);
    baseButton = new QPushButton(
                QIcon::fromTheme("DNSCryptClient_start",
                                 QIcon(":/start.png")),
                "", this);
    baseButton->setFlat(true);
    baseButton->setToolTip("to Control Panel");
    baseButton->setSizePolicy(
                QSizePolicy(
                    QSizePolicy::Ignored,
                    QSizePolicy::Ignored));
    baseButton->setContentsMargins(0, 0, 0, 0);
    headLayout = new QHBoxLayout(this);
    headLayout->addWidget(setLabel, 1);
    headLayout->addWidget(nameLabel, 10, Qt::AlignCenter);
    headLayout->addWidget(baseButton, 1);
    headWdg = new QWidget(this);
    headWdg->setContentsMargins(0, 0, 0, 0);
    headWdg->setLayout(headLayout);

    runAtStart = new QCheckBox("run service at start", this);
    findActiveService = new QCheckBox("find the active service", this);
    appSetLayout = new QVBoxLayout(this);
    appSetLayout->addWidget(runAtStart);
    appSetLayout->addWidget(findActiveService);
    appSettings = new QWidget(this);
    appSettings->setContentsMargins(0, 0, 0, 0);
    appSettings->setLayout(appSetLayout);

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(headWdg, 1);
    commonLayout->addWidget(appSettings, 5);
    commonLayout->addStretch(-1);
    setLayout(commonLayout);

    connect(baseButton, SIGNAL(released()),
            this, SIGNAL(toBase()));
    connect(findActiveService, SIGNAL(toggled(bool)),
            this, SIGNAL(findActiveServiceStateChanged(bool)));
    connect(runAtStart, SIGNAL(toggled(bool)),
            this, SIGNAL(runAtStartStateChanged(bool)));
}

void AppSettings::setRunAtStartState(bool state)
{
    runAtStart->setChecked(state);
}
void AppSettings::setFindActiveServiceState(bool state)
{
    findActiveService->setChecked(state);
}

/* private slots */
void AppSettings::resizeEvent(QResizeEvent *ev)
{
    /*
    int h = ev->size().height()-12;
    QSize s = QSize(h, h);
    setLabel->setFixedHeight(h);
    nameLabel->setFixedHeight(h);
    baseButton->setFixedHeight(h);
    baseButton->setIconSize(s);
    */
    ev->accept();
}
