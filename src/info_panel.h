#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QVariantMap>
#include <QTimerEvent>
#include "enums.h"

class InfoPanel : public QStackedWidget
{
    Q_OBJECT
public:
    explicit InfoPanel(QWidget *parent = Q_NULLPTR);

signals:

private:
    int              timerId = 0;
    QString          st;
    QLabel          *location, *fullName, *description,
                    *respond, *srvState, *attention;
    QVBoxLayout     *servLayout, *attentLayout;
    QWidget         *servInfo, *attentions;

public slots:
    void             changeAppState(SRV_STATUS);
    void             setServerDescription(const QVariantMap&);

private slots:
    void             timerEvent(QTimerEvent*);
};

#endif // INFO_PANEL_H
