#ifndef TEST_RESPOND_H
#define TEST_RESPOND_H

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "test_widget.h"

class TestRespond : public QWidget
{
    Q_OBJECT
public:
    explicit TestRespond(QWidget *parent = nullptr, QString ver = "");
    TestWidget     *testWdg;

signals:
    void            toBase();
    void            findActiveServiceStateChanged(bool);
    void            restoreAtCloseChanged(bool);

private:
    const QString   serviceVersion;
    QLabel         *testLabel, *nameLabel;
    QPushButton    *baseButton;
    QHBoxLayout    *headLayout;
    QWidget        *headWdg;

    QVBoxLayout    *commonLayout;

public slots:
};

#endif // TEST_RESPOND_H
