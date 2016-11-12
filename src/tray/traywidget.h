#ifndef TRAYWIDGET_H
#define TRAYWIDGET_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
#include <QDebug>

class TrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
public :
  explicit TrayIcon(QWidget *parent = nullptr);
/*
  QAction   *startAction;
  QAction   *stopAction;
  */
  QAction   *closeAction;

private :
  QMenu     *trayIconMenu;
};

#endif
