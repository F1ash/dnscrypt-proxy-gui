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
#include <QTemporaryFile>
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
bool    started()
{
    QString _str;
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LANG", "C"); // Add an environment variable
    p.setProcessEnvironment(env);
    p.setProgram("dnscrypt-proxy");
    p.setArguments(QStringList()<<"-service"<<"start");
    p.start(QIODevice::ReadOnly);
    if ( p.waitForFinished(-1) ) {
        _str = p.readAllStandardOutput().constData();
    } else {
        _str = p.errorString();
    };
    p.close();

    bool ret = _str.contains("NOTICE") && _str.contains("Service started");

    return ret;
}
bool    installed()
{
    QString _str;
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LANG", "C"); // Add an environment variable
    p.setProcessEnvironment(env);
    p.setProgram("dnscrypt-proxy");
    p.setArguments(QStringList()<<"-service"<<"install");
    p.start(QIODevice::ReadOnly);
    if ( p.waitForFinished(-1) ) {
        _str = p.readAllStandardOutput().constData();
    } else {
        _str = p.errorString();
    };
    p.close();

    bool ret = _str.contains("NOTICE") && _str.contains("Installed as a service");
    if ( !ret ) {
        ret = _str.contains("FATAL") && _str.contains("Init already exists");
    };

    return ret;
}
bool    preparedConfig()
{
    bool ret = false;
    QString entry = readFile("/etc/dnscrypt-proxy/dnscrypt-proxy.toml");
    QStringList _data, _new_data;
    _data = entry.split("\n");
    foreach(QString _s, _data) {
        QString _new_str;
        if ( _s.contains("server_names") ) {
            QStringList _parts = _s.split(" ", Qt::KeepEmptyParts);
            int idx = _parts.indexOf("server_names");
            if ( idx == 1 ) {
                _parts.removeFirst();
                ret = true;
            } else if ( idx == 0 ) {
                ret = true;
            };
            _new_str.append(_parts.join(" "));
        } else if ( _s.startsWith("listen_addresses")
                    && !_s.endsWith("['127.0.0.1:53', '[::1]:53']") ) {
            QStringList _parts = _s.split(" = ");
            _parts.removeLast();
            // Listen for both domain type (ipv4\ipv6)
            _parts.append("['127.0.0.1:53', '[::1]:53']");
            _new_str.append(_parts.join(" = "));
        } else {
            _new_str.append(_s);
        };
        _new_data.append(_new_str);
    };
    qint64 code = writeFile("/etc/dnscrypt-proxy/dnscrypt-proxy.toml",
                            _new_data.join("\n"));
    ret &= (code > 0);

    return ret;
}
bool    preparedServiceUnit()
{
    bool ret = false;
    QString entry = readFile("/etc/systemd/system/dnscrypt-proxy.service");
    QStringList _data, _new_data;
    _data = entry.split("\n");
    foreach(QString _s, _data) {
        QString _new_str;
        if ( _s.startsWith("WorkingDirectory")
             && !_s.endsWith("/etc/dnscrypt-proxy") ) {
            QStringList _parts = _s.split("=", Qt::KeepEmptyParts);
            _parts.removeLast();
            _parts.append("/etc/dnscrypt-proxy");
            ret = true;
            _new_str.append(_parts.join("="));
        } else if ( _s.startsWith("WorkingDirectory")
                    && _s.endsWith("/etc/dnscrypt-proxy") ) {
            _new_str.append(_s);
            ret = true;
        } else {
            _new_str.append(_s);
        };
        _new_data.append(_new_str);
    };
    qint64 code = writeFile("/etc/systemd/system/dnscrypt-proxy.service",
                            _new_data.join("\n"));
    ret &= (code > 0);

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
    if ( !entry.startsWith("nameserver 127.0.0.1\nnameserver ::1\n") ) {
        entry.clear();
        entry.append("nameserver 127.0.0.1\nnameserver ::1\n");
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

    QString serviceVersion = get_key_varmap(args, "version");
    QVariantMap retdata;
    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1",
                "org.freedesktop.systemd1.Manager",
                "StopUnit");
    QList<QVariant> _args;
    if ( serviceVersion.compare("2")>0 ) {
        _args<<QString("DNSCryptClient_test_v2.service")<<"fail";
    } else {
        _args<<QString("system-DNSCryptClient_test.slice")<<"fail";
    };
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

ActionReply DNSCryptClientTestHelper::initialization(const QVariantMap args) const
{
/*
 * check for installed dnscrypt-proxy.service;
 * prepare config file and reload units for apply new config;
 * start dnscrypt-proxy.service;
 */
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="initialization" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QVariantMap retdata;
    int code = 0;
    if ( installed() ) {
        retdata["installed"] = "Service installed";
        if ( preparedConfig() ) {
            retdata["preparedCfg"] = "Config file prepared for use";
            if ( preparedServiceUnit() ) {
                retdata["preparedUnit"] = "Service unit file prepared for use";
                // need to reload services (aka 'daemon-reload')
                // for use edited service unit file
                QDBusMessage msg = QDBusMessage::createMethodCall(
                            "org.freedesktop.systemd1",
                            "/org/freedesktop/systemd1",
                            "org.freedesktop.systemd1.Manager",
                            "Reload");
                QDBusMessage res = QDBusConnection::systemBus()
                        .call(msg, QDBus::Block);
                QString str;
                foreach (QVariant arg, res.arguments()) {
                    str.append(QDBusUtil::argumentToString(arg));
                    str.append("\n");
                };
                code = 1;
                //if ( started() ) {
                //    retdata["started"] = "Service started";
                //    code = 1;
                //} else {
                //    retdata["started"] = "Service can't to start";
                //    code = -1;
                //};
            } else {
                retdata["preparedUnit"] = "Service unit file not prepared for use.\n\
Prepare manually or restart application";
                code = -1;
            };
        } else {
            retdata["preparedCfg"] = "Config file not prepared for use.\n\
Prepare manually or restart application";
            code = -1;
        };
    } else {
        retdata["installed"] = "Service not installed;\n\
check 'dnscrypt-proxy' package installation";
        code = -1;
    };

    retdata["code"] = QString::number(code);
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
    QString _toml = readFile("/etc/dnscrypt-proxy/dnscrypt-proxy.toml");

    retdata["listOfServers"]    = _list;
    retdata["cfg_data"]         = _toml;
    reply.setData(retdata);
    return reply;
}
ActionReply DNSCryptClientTestHelper::starttestv2(const QVariantMap args) const
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
    int     testPort = get_key_varmap(args, "port").toInt();
    QString cfg_data = get_key_varmap(args, "cfg_data");

    qint64 code = 0;
    QString entry = readFile("/etc/resolv.conf");
    if ( !entry.startsWith("nameserver 127.0.0.1\nnameserver ::1\n") ) {
        entry.clear();
        entry.append("nameserver 127.0.0.1\nnameserver ::1\n");
        code = writeFile("/etc/resolv.conf", entry);
    };

    QTemporaryFile _tmp_cfg;
    _tmp_cfg.setFileName("/tmp/DNSCryptClientV2.toml");
    _tmp_cfg.setAutoRemove(false);
    _tmp_cfg.open();
    int domain = (servName.endsWith("ipv6") || servName.contains("ip6")) ? 6 : 4;
    QStringList _cfg;
    foreach(QString s, cfg_data.split("\n")) {
        if ( s.startsWith("server_names") ) {
            QStringList _parts = s.split(" = ");
            s.clear();
            s.append(_parts.first());
            s.append(" = ");
            s.append(QString("['%1']").arg(servName));
        };
        if ( s.startsWith("listen_addresses") ) {
            QStringList _parts = s.split(" = ");
            s.clear();
            s.append(_parts.first());
            s.append(" = ");
            // Listen for both domain type (ipv4\ipv6)
            s.append(QString("['127.0.0.1:%1', '[::1]:%1']").arg(testPort));
        };
        _cfg.append(s);
    };
    _tmp_cfg.write(_cfg.join("\n").toUtf8());
    _tmp_cfg.close();

    QVariantMap retdata;
    if ( code != -1 ) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    "org.freedesktop.systemd1",
                    "/org/freedesktop/systemd1",
                    "org.freedesktop.systemd1.Manager",
                    "StartUnit");
        QList<QVariant> _args;
        _args<<QString("DNSCryptClient_test_v2.service")
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
    _tmp_cfg.remove();

    reply.setData(retdata);
    return reply;
}

KAUTH_HELPER_MAIN("pro.russianfedora.dnscryptclienttest", DNSCryptClientTestHelper)
