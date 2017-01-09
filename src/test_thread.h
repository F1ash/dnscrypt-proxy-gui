#ifndef TEST_THREAD_H
#define TEST_THREAD_H

#include <QThread>

class TestThread : public QThread
{
    Q_OBJECT
public:
    explicit TestThread(QObject *parent = nullptr);
};

#endif // TEST_THREAD_H
