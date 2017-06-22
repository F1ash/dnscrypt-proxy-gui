#include "app_settings.h"
//#include <QTextStream>

AppSettings::AppSettings(QWidget *parent) :
    QWidget(parent)
{
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
    baseButton->setFlat(false);
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

    advancedLabel = new QLabel(this);
    advancedLabel->setText("<font color='red'>Advanced settings</font>");

    runAtStart = new QCheckBox(
                "run service at start",
                this);
    findActiveService = new QCheckBox(
                "find the active service automatically",
                this);
    useFastOnly = new QCheckBox(
                "Use fast servers only",
                this);
    useFastOnly->setEnabled(false);
    restoreAtClose = new QCheckBox(
                "restore DNS system resolver settings at close",
                this);

    asUser = new QCheckBox("Use as user", this);
    asUserLine = new QLineEdit(this);
    asUserLine->setEnabled(false);
    asUserLine->setPlaceholderText("dnscrypt");
    asUserLayout = new QHBoxLayout(this);
    asUserLayout->addWidget(asUser);
    asUserLayout->addWidget(asUserLine);
    asUserWdg = new QWidget(this);
    asUserWdg->setLayout(asUserLayout);

    jobPort = new PortSettings(this, JOB_PORT);
    jobPort->setName("port for receive DNS");
    testPort = new PortSettings(this, TEST_PORT);
    testPort->setName("port for testing server's responds");

    restoreDefaultBtn = new QPushButton(
                "Restore default",
                this);
    applyNewPortsBtn  = new QPushButton(
                "Apply new ports",
                this);
    applyNewPortsBtn->setEnabled(false);
    advancedButtonsLayout = new QHBoxLayout(this);
    advancedButtonsLayout->addWidget(restoreDefaultBtn);
    advancedButtonsLayout->addWidget(applyNewPortsBtn);
    advancedButtons = new QWidget(this);
    advancedButtons->setLayout(advancedButtonsLayout);

    appSettingsLayout = new QVBoxLayout(this);
    appSettingsLayout->addWidget(runAtStart);
    appSettingsLayout->addWidget(findActiveService);
    appSettingsLayout->addWidget(useFastOnly);
    appSettingsLayout->addWidget(restoreAtClose);
    appSettingsLayout->addWidget(advancedLabel, 0, Qt::AlignCenter);
    appSettingsLayout->addWidget(asUserWdg, 0, Qt::AlignLeft);
    appSettingsLayout->addWidget(jobPort, 0, Qt::AlignLeft);
    appSettingsLayout->addWidget(testPort, 0, Qt::AlignLeft);
    appSettingsLayout->addWidget(advancedButtons, 0, Qt::AlignCenter);
    appSettingsLayout->addStretch(-1);

    appSettings = new QWidget(this);
    appSettings->setContentsMargins(0, 0, 0, 0);
    appSettings->setLayout(appSettingsLayout);

    scrolled = new QScrollArea(this);
    scrolled->setWidget(appSettings);

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(headWdg, 1);
    commonLayout->addWidget(scrolled, 5);
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
    //connect(jobPort, SIGNAL(valueChanged(int)),
    //        this, SIGNAL(jobPortChanged(int)));
    //connect(testPort, SIGNAL(valueChanged(int)),
    //        this, SIGNAL(testPortChanged(int)));
    connect(jobPort, SIGNAL(valueChanged(int)),
            this, SLOT(unitChanged()));
    connect(testPort, SIGNAL(valueChanged(int)),
            this, SLOT(unitChanged()));
    connect(restoreDefaultBtn, SIGNAL(clicked(bool)),
            this, SLOT(setUnits()));
    connect(applyNewPortsBtn, SIGNAL(clicked(bool)),
            this, SLOT(setUnits()));
    connect(asUser, SIGNAL(toggled(bool)),
            asUserLine, SLOT(setEnabled(bool)));
    connect(asUser, SIGNAL(toggled(bool)),
            asUserLine, SLOT(clear()));
    connect(asUser, SIGNAL(toggled(bool)),
            this, SLOT(unitChanged()));
    connect(asUserLine, SIGNAL(textChanged(QString)),
            this, SLOT(unitChanged()));
}

void AppSettings::setUserName(QString _user)
{
    if ( !_user.isEmpty() ) {
        asUser->setChecked(true);
        asUserLine->setText(_user);
    };
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
void AppSettings::runChangeUnits()
{
    //QTextStream s(stdout);
    //s << "runChangeUnits" << endl;
    QVariantMap args;
    args["action"]      = "setUnits";
    args["JobPort"]     = jobPort->getPort();
    args["TestPort"]    = testPort->getPort();
    if ( asUser->isChecked() ) {
        args["User"]    = asUserLine->text();
    };
    Action act("pro.russianfedora.dnscryptclientreload.setunits");
    act.setHelperId("pro.russianfedora.dnscryptclientreload");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(resultChangeUnits(KJob*)));
    job->start();
}

/* private slots */
void AppSettings::enableUseFastOnly(bool state)
{
    useFastOnly->setChecked(false);
    useFastOnly->setEnabled(state);
}
void AppSettings::unitChanged()
{
    applyNewPortsBtn->setEnabled(true);
}
void AppSettings::setUnits()
{
    if ( asUser->isChecked() && (
         asUserLine->text().isEmpty() ||
         asUserLine->text().contains(" ")) ) {
        asUserLine->clear();
        asUser->setChecked(false);
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("Username contains blanks or empty."));
        return;
    };
    scrolled->setEnabled(false);
    if ( sender()==restoreDefaultBtn ) {
        jobPort->setPort(JOB_PORT);
        testPort->setPort(TEST_PORT);
        asUser->setChecked(false);
    };
    emit stopSystemdAppUnits();
}

void AppSettings::resultChangeUnits(KJob *_job)
{
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=nullptr ) {
        //QTextStream s(stdout);
        if ( job->data().value("jobUnit").toInt()>0 ) {
            emit jobPortChanged(jobPort->getPort());
            emit userChanged(asUserLine->text());
            KNotification::event(
                        KNotification::Notification,
                        "DNSCryptClient",
                        QString("Job systemd unit changed."));
        };
        if ( job->data().value("testUnit").toInt()>0 ) {
            emit testPortChanged(testPort->getPort());
            KNotification::event(
                        KNotification::Notification,
                        "DNSCryptClient",
                        QString("Test systemd unit changed."));
        };
        //s << "jobUnit "<< job->data().value("jobUnit").toString() << endl;
        //s << "testUnit "<< job->data().value("testUnit").toString() << endl;
        //s << job->data().value("jobUnitText").toString() << endl;
        //s << job->data().value("testUnitText").toString() << endl;
    } else {
        KNotification::event(
                    KNotification::Notification,
                    "DNSCryptClient",
                    QString("Units not changed."));
    };
    scrolled->setEnabled(true);
    emit changeUnitsFinished();
}
