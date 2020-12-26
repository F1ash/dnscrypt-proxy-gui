extern "C" {
#include "dns.h"
}
#include "dnscrypt_client_reload_helper.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <private/qdbusmetatype_p.h>
#include <private/qdbusutil_p.h>
#include <QFile>
//#include <QTextStream>

#define UnitName QString("DNSCryptClient")

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
QString changeUnit(const QString &entry, const QString &_port,
                   bool asUser = false, const QString &_User = "")
{
    //ExecStart=/usr/sbin/dnscrypt-proxy -a 127.0.0.1:53 [-u username] -R %i
    //QTextStream s(stdout);
    bool entryChanged = false;
    QStringList _strings = entry.split("\n");
    QStringList changedEntry;
    foreach (QString _str, _strings) {
        QString changedString;
        if ( _str.startsWith("ExecStart") ) {
            QStringList _parts = _str.split(" ");
            QStringList _changedParts;
            if ( _parts.count()>1 ) {
                QString _comm("ExecStart=/usr/sbin/dnscrypt-proxy");
                QString _addrKey("-a");
                QString _addr;
                QString _userKey("-u");
                QString _suff("-R %i");
                _changedParts.append(_comm);
                _changedParts.append(_addrKey);
                QStringList _addrParts = _parts.at(2).split(":");
                if ( _addrParts.count()>1 && !_addrParts.at(1).isEmpty() ) {
                    if ( _addrParts.at(1)!=_port ) {
                        _addr.append("127.0.0.1");
                        _addr.append(":");
                        _addr.append(_port);
                        entryChanged = true;
                    } else {
                        _addr.append(_parts.at(2));
                    };
                } else {
                    _addr.append("127.0.0.1:53");
                    entryChanged = true;
                };
                _changedParts.append(_addr);
                if ( asUser ) {
                    if ( _parts.contains(_userKey) ) {
                        _changedParts.append(_userKey);
                        if ( _parts.at(4)!=_User ) {
                            entryChanged = true;
                        };
                        _changedParts.append(_User);
                    } else {
                        _changedParts.append(_userKey);
                        _changedParts.append(_User);
                        entryChanged = true;
                    };
                } else {
                    if ( _parts.contains("-u") ) {
                        entryChanged = true;
                    };
                };
                _changedParts.append(_suff);
                changedString.append(_changedParts.join(" "));
            } else {
                changedString.append(_str);
            };
        } else {
            changedString.append(_str);
        };
        changedEntry.append(changedString);
    };
    return ( entryChanged ) ? changedEntry.join("\n") : QString();
}

DNSCryptClientReloadHelper::DNSCryptClientReloadHelper(QObject *parent) :
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

ActionReply DNSCryptClientReloadHelper::setunits(const QVariantMap args) const
{
    ActionReply reply;

    const QString act = get_key_varmap(args, "action");
    if ( act!="setUnits" ) {
        QVariantMap err;
        err["result"] = QString::number(-1);
        reply.setData(err);
        return reply;
    };

    QString unitPath("/usr/lib/systemd/system/");
    QString jobUnitPath = QString("%1%2@.service")
            .arg(unitPath).arg(UnitName);
    QString testUnitPath = QString("%1%2_test@.service")
            .arg(unitPath).arg(UnitName);

    QString serviceVersion = get_key_varmap(args, "version");

    QString _User = get_key_varmap(args, "User");
    bool asUser = !_User.isEmpty();

    QString jobPort = get_key_varmap(args, "JobPort");
    QString jobUnitText = changeUnit(
                readFile(jobUnitPath), jobPort,
                asUser, _User);

    QString testPort = get_key_varmap(args, "TestPort");
    QString testUnitText = changeUnit(
                readFile(testUnitPath), testPort);

    QVariantMap retdata;
    if ( serviceVersion.compare("2")<0 && !jobUnitText.isEmpty() ) {
        retdata["jobUnit"]      = QString::number(
                    writeFile(jobUnitPath, jobUnitText));
        // reload unit
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    "org.freedesktop.systemd1",
                    "/org/freedesktop/systemd1",
                    "org.freedesktop.systemd1.Manager",
                    "ReloadUnit");
        QList<QVariant> _args;
        _args<<QString("%1@.service").arg(UnitName)
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
        switch (res.type()) {
        case QDBusMessage::ReplyMessage:
            retdata["code"]     = QString::number(0);
            break;
        case QDBusMessage::ErrorMessage:
        default:
            retdata["code"]     = QString::number(-1);
            retdata["err"]      = res.errorMessage();
            break;
        };
    } else {
        // For version>=2 nothing to change in service unit file,
        // because it has new changes at each job iteraton
        retdata["code"]         = QString::number(1);
        retdata["jobUnit"]      = jobPort;
    };
    if ( serviceVersion.compare("2")<0 && !testUnitText.isEmpty() ) {
        retdata["testUnit"]     = QString::number(
                    writeFile(testUnitPath, testUnitText));
        // reload unit
        QDBusMessage msg = QDBusMessage::createMethodCall(
                    "org.freedesktop.systemd1",
                    "/org/freedesktop/systemd1",
                    "org.freedesktop.systemd1.Manager",
                    "ReloadUnit");
        QList<QVariant> _args;
        _args<<QString("%1_test@.service").arg(UnitName)
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
        switch (res.type()) {
        case QDBusMessage::ReplyMessage:
            retdata["code"]     = QString::number(0);
            break;
        case QDBusMessage::ErrorMessage:
        default:
            retdata["code"]     = QString::number(-1);
            retdata["err"]      = res.errorMessage();
            break;
        };
    } else {
        // For version>=2 nothing to change in test unit file,
        // because it created new at each test iteration
        retdata["code"]         = QString::number(1);
        retdata["testUnit"]     = testPort;
    };
    //retdata["jobUnitText"]      = jobUnitText;
    //retdata["testUnitText"]     = testUnitText;

    reply.setData(retdata);
    return reply;
}

KAUTH_HELPER_MAIN("pro.russianfedora.dnscryptclientreload", DNSCryptClientReloadHelper)
