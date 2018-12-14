#include "test_respond.h"

TestRespond::TestRespond(QWidget *parent, QString ver) :
    QWidget(parent), serviceVersion(ver)
{
    setContentsMargins(0, 0, 0, 0);
    testLabel = new QLabel(this);
    testLabel->setPixmap(QIcon::fromTheme(
                            "DNSCryptClient_test",
                            QIcon(":/test.png"))
                        .pixmap(32));
    testLabel->setSizePolicy(
               QSizePolicy(
                   QSizePolicy::Ignored,
                   QSizePolicy::Ignored));
    testLabel->setContentsMargins(0, 0, 0, 0);
    nameLabel = new QLabel(this);
    nameLabel->setText("Test servers respond");
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
    headLayout->addWidget(testLabel, 1);
    headLayout->addWidget(nameLabel, 10, Qt::AlignCenter);
    headLayout->addWidget(baseButton, 1);
    headWdg = new QWidget(this);
    headWdg->setContentsMargins(0, 0, 0, 0);
    headWdg->setLayout(headLayout);

    testWdg = new TestWidget(this);

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(headWdg, 1);
    commonLayout->addWidget(testWdg, 5);
    commonLayout->addStretch(-1);
    setLayout(commonLayout);

    connect(baseButton, SIGNAL(released()),
            this, SIGNAL(toBase()));
}
