#ifndef PORT_H
#define PORT_H

#include <QGroupBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPort>
#include "portsettings.h"

namespace Ui {
class Port;
}

class Port : public QGroupBox
{
    Q_OBJECT

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

public:
    explicit Port(QWidget *parent = 0);
    explicit Port(int port_addr, QWidget *parent = 0);
    ~Port();
    void sendData(QByteArray);
    QByteArray writeData(QByteArray text);
    bool changeAccess(int);

private slots:
    void onSettingsClick();
    void rewriteSettings(int addr = -1);
    void onChangeAccessClick();
    void onAccessUpdateClick();

private:
    Ui::Port *ui;
    PortSettings *port_settings;
    Settings sett;
    QSerialPort *port;
    void closePort();
    void openPort();
    QByteArray getPortDataOnly(int);
    QByteArray getPortData(char,int);
    void putPortData(QByteArray);

    bool transfer_data = false; //если true, то запрет на передачу
    unsigned char buff[255];
    unsigned char errnum;  // Количество ошибочных обменов
    unsigned char errcode; // Код ошибки обмена
    unsigned char answc;   // Код ответа
};

#endif // PORT_H
