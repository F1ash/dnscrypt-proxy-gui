#ifndef PORT_SETTINGS_H
#define PORT_SETTINGS_H

#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>

class PortSettings : public QWidget
{
    Q_OBJECT
public:
    explicit PortSettings(QWidget *parent = nullptr,
                          int      _port  = 0);
    void         setPort(const int);
    void         setName(const QString&);
    int          getPort() const;

signals:
    void         valueChanged(int);

private:
    QLabel      *name;
    QSpinBox    *port;
    QHBoxLayout *commonLayout;
};

#endif // PORT_SETTINGS_H
