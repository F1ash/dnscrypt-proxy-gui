#ifndef RESOLVER_ENTRIES_H
#define RESOLVER_ENTRIES_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

class ResolverEntries : public QDialog
{
    Q_OBJECT
public:
    explicit ResolverEntries(QWidget *parent = nullptr);
    QString         getEntry() const;
    void            setEntries(const QStringList);

private:
    QListWidget    *entries;
    QPushButton    *ok;
    QVBoxLayout    *commonLayout;
};

#endif // RESOLVER_ENTRIES_H
