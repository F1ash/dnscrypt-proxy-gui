#include "port_settings.h"

PortSettings::PortSettings(QWidget *parent, int _port) :
    QWidget(parent)
{
    name = new QLabel(this);
    name->setContentsMargins(0, 0, 0, 0);
    port = new QSpinBox(this);
    port->setRange(0, 65535);
    port->setValue(_port);
    port->setContentsMargins(0, 0, 0, 0);
    commonLayout = new QHBoxLayout(this);
    commonLayout->addWidget(port, 0, Qt::AlignLeft);
    commonLayout->addWidget(name, 0, Qt::AlignLeft);
    setContentsMargins(0, 0, 0, 0);
    setLayout(commonLayout);
    connect(port, SIGNAL(valueChanged(int)),
            this, SIGNAL(valueChanged(int)));
}

void PortSettings::setPort(const int i)
{
    port->setValue(i);
}
void PortSettings::setName(const QString &_name)
{
    name->setText(_name);
}
int  PortSettings::getPort() const
{
    return port->value();
}
