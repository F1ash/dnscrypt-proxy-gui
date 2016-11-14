//#include <QProcess>
#include "dnscrypt_client_helper.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <private/qdbusmetatype_p.h>
#include <private/qdbusutil_p.h>
#include <QFile>

#define CLIENT QString("DNSCryptClient")

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

DNSCryptClientHelper::DNSCryptClientHelper(QObject *parent) :
    QObject(parent)
{
}

QString DNSCryptClientHelper::get_key_varmap(const QVariantMap &args, const QString& key) const
{
    QString value;
    if (args.keys().contains(key)) {
        value = args[key].toString();
    } else {
        value = QString();
    };
    return value;
}
QString DNSCryptClientHelper::readResolvConf() const
{
    QString ret;
    QFile f("/etc/resolv.conf");
    bool opened = f.open(QIODevice::ReadOnly);
    if ( opened ) {
        ret = QString:: fromUtf8( f.readAll() );
        f.close();
    };
    return ret;
}
int DNSCryptClientHelper::writeResolvConf(QString &entry) const
{
    int ret = 0;
    QFile f("/etc/resolv.conf");
    bool opened = f.open(QIODevice::WriteOnly);
    if ( opened ) {
        ret = f.writeData(entry, entry.size());
        f.close();
    } else {
        ret = -1;
    };
    return ret;
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

QVariantList &operator<<(QVariantList &list, const QDBusArgument &argument)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        QDBusVariant item;
        argument >> item;
        list.append(item.variant());
    }
    argument.endArray();

    return list;
}

// Not implemented;
// used persistent DNSCryptClient.service systemd unit
ActionReply DNSCryptClientHelper::create(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="create" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QVariantMap retdata;
    // Start new transient service unit: wvdialer.service

    /*
    QProcess proc;
    proc.setProgram("/usr/bin/systemd-run");
    proc.setArguments(QStringList()
                      <<"--unit"<<CLIENT
                      <<"--service-type"<<"simple"
                      <<"/usr/bin/wvdial");
    proc.start(QIODevice::ReadOnly);
    if ( proc.waitForStarted() && proc.waitForFinished() ) {
        retdata["code"]     = QString::number(proc.exitCode());
    } else {
        retdata["code"]     = QString::number(-1);
    };
    */

    qDBusRegisterMetaType<PrimitivePair>();
    qDBusRegisterMetaType<SrvParameters>();
    qDBusRegisterMetaType<ComplexPair>();
    qDBusRegisterMetaType<AuxParameters>();

    QDBusMessage msg = QDBusMessage::createMethodCall(
                "org.freedesktop.systemd1",
                "/org/freedesktop/systemd1",
                "org.freedesktop.systemd1.Manager",
                "StartTransientUnit");
    // Expecting 'ssa(sv)a(sa(sv))'
    QVariantList  _args;

    SrvParameters _props;
    PrimitivePair srvType, execStart, execStop;
    srvType.name    = "Type";
    srvType.value   = "simple";
    execStart.name  = "ExecStart";
    execStart.value = QLatin1String("/usr/sbin/dnscrypt-proxy");
    execStop.name   = "ExecStop";
    execStop.value  = QLatin1String("/bin/kill -INT ${MAINPID}");
    _props.append(srvType);
    _props.append(execStart);
    _props.append(execStop);

    AuxParameters _aux;
    // aux is currently unused and should be passed as empty array.
    // ComplexPair   srvAuxData;
    //_aux.append(srvAuxData);

    QDBusArgument PROPS, AUX;
    PROPS<<_props;
    AUX<<_aux;

    _args
        << QString("%1.service").arg(CLIENT)
        << "replace";
    _args<< PROPS;
    _args<< AUX;
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
        retdata["code"]     = QString::number(1);
        retdata["err"]      = res.errorMessage();
        break;
    default:
        retdata["code"]     = QString::number(1);
        break;
    };

    reply.setData(retdata);
    return reply;
}

ActionReply DNSCryptClientHelper::start(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="start" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    int code = 0;
    QString entry = readResolvConf();
    if ( entry.startsWith("127.0.0.1\n") ) entry.clear();
    //code = writeResolvConf("127.0.0.1\n");
    QVariantMap retdata;
    if ( code != -1 ) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    "org.freedesktop.systemd1",
                    "/org/freedesktop/systemd1",
                    "org.freedesktop.systemd1.Manager",
                    "StartUnit");
        QList<QVariant> _args;
        _args<<QString("%1.service").arg(CLIENT)<<"fail";
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
            retdata["entry"]    = entry;
            retdata["code"]     = QString::number(0);
            break;
        case QDBusMessage::ErrorMessage:
        default:
            retdata["code"]     = QString::number(1);
            retdata["err"]      = res.errorMessage();
            break;
        };
    } else {
        retdata["msg"]    = "Start failed";
        retdata["code"]   = QString::number(1);
        retdata["err"]    = "Resolv.conf not changed";
    };

    reply.setData(retdata);
    return reply;
}

ActionReply DNSCryptClientHelper::stop(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="stop" ) {
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
    _args<<QString("%1.service").arg(CLIENT)<<"fail";
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
        retdata["code"]     = QString::number(1);
        retdata["err"]      = res.errorMessage();
        break;
    default:
        retdata["code"]     = QString::number(1);
        break;
    };

    reply.setData(retdata);
    return reply;
}

ActionReply DNSCryptClientHelper::restore(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="restore" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    int code = -1;
    QString entry     = args["entry"].toString();
    //code = writeResolvConf(entry);
    QVariantMap retdata;
    if ( code != -1 ) {
        retdata["msg"]    = "Restore down";
    } else {
        retdata["msg"]    = "Restore failed";
    };
    retdata["code"]   = QString::number(code);

    reply.setData(retdata);
    return reply;
}

KAUTH_HELPER_MAIN("pro.russianfedora.dnscrypt_client", DNSCryptClientHelper)
