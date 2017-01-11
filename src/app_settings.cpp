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
    findActiveService = new QCheckBox("find the active service automatically", this);
    useFastOnly = new QCheckBox("Use fast servers only", this);
    useFastOnly->setEnabled(false);
    restoreAtClose = new QCheckBox("restore DNS system resolver settings", this);
    appSetLayout = new QVBoxLayout(this);
    appSetLayout->addWidget(runAtStart);
    appSetLayout->addWidget(findActiveService);
    appSetLayout->addWidget(useFastOnly);
    appSetLayout->addWidget(restoreAtClose);
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
    connect(findActiveService, SIGNAL(toggled(bool)),
            this, SLOT(enableUseFastOnly(bool)));
    connect(useFastOnly, SIGNAL(toggled(bool)),
            this, SIGNAL(useFastOnlyStateChanged(bool)));
    connect(restoreAtClose, SIGNAL(toggled(bool)),
            this, SIGNAL(restoreAtCloseChanged(bool)));
}

bool AppSettings::getRunAtStartState() const
{
    return runAtStart->isChecked();
}
void AppSettings::setRunAtStartState(bool state)
{
    runAtStart->setChecked(state);
}
void AppSettings::setFindActiveServiceState(bool state)
{
    findActiveService->setChecked(state);
}
void AppSettings::setUseFastOnlyState(bool state)
{
    if ( findActiveService->isChecked() ) {
        useFastOnly->setChecked(state);
    };
}
void AppSettings::setRestoreAtClose(bool state)
{
    restoreAtClose->setChecked(state);
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
void AppSettings::enableUseFastOnly(bool state)
{
    useFastOnly->setChecked(false);
    useFastOnly->setEnabled(state);
}
