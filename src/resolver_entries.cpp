#include "resolver_entries.h"

ResolverEntries::ResolverEntries(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Resolver Entries");

    entries = new QListWidget(this);
    ok = new QPushButton("Ok", this);

    dnsEntry = new QLineEdit(this);
    dnsEntry->setPlaceholderText("123.45.67.89");
    addDNS = new QPushButton("Add", this);
    addDNS->setToolTip("Add to list");
    delDNS = new QPushButton("Delete", this);
    delDNS->setToolTip("Delete from list");
    entryLayout = new QHBoxLayout(this);
    entryLayout->addWidget(delDNS);
    entryLayout->addWidget(dnsEntry);
    entryLayout->addWidget(addDNS);
    entryWdg = new QWidget(this);
    entryWdg->setLayout(entryLayout);

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(entries);
    commonLayout->addWidget(entryWdg);
    commonLayout->addWidget(ok, 0, Qt::AlignCenter);
    setLayout(commonLayout);

    connect(entries, SIGNAL(entered(QModelIndex)),
            this, SLOT(_close()));
    connect(ok, SIGNAL(released()),
            this, SLOT(_close()));
    connect(dnsEntry, SIGNAL(returnPressed()),
            this, SLOT(addEntry()));
    connect(addDNS, SIGNAL(released()),
            this, SLOT(addEntry()));
    connect(delDNS, SIGNAL(released()),
            this, SLOT(delEntry()));
}

QString ResolverEntries::getEntry() const
{
    return selectedEntry;
}
void ResolverEntries::setEntries(const QStringList &_entries)
{
    entries->addItems(_entries);
    entries->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
}
QStringList ResolverEntries::getEntries() const
{
    QStringList ret;
    for (int i = 0; i<entries->count(); i++ ) {
        QListWidgetItem *item = entries->item(i);
        if ( item!=nullptr && !item->text().isEmpty() ) {
            ret.append(item->text());
        };
    };
    return ret;
}

/* private slots */
void ResolverEntries::_close()
{
    QList<QListWidgetItem*> items = entries->selectedItems();
    if ( items.count()==0 ) {
        return;
    };
    if ( items.first()!=nullptr ) {
        selectedEntry = items.first()->text();
    };
    done(0);
}
void ResolverEntries::addEntry()
{
    if ( dnsEntry->text().isEmpty() ) return;
    QString _entry = QString("nameserver %1\n").arg(dnsEntry->text());
    entries->addItem(_entry);
    dnsEntry->clear();
}
void ResolverEntries::delEntry()
{
    QList<QListWidgetItem*> items = entries->selectedItems();
    if ( items.count()==0 ) return;
    foreach (QListWidgetItem *item, items) {
        if ( item!=nullptr ) {
            int idx = entries->row(item);
            entries->takeItem(idx);
            delete item;
            item = nullptr;
        };
    };
}
