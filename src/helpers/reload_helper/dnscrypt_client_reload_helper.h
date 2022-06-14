#include <kauth_version.h>
#if KAUTH_VERSION < ((5<<16)|(92<<8)|(0))
    #include <kauth.h>
#else
    #include <kauth/actionreply.h>
    #include <kauth/helpersupport.h>
#endif
using namespace KAuth;

class DNSCryptClientReloadHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientReloadHelper(QObject *parent = Q_NULLPTR);

public slots:
    ActionReply     setunits(const QVariantMap args) const;
};
