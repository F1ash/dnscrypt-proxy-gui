#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include "test_thread.h"
#include "enums.h"

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);
    QPushButton    *start, *stop;

signals:

private:
    bool            processing;
    QLabel         *info;
    QProgressBar   *progress;
    QHBoxLayout    *buttonLayout;
    QWidget        *buttons;
    QVBoxLayout    *commonLayout;

    TestThread     *testThread;

private slots:
    void            testStarted();
    void            testFinished();

public slots:
    void            changeAppState(SRV_STATUS);
};

#endif // TEST_WIDGET_H
