#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include "port_settings.h"
#include <kauth.h>
#include <knotification.h>
using namespace KAuth;

#define JOB_PORT    53
#define TEST_PORT   53535

class AppSettings : public QWidget
{
    Q_OBJECT
public:
    explicit AppSettings(QWidget *parent = nullptr);
    PortSettings   *jobPort, *testPort;
    QPushButton    *applyNewPortsBtn;
    void            setUserName(QString);
    bool            getRunAtStartState() const;
    void            setRunAtStartState(bool);
    bool            getUnhideAtStartState() const;
    void            setUnhideAtStartState(bool);
    void            setFindActiveServiceState(bool);
    void            setUseFastOnlyState(bool);
    void            setRestoreAtClose(bool);
    void            runChangeUnits();

signals:
    void            toBase();
    void            findActiveServiceStateChanged(bool);
    void            useFastOnlyStateChanged(bool);
    void            restoreAtCloseChanged(bool);
    void            jobPortChanged(int);
    void            testPortChanged(int);
    void            userChanged(QString);
    void            stopSystemdAppUnits();
    void            changeUnitsFinished();

private:
    QLabel         *setLabel, *nameLabel, *advancedLabel;
    QLineEdit      *asUserLine;
    QPushButton    *baseButton, *restoreDefaultBtn;
    QHBoxLayout    *headLayout, *asUserLayout;
    QVBoxLayout    *appSettingsLayout;
    QHBoxLayout    *advancedButtonsLayout;
    QCheckBox      *runAtStart, *findActiveService,
                   *useFastOnly, *restoreAtClose,
                   *asUser, *unhideAtStart;
    QWidget        *headWdg, *appSettings,
                   *asUserWdg, *advancedButtons;
    QScrollArea    *scrolled;

    QVBoxLayout    *commonLayout;

public slots:

private slots:
    void            enableUseFastOnly(bool);
    void            unitChanged();
    void            setUnits();
    void            resultChangeUnits(KJob*);
};

#endif // APP_SETTINGS_H
