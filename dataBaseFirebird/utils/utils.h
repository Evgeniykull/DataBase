#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QSerialPort>

struct Settings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
    QString addres;
    int byteOnPackage;
};

#endif // UTILS_H
