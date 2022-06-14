#include <kauth_version.h>
#if KAUTH_VERSION < ((5<<16)|(92<<8)|(0))
    #include <kauth.h>
#else
    #include <kauth/actionreply.h>
    #include <kauth/helpersupport.h>
#endif
using namespace KAuth;

class DNSCryptClientHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientHelper(QObject *parent = Q_NULLPTR);

public slots:
    //ActionReply     create(const QVariantMap args) const;
    ActionReply     start(const QVariantMap args) const;
    ActionReply     stop(const QVariantMap args) const;
    ActionReply     restore(const QVariantMap args) const;
    ActionReply     stopslice(const QVariantMap args) const;
    ActionReply     startv2(const QVariantMap args) const;
    ActionReply     stopv2(const QVariantMap args) const;
};
