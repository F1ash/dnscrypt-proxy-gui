extern "C" {
#include "dns.h"
}
#include "dnscrypt_client_test_helper.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <private/qdbusmetatype_p.h>
#include <private/qdbusutil_p.h>
#include <QFile>
#include <QProcess>
//#include <QTextStream>

struct PrimitivePair
{
    QString         name;
    QVariant        value;
};
Q_DECLARE_METATYPE(PrimitivePair)
typedef QList<PrimitivePair>    SrvParameters;
Q_DECLARE_METATYPE(SrvParameters)

struct ComplexPair
{
    QString         name;
    SrvParameters   value;
};
Q_DECLARE_METATYPE(ComplexPair)
typedef QList<ComplexPair>      AuxParameters;
Q_DECLARE_METATYPE(AuxParameters)

QString getRespondIconName(qreal r)
{
    if        ( 0<r   && r<=0.3 ) {
        return "fast";
    } else if ( 0.3<r && r<=0.7 ) {
        return "middle";
    } else if ( 0.7<r && r<3.0 ) {
        return "slow";
    };
    return "none";
}
QString get_key_varmap(const QVariantMap &args, const QString& key)
{
    QString value;
    if (args.keys().contains(key)) {
        value = args[key].toString();
    };
    return value;
}
QString readFile(const QString &_path)
{
    QString ret;
    QFile f(_path);
    bool opened = f.open(QIODevice::ReadOnly);
    if ( opened ) {
        ret = QString::fromUtf8( f.readAll() );
        f.close();
    };
    return ret;
}
qint64  writeFile(const QString &_path, const QString &entry)
{
    qint64 ret = 0;
    QFile f(_path);
    bool opened = f.open(QIODevice::WriteOnly);
    if ( opened ) {
        ret = f.write(entry.toUtf8().data(), entry.size());
        f.close();
    } else {
        ret = -1;
    };
    return ret;
}

DNSCryptClientTestHelper::DNSCryptClientTestHelper(QObject *parent) :
    QObject(parent)
{
    qDBusRegisterMetaType<PrimitivePair>();
    qDBusRegisterMetaType<SrvParameters>();
    qDBusRegisterMetaType<ComplexPair>();
    qDBusRegisterMetaType<AuxParameters>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const PrimitivePair &pair)
{
    argument.beginStructure();
    argument << pair.name << QDBusVariant(pair.value);
    argument.endStructure();
    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, PrimitivePair &pair)
{
    argument.beginStructure();
    argument >> pair.name >> pair.value;
    argument.endStructure();
    return argument;
}
QDBusArgument &operator<<(QDBusArgument &argument, const SrvParameters &list)
{
    int id = qMetaTypeId<PrimitivePair>();
    argument.beginArray(id);
    //foreach (PrimitivePair item, list) {
    //    argument << item;
    //};
    SrvParameters::ConstIterator it = list.constBegin();
    SrvParameters::ConstIterator end = list.constEnd();
    for ( ; it != end; ++it)
        argument << *it;
    argument.endArray();
    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, SrvParameters &list)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        PrimitivePair item;
        argument >> item;
        list.append(item);
    };
    argument.endArray();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const ComplexPair &pair)
{
    argument.beginStructure();
    argument << pair.name << pair.value;
    argument.endStructure();
    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, ComplexPair &pair)
{
    argument.beginStructure();
    argument >> pair.name >> pair.value;
    argument.endStructure();
    return argument;
}
QDBusArgument &operator<<(QDBusArgument &argument, const AuxParameters &list)
{
    int id = qMetaTypeId<ComplexPair>();
    argument.beginArray(id);
    //foreach (ComplexPair item, list) {
    //    argument << item;
    //};
    AuxParameters::ConstIterator it = list.constBegin();
    AuxParameters::ConstIterator end = list.constEnd();
    for ( ; it != end; ++it)
        argument << *it;
    argument.endArray();
    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, AuxParameters &list)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        ComplexPair item;
        argument >> item;
        list.append(item);
    };
    argument.endArray();
    return argument;
}


