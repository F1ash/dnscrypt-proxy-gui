#ifndef CLICK_LABEL_H
#define CLICK_LABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QMenu>

class Click_Label : public QLabel
{
    Q_OBJECT
public:
    explicit Click_Label(QWidget *parent = Q_NULLPTR);

private slots:
    void mouseReleaseEvent(QMouseEvent*);
};

#endif // CLICK_LABEL_H
