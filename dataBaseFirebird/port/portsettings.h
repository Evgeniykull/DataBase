#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <utils/utils.h>

namespace Ui {
class PortSettings;
}

class PortSettings : public QDialog
{
    Q_OBJECT

public:
    explicit PortSettings(QWidget *parent = 0);
    explicit PortSettings(int addr, QWidget *parent = 0);
    explicit PortSettings(QString,QString,QString,QString,QString,QString,QString,QString,QWidget *parent = 0);

    ~PortSettings();
    Settings SettingsPort;
    bool isChanged;

private slots:
    void checkCustomBaudRatePolicy(int idx);
    void onOkClick();
    void onOkClick2();
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
