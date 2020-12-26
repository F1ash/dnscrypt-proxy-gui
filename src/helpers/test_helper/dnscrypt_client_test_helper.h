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
    ActionReply     initialization(const QVariantMap args) const;
    ActionReply     getversion(const QVariantMap args) const;
    ActionReply     getlistofservers(const QVariantMap args) const;
    ActionReply     starttestv2(const QVariantMap args) const;
};
