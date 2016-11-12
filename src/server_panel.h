#ifndef SERVER_PANEL_H
#define SERVER_PANEL_H

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "enums.h"

class ServerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ServerPanel(QWidget *parent = nullptr);

signals:

private:
    QLabel          *servLabel;
    QComboBox       *servList;
    QPushButton     *servInfo;
    QHBoxLayout     *baseLayout;

public slots:
    void             changeAppState(SRV_STATUS);
};

#endif // SERVER_PANEL_H
