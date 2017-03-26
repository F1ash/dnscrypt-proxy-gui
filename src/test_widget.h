#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include "enums.h"
#include <kauth.h>
using namespace KAuth;

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);
    void                setServerList(QStringList);

signals:
    void                started();
    void                finished();
    void                serverRespondIcon(const QString, const QString);
    void                nextItem();
    void                endList();

private:
    int                 counter;
    bool                processing;
    QLabel             *info;
    QProgressBar       *progress;
    QPushButton        *start, *stop;
    QHBoxLayout        *buttonLayout;
    QWidget            *buttons;
    QVBoxLayout        *commonLayout;

    QStringList         list;
    QDBusConnection     connection;

private slots:
    void                startTest();
    void                stopTest();
    void                finishTest();
    void                checkServerRespond();
    void                stopServiceSlice();
    void                resultCheckServerRespond(KJob*);
    void                resultStopServiceSlice(KJob*);

public slots:
    void                changeAppState(SRV_STATUS);
};

#endif // TEST_WIDGET_H
