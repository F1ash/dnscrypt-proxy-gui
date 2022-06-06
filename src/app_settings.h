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
#include <kauth/kauthcore_export.h>
#if KAUTHCORE_ENABLE_DEPRECATED_SINCE(5, 92)
#include <kauth/executejob.h>
#else
#include <kauth.h>
#endif
#include <knotification.h>
using namespace KAuth;

#define JOB_PORT    53
#define TEST_PORT   53535

class AppSettings : public QWidget
{
    Q_OBJECT
public:
    explicit AppSettings(QWidget *parent = Q_NULLPTR, QString ver = "");
    PortSettings   *jobPort, *testPort;
    QPushButton    *applyNewPortsBtn;
    void            setUserName(QString);
    bool            getRunAtStartState() const;
    void            setRunAtStartState(bool);
    bool            getUnhideAtStartState() const;
    void            setUnhideAtStartState(bool);
    void            setUseActiveServiceState(bool);
    void            setUseFastOnlyState(bool);
    void            setRestoreAtClose(bool);
    void            setShowMessagesState(bool);
    void            setShowBasicMsgOnlyState(bool);
    void            runChangeUnits();

signals:
    void            toBase();
    void            findActiveServiceStateChanged(bool);
    void            useFastOnlyStateChanged(bool);
    void            restoreAtCloseChanged(bool);
    void            showMsgStateChanged(bool);
    void            showBasicMsgOnlyStateChanged(bool);
    void            jobPortChanged(int);
    void            testPortChanged(int);
    void            userChanged(QString);
    void            stopSystemdAppUnits();
    void            changeUnitsFinished();

private:
    const QString   serviceVersion;
    QLabel         *setLabel, *nameLabel, *advancedLabel;
    QLineEdit      *asUserLine;
    QPushButton    *baseButton, *restoreDefaultBtn;
    QHBoxLayout    *headLayout, *asUserLayout;
    QVBoxLayout    *appSettingsLayout;
    QHBoxLayout    *advancedButtonsLayout;
    QCheckBox      *runAtStart, *useActiveService,
                   *useFastOnly, *restoreAtClose,
                   *asUser, *unhideAtStart,
                   *showMessages, *showBasicMsgOnly;
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