ActionReply DNSCryptClientTestHelper::starttest(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="startTest" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QString servName = get_key_varmap(args, "server");
    int testPort     = get_key_varmap(args, "port").toInt();

    qint64 code = 0;
    QString entry = readFile("/etc/resolv.conf");
    if ( !entry.startsWith("nameserver 127.0.0.1\n") ) {
        entry.clear();
        entry.append("nameserver 127.0.0.1\n");
        code = writeFile("/etc/resolv.conf", entry);
    };
    QVariantMap retdata;
    if ( code != -1 ) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    "org.freedesktop.systemd1",
                    "/org/freedesktop/systemd1",
                    "org.freedesktop.systemd1.Manager",
                    "StartUnit");
        QList<QVariant> _args;
        _args<<QString("DNSCryptClient_test@%1.service")
               .arg(servName)
             <<"fail";
        msg.setArguments(_args);
        QDBusMessage res = QDBusConnection::systemBus()
                .call(msg, QDBus::Block);
        QString str;
        foreach (QVariant arg, res.arguments()) {
            str.append(QDBusUtil::argumentToString(arg));
            str.append("\n");
        };
        retdata["msg"]          = str;
        long unsigned int t     = 0;
        int domain = (servName.endsWith("ipv6"))? 6 : 4;
        switch (res.type()) {
        case QDBusMessage::ReplyMessage:
            retdata["entry"]    = entry;
            retdata["code"]     = QString::number(0);
            retdata["answ"]     = QString::number(
                        is_responsible(&t, testPort, domain));
            retdata["resp"]     = getRespondIconName(qreal(t)/1000000);
            break;
        case QDBusMessage::ErrorMessage:
        default:
            retdata["entry"]    = entry;
            retdata["code"]     = QString::number(-1);
            retdata["err"]      = res.errorMessage();
            break;
        };
    } else {
        retdata["msg"]          = "Start failed";
        retdata["code"]         = QString::number(-1);
        retdata["err"]          = "Resolv.conf not changed";
    };

    reply.setData(retdata);
    return reply;
}
ActionReply DNSCryptClientTestHelper::stoptestslice(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="stopTestSlice" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QVariantMap retdata;
    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1",
                "org.freedesktop.systemd1.Manager",
                "StopUnit");
    QList<QVariant> _args;
    _args<<QString("system-DNSCryptClient_test.slice")<<"fail";
    msg.setArguments(_args);
    QDBusMessage res = QDBusConnection::systemBus()
            .call(msg, QDBus::Block);
    QString str;
    foreach (QVariant arg, res.arguments()) {
        str.append(QDBusUtil::argumentToString(arg));
        str.append("\n");
    };
    retdata["msg"]          = str;
    switch (res.type()) {
    case QDBusMessage::ReplyMessage:
        retdata["code"]     = QString::number(0);
        break;
    case QDBusMessage::ErrorMessage:
        retdata["code"]     = QString::number(-1);
        retdata["err"]      = res.errorMessage();
        break;
    default:
        retdata["msg"]      = "Slice not stopped";
        retdata["code"]     = QString::number(-1);
        break;
    };

    reply.setData(retdata);
    return reply;
}

ActionReply DNSCryptClientTestHelper::getversion(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="getVersion" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QVariantMap retdata;
    QString _str, _ver;
    QProcess p;
    p.setProgram("dnscrypt-proxy");
    p.setArguments(QStringList()<<"--version");
    p.start(QIODevice::ReadOnly);
    bool ready = p.waitForReadyRead(-1);
    if ( ready ) {
        _str = QString::fromUtf8(p.readAllStandardOutput());
        _ver = _str.split(" ").last();
    };
    p.close();

    retdata["version"]          = _ver;
    reply.setData(retdata);
    return reply;
}
ActionReply DNSCryptClientTestHelper::getlistofservers(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="getListOfServers" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QVariantMap retdata;
    QString _md = readFile("/var/cache/dnscrypt-proxy/public-resolvers.md");
    QStringList _md_lines, _list, _description;
    _md_lines = _md.split("\n");
    bool _records_began = false;
    QString currServerName;
    foreach(QString _s, _md_lines) {
        if ( !_records_began && _s.startsWith("## ") ) {
            _records_began = true;
        } else if ( !_records_began ) {
            continue;
        };
        if ( _s.isEmpty() ) continue;
        if ( _s.startsWith("## ") ) {
            currServerName.append( _s.split("## ").last() );
            _list.append(currServerName);
            continue;
        };
        _description.append(_s);
        if ( _s.startsWith("sdns://") ) {
            retdata[currServerName] = _description;
            currServerName.clear();
            _description.clear();
        };
    };

    retdata["listOfServers"]    = _list;
    reply.setData(retdata);
    return reply;
}

KAUTH_HELPER_MAIN("pro.russianfedora.dnscryptclienttest", DNSCryptClientTestHelper)
