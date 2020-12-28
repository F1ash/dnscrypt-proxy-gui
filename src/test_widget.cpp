#include "test_widget.h"
//#include <QTextStream>

TestWidget::TestWidget(QWidget *parent, QString ver, QString cfg) :
    QWidget(parent), serviceVersion(ver), cfg_data(cfg)
{
    processing = false;
    active = false;
    info = new QLabel(this);
    progress = new QProgressBar(this);
    progress->setEnabled(false);
    start = new QPushButton("Start", this);
    stop = new QPushButton("Stop", this);
    //stop->setEnabled(false);
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
            this, SLOT(startTest()));
    connect(stop, SIGNAL(released()),
            this, SLOT(stopTest()));

    connect(this, SIGNAL(nextItem()),
            this, SLOT(checkServerRespond()));
}

void TestWidget::setServerList(QStringList _list)
{
    list = _list;
    progress->setRange(0, _list.count());
}
void TestWidget::setTestPort(int port)
{
    testPort = port;
}
bool TestWidget::isActive() const
{
    return active;
}

/* private slots */
void TestWidget::startTest()
{
    progress->setEnabled(true);
    start->setEnabled(false);
    stop->setEnabled(true);
    processing = true;
    active = true;
    //emit started();
    progress->setValue(0);
    counter = 0;
    emit nextItem();
}
void TestWidget::stopTest()
{
    stop->setEnabled(false);
    processing = false;
}
void TestWidget::finishTest()
{
    active = false;
    progress->setEnabled(false);
    stop->setEnabled(false);
    start->setEnabled(true);
    emit finished();
    info->clear();
}
void TestWidget::checkServerRespond()
{
    //QTextStream s(stdout);
    if ( !processing || counter>=list.count() ) {
        finishTest();
        return;
    };
    QVariantMap args;
    args["action"] = "startTest";
    args["port"]   = testPort;
    args["server"] = list.at(counter);
    Action act;
    if ( serviceVersion.compare("2")>0 ) {
        args["cfg_data"] = cfg_data;
        act.setName("pro.russianfedora.dnscryptclienttest.starttestv2");
    } else {
        act.setName("pro.russianfedora.dnscryptclienttest.starttest");
    };
    act.setHelperId("pro.russianfedora.dnscryptclienttest");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(resultCheckServerRespond(KJob*)));
    job->start();
    //s<< counter <<"\t"<< "startTest"<< endl;
}
void TestWidget::stopServiceSlice()
{
    //QTextStream s(stdout);
    QVariantMap args;
    args["action"]  = "stopTestSlice";
    args["version"] = serviceVersion;
    Action act("pro.russianfedora.dnscryptclienttest.stoptestslice");
    act.setHelperId("pro.russianfedora.dnscryptclienttest");
    act.setArguments(args);
    ExecuteJob *job = act.execute();
    job->setParent(this);
    job->setAutoDelete(true);
    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(resultStopServiceSlice(KJob*)));
    job->start();
    //s<< counter <<"\t"<< "stopTestSlice"<< endl;
}
void TestWidget::resultCheckServerRespond(KJob *_job)
{
    //QTextStream s(stdout);
    QString i = QString("\nIt left about %1 sec...")
            .arg(list.count()-counter-1);
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=Q_NULLPTR ) {
        QString code        = job->data().value("code").toString();
        //QString msg         = job->data().value("msg").toString();
        //QString err         = job->data().value("err").toString();
        //QString entry       = job->data().value("entry").toString();
        QString answ        = job->data().value("answ").toString();
        QString resp_icon   = job->data().value("resp").toString();
        if ( code.toInt()==0 && answ.toInt()>0 ) {
            emit serverRespondIcon(list.at(counter), resp_icon);
            i.prepend(QString("%1\tis %2.")
                      .arg(list.at(counter)).arg(resp_icon));
        } else {
            emit serverRespondIcon(list.at(counter), "none");
            i.prepend(QString("%1\tis none.").arg(list.at(counter)));
        };
    } else {
        emit serverRespondIcon(list.at(counter), "none");
        i.prepend(QString("%1\tis none.").arg(list.at(counter)));
    };
    //s<< counter <<"\t"<< "start1"<< endl;
    info->setText(i);
    stopServiceSlice();
}
void TestWidget::resultStopServiceSlice(KJob *_job)
{
    //QTextStream s(stdout);
    ExecuteJob *job = static_cast<ExecuteJob*>(_job);
    if ( job!=Q_NULLPTR ) {
        QString code = job->data().value("code").toString();
        if ( code.toInt()!=0 ) {
            stopServiceSlice();
        };
    } else {
        //stopServiceSlice();
    };
    //s<< counter <<"\t"<< "stopslice1"<< endl;
    ++counter;
    progress->setValue(counter);
    emit nextItem();
}

/* public slots */
void TestWidget::changeAppState(SRV_STATUS state)
{
    switch (state) {
    case READY:
        //start->setEnabled(true);
        break;
    case PROCESSING:
        //start->setEnabled(false);
        break;
    case ACTIVE:
        //buttons->setDisabled(true);
        //info->setText("To use necessary to stop service.");
        break;
    case INACTIVE:
    case FAILED:
        //buttons->setEnabled(true);
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
