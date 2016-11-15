#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QVariantMap>

class ServerInfo : public QDialog
{
    Q_OBJECT
public:
    explicit ServerInfo(QWidget *parent = nullptr);
    void         setServerData(const QVariantMap&);

private:
    QVBoxLayout *commonLayout;
    QLabel      *name, *fullName, *description,
                *location, *coordinates, *URL,
                *version, *DNSSECvalidation, *NoLogs,
                *Namecoin, *ResolverAddress, *ProviderName,
                *ProviderPublicKey, *ProviderPublicKeyTXTrecord;
};

#endif // SERVER_INFO_H
