#if defined(KAUTHCORE_ENABLE_DEPRECATED_SINCE)
    #include <kauth.h>
#else
    #include <kauth/kauthcore_export.h>
    #if KAUTHCORE_ENABLE_DEPRECATED_SINCE(5, 92)
        #include <kauth/actionreply.h>
        #include <kauth/helpersupport.h>
    #else
        #include <kauth.h>
    #endif
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
