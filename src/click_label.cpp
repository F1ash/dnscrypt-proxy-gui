#include "click_label.h"
#include <QClipboard>
#include <QApplication>

Click_Label::Click_Label(QWidget *parent) :
    QLabel(parent)
{

}

void Click_Label::mouseReleaseEvent(QMouseEvent *ev)
{
    if ( ev->type()==QEvent::MouseButtonRelease ) {
        //setStyleSheet("QLabel {background-color: gold;}");
        QMenu *menu = new QMenu(this);
        QAction *copy = menu->addAction("Copy to Clipboard");
        //menu->setStyleSheet("QMenu {background-color: blue;}");
        QAction *act = menu->exec(mapToGlobal(ev->pos()+QPoint(3,0)));
        if ( act==copy ) {
            QClipboard *c = QApplication::clipboard();
            c->setText(this->text());
        };
        //setStyleSheet("QLabel {background-color: white;}");
    };
}
