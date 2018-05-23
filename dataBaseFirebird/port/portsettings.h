#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

namespace Ui {
class PortSettings;
}

class PortSettings : public QDialog
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
    explicit PortSettings(QWidget *parent = 0);
    explicit PortSettings(int addr, QWidget *parent = 0);
    ~PortSettings();
    Settings SettingsPort;
    bool isChanged;

private slots:
    void checkCustomBaudRatePolicy(int idx);
    void onOkClick();
    void onCacnelClick();

signals:
    void settingsIsChanged();

private:
    Ui::PortSettings *ui;
    QStringList port_names;
    void addValueToSettings();
    void getPortSettingsFromFile();
    void getPortsInfo();
    QSettings *port_settings;
};

#endif // PORTSETTINGS_H
