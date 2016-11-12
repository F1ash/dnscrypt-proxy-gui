#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "enums.h"

class InfoPanel : public QStackedWidget
{
    Q_OBJECT
public:
    explicit InfoPanel(QWidget *parent = nullptr);

signals:

private:
    QLabel          *location, *name, *attention;
    QVBoxLayout     *servLayout, *attentLayout;
    QWidget         *servInfo, *attentions;

public slots:
    void             changeAppState(SRV_STATUS);
};

#endif // INFO_PANEL_H
