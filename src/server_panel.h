#ifndef SERVER_PANEL_H
#define SERVER_PANEL_H

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QResizeEvent>
#include "enums.h"

class ServerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ServerPanel(QWidget *parent = nullptr);

signals:
    void             toSettings();

private:
    QLabel          *servLabel;
    QComboBox       *servList;
    QPushButton     *servInfo, *appSettings;
    QHBoxLayout     *baseLayout;

public slots:
    void             changeAppState(SRV_STATUS);

private slots:
    void             resizeEvent(QResizeEvent*);
};

#endif // SERVER_PANEL_H
