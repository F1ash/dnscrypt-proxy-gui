#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
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
    bool            getRunAtStartState() const;
    void            setRunAtStartState(bool);
    void            setFindActiveServiceState(bool);
    void            setUseFastOnlyState(bool);
    void            setRestoreAtClose(bool);
    void            runChangePorts();

signals:
    void            toBase();
    void            findActiveServiceStateChanged(bool);
    void            useFastOnlyStateChanged(bool);
    void            restoreAtCloseChanged(bool);
    void            jobPortChanged(int);
    void            testPortChanged(int);
    void            stopSystemdAppUnits();

private:
    QLabel         *setLabel, *nameLabel, *advancedlabel;
    QPushButton    *baseButton, *restoreDefaultBtn;
    QHBoxLayout    *headLayout;
    QVBoxLayout    *appSettingsLayout;
    QHBoxLayout    *advancedButtonsLayout;
    QCheckBox      *runAtStart, *findActiveService,
                   *useFastOnly, *restoreAtClose;
    QWidget        *headWdg, *appSettings, *advancedButtons;
    QScrollArea    *scrolled;

    QVBoxLayout    *commonLayout;

public slots:

private slots:
    void            resizeEvent(QResizeEvent*);
    void            enableUseFastOnly(bool);
    void            portChanged();
    void            setPorts();
    void            resultChangePorts(KJob*);
};

#endif // APP_SETTINGS_H
