#include <kauth.h>
using namespace KAuth;

class DNSCryptClientReloadHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientReloadHelper(QObject *parent = nullptr);

public slots:
    ActionReply     setunits(const QVariantMap args) const;
};
