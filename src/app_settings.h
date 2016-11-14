#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QResizeEvent>

class AppSettings : public QWidget
{
    Q_OBJECT
public:
    explicit AppSettings(QWidget *parent = nullptr);
    void            setRunAtStartState(bool);
    void            setFindActiveServiceState(bool);

signals:
    void            toBase();
    void            findActiveServiceStateChanged(bool);
    void            runAtStartStateChanged(bool);

private:
    QLabel         *setLabel, *nameLabel;
    QPushButton    *baseButton;
    QHBoxLayout    *headLayout;
    QVBoxLayout    *appSetLayout;
    QCheckBox      *runAtStart, *findActiveService;
    QWidget        *headWdg, *appSettings;

    QVBoxLayout    *commonLayout;

public slots:

private slots:
    void            resizeEvent(QResizeEvent*);
};

#endif // APP_SETTINGS_H
