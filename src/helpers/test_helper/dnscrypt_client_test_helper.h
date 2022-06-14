#include <kauth_version.h>
#if KAUTH_VERSION < ((5<<16)|(92<<8)|(0))
    #include <kauth.h>
#else
    #include <kauth/actionreply.h>
    #include <kauth/helpersupport.h>
#endif
using namespace KAuth;

class DNSCryptClientTestHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientTestHelper(QObject *parent = Q_NULLPTR);

public slots:
    ActionReply     starttest(const QVariantMap args) const;
    ActionReply     stoptestslice(const QVariantMap args) const;
    ActionReply     initialization(const QVariantMap args) const;
    ActionReply     getversion(const QVariantMap args) const;
    ActionReply     getlistofservers(const QVariantMap args) const;
    ActionReply     starttestv2(const QVariantMap args) const;
};
