#include <kauth.h>
using namespace KAuth;

class DNSCryptClientTestHelper : public QObject
{
    Q_OBJECT
public:
    explicit DNSCryptClientTestHelper(QObject *parent = nullptr);

public slots:
    ActionReply     starttest(const QVariantMap args) const;
    ActionReply     stoptestslice(const QVariantMap args) const;
};
