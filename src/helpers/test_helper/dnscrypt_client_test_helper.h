#include <kauth/kauthcore_export.h>
#if KAUTHCORE_ENABLE_DEPRECATED_SINCE(5, 92)
#include <kauth/actionreply.h>
#include <kauth/helpersupport.h>
#else
#include <kauth.h>
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
