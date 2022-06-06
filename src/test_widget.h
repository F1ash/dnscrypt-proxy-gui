#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include "enums.h"
#include <kauth/kauthcore_export.h>
#if KAUTHCORE_ENABLE_DEPRECATED_SINCE(5, 92)
#include <kauth/executejob.h>
#else
#include <kauth.h>
#endif
using namespace KAuth;

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = Q_NULLPTR,
                        QString ver = "",
                        QString cfg = "");
    void                setServerList(QStringList);
    void                setTestPort(int);
    bool                isActive() const;

signals:
    void                started();
    void                finished();
    void                serverRespondIcon(const QString, const QString);
    void                nextItem();

private:
    int                 counter, testPort;
    bool                processing, active;
    QLabel             *info;
    QProgressBar       *progress;
    QPushButton        *start, *stop;
    QHBoxLayout        *buttonLayout;
    QWidget            *buttons;
    QVBoxLayout        *commonLayout;

    QStringList         list;
    QString             serviceVersion, cfg_data;

private slots:
    void                startTest();
    void                finishTest();
    void                checkServerRespond();
    void                stopServiceSlice();
    void                resultCheckServerRespond(KJob*);
    void                resultStopServiceSlice(KJob*);

public slots:
    void                changeAppState(SRV_STATUS);
    void                stopTest();
};

#endif // TEST_WIDGET_H
