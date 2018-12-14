#ifndef HELP_THREAD_H
#define HELP_THREAD_H

#include <QThread>
#include <QVariantMap>
#include <QSettings>

class HelpThread : public QThread
{
    Q_OBJECT
public:
    explicit HelpThread(QObject *parent = nullptr);
    void            setServerDataMap(const QVariantMap);

signals:
    void            newDNSCryptSever(const QVariantMap&);

private:
    QSettings       settings;
    QString         readToNextComma(QString*);
    QString         readToNextQuotes(QString*);
    QString         readNextItem(QString*);
    void            readServerData(QString);
    QVariantMap     listOfServers;

private slots:
    void            run();
};

#endif // HELP_THREAD_H
