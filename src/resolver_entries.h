#ifndef RESOLVER_ENTRIES_H
#define RESOLVER_ENTRIES_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

class ResolverEntries : public QDialog
{
    Q_OBJECT
public:
    explicit ResolverEntries(QWidget *parent = Q_NULLPTR);
    QString         getEntry() const;
    void            setEntries(const QStringList&);
    QStringList     getEntries() const;

private:
    QListWidget    *entries;
    QPushButton    *ok;
    QLineEdit      *dnsEntry;
    QPushButton    *addDNS, *delDNS;
    QHBoxLayout    *entryLayout;
    QWidget        *entryWdg;
    QVBoxLayout    *commonLayout;
    QString         selectedEntry;

private slots:
    void            _close();
    void            addEntry();
    void            delEntry();
};

#endif // RESOLVER_ENTRIES_H
