#include "help_thread.h"
#include <QFile>

HelpThread::HelpThread(QObject *parent) :
    QThread(parent)
{

}

void HelpThread::setServerDataMap(const QVariantMap _map)
{
    listOfServers = _map;
}
QString HelpThread::readToNextComma(QString *_str)
{
    QString item;
    if ( _str->startsWith(',') ) {
        // empty item;
    } else if ( !_str->isEmpty() ) {
        item.append( _str->at(0) );
        _str->remove(0, 1);
        item.append(readToNextComma(_str));
    };
    return item;
}
QString HelpThread::readToNextQuotes(QString *_str)
{
    QString item;
    if ( _str->startsWith('"') ) {
        _str->remove(0, 1);
    } else if ( !_str->isEmpty() ) {
        item.append( _str->at(0) );
        _str->remove(0, 1);
        item.append(readToNextQuotes(_str));
    };
    return item;
}
QString HelpThread::readNextItem(QString *_str)
{
    QString item;
    if ( _str->startsWith(',') ) {
        _str->remove(0, 1);
    };
    if ( _str->startsWith('"') ) {
        _str->remove(0, 1);
        item.append(readToNextQuotes(_str));
    } else {
        item.append(readToNextComma(_str));
    };
    if ( item.isEmpty() || item.startsWith("\r") ) {
        item.clear();
        item.append("---");
    };
    return item;
}
void HelpThread::readServerData(QString servData)
{
    // Name,Full name,Description,Location,Coordinates,URL,Version,
    // DNSSEC validation,No logs,Namecoin,Resolver address,
    // Provider name,Provider public key,Provider public key TXT record
    QVariantMap _data;
    {
        _data.insert("Name", readNextItem(&servData));
        _data.insert("FullName", readNextItem(&servData));
        _data.insert("Description", readNextItem(&servData));
        _data.insert("Location", readNextItem(&servData));
        _data.insert("Coordinates", readNextItem(&servData));
        _data.insert("URL", readNextItem(&servData));
        _data.insert("Version", readNextItem(&servData));
        _data.insert("DNSSECvalidation", readNextItem(&servData));
        _data.insert("NoLogs", readNextItem(&servData));
        _data.insert("Namecoin", readNextItem(&servData));
        _data.insert("ResolverAddress", readNextItem(&servData));
        _data.insert("ProviderName", readNextItem(&servData));
        _data.insert("ProviderPublicKey", readNextItem(&servData));
        _data.insert("ProviderPublicKeyTXTrecord", readNextItem(&servData));
    };
    QString _key = _data.value("Name").toString();
    settings.beginGroup("Responds");
    QString respondIconName = settings.value(_key).toString();
    if ( respondIconName.isEmpty() ) respondIconName.append("none");
    settings.endGroup();
    settings.beginGroup("Enables");
    bool state = settings.value(_key, true).toBool();
    settings.endGroup();
    _data.insert("Respond", respondIconName);
    _data.insert("Enable", state);
    emit newDNSCryptSever(_data);
}

void HelpThread::run()
{
    if ( listOfServers.isEmpty() ) {
        QFile f("/usr/share/dnscrypt-proxy/dnscrypt-resolvers.csv");
        bool opened = f.open(QIODevice::ReadOnly);
        if ( opened ) {
            QString _data = QString::fromUtf8(f.readAll());
            QStringList _dataList = _data.split("\n");
            _dataList.removeFirst();
            foreach (QString s, _dataList) {
                if ( s.isEmpty() ) continue;
                readServerData(s);
            };
            f.close();
        };
    } else {
        foreach (QString _key, listOfServers.keys()) {
            QStringList _srvData = listOfServers.value(_key).toStringList();
            QVariantMap _data;
            _data.insert("Name", _key);
            _srvData.removeLast();
            _data.insert("Description", _srvData.join("\n"));
            settings.beginGroup("Responds");
            QString respondIconName = settings.value(_key).toString();
            if ( respondIconName.isEmpty() ) respondIconName.append("none");
            settings.endGroup();
            settings.beginGroup("Enables");
            bool state = settings.value(_key, true).toBool();
            settings.endGroup();
            _data.insert("Respond", respondIconName);
            _data.insert("Enable", state);
            emit newDNSCryptSever(_data);
        };
    }
}
