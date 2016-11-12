#ifndef BUTTON_PANEL_H
#define BUTTON_PANEL_H

#include <QPushButton>
#include <QHBoxLayout>
#include <QResizeEvent>
#include "enums.h"

class ButtonPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonPanel(QWidget *parent = nullptr);

signals:
    void             startProxing();
    void             stopProxing();
    void             restoreSettings();

private:
    QPushButton     *start, *stop, *restore;
    QHBoxLayout     *baseLayout;

public slots:
    void             changeAppState(SRV_STATUS);

private slots:
    void             resizeEvent(QResizeEvent*);
};

#endif // BUTTON_PANEL_H
