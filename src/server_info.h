#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QVariantMap>
#include "click_label.h"

class ServerInfo : public QDialog
{
    Q_OBJECT
public:
    explicit ServerInfo(QWidget *parent = nullptr);
    void         setServerData(const QVariantMap&);

private:
    QVBoxLayout *commonLayout;
    Click_Label *name, *fullName, *description,
                *location, *coordinates, *URL,
                *version, *DNSSECvalidation, *NoLogs,
                *Namecoin, *ResolverAddress, *ProviderName,
                *ProviderPublicKey, *ProviderPublicKeyTXTrecord;
};

#endif // SERVER_INFO_H
