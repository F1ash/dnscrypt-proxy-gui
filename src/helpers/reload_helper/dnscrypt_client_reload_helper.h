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

class DNSCryptClientReloadHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientReloadHelper(QObject *parent = Q_NULLPTR);

public slots:
    ActionReply     setunits(const QVariantMap args) const;
};
