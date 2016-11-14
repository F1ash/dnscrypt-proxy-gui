#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QVariantMap>
#include "enums.h"

class InfoPanel : public QStackedWidget
{
    Q_OBJECT
public:
    explicit InfoPanel(QWidget *parent = nullptr);

signals:

private:
    QLabel          *location, *fullName, *attention;
    QVBoxLayout     *servLayout, *attentLayout;
    QWidget         *servInfo, *attentions;

public slots:
    void             changeAppState(SRV_STATUS);
    void             setServerDescription(const QVariantMap&);
};

#endif // INFO_PANEL_H
