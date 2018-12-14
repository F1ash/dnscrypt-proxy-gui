#ifndef SERVER_PANEL_H
#define SERVER_PANEL_H

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QResizeEvent>
#include "enums.h"
#include <QStandardItemModel>

class ServerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ServerPanel(QWidget *parent = nullptr, QString ver = "");
    void             setServerDataMap(const QVariantMap);
    void             setLastServer(const QString&);
    QString          getCurrentServer() const;
    QString          getCurrentRespondIconName() const;
    int              getServerListCount() const;
    void             setNextServer();
    QString          getItemName(int) const;
    QString          getRespondIconName(int) const;
    bool             getItemState(int) const;
    bool             serverIsEnabled() const;
    void             changeServerInfo();

signals:
    void             toSettings();
    void             toTest();
    void             serverData(const QVariantMap&);
    void             readyForStart();
    void             checkItem(const QString, const QString);
    void             serverActivated();

private:
    const QString    serviceVersion;
    QString          lastServer;
    QLabel          *servLabel;
    QComboBox       *servList;
    QPushButton     *servInfo, *appSettings, *testRespond;
    QHBoxLayout     *baseLayout;

    QStandardItemModel
                    *servItemModel;
    QList<QStandardItem*>
                     servItems;
    QVariantMap      listOfServers;

public slots:
    void             changeAppState(SRV_STATUS);
    void             setItemIcon(QString, QString);

private slots:
    void             resizeEvent(QResizeEvent*);
    void             serverDataChanged(int);
    void             addServer(const QVariantMap&);
    void             findLastServer();
    void             showServerInfo();
    void             changeItemState(QModelIndex, QModelIndex);
};

#endif // SERVER_PANEL_H
