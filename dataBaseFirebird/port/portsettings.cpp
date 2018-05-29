#include "portsettings.h"
#include "ui_portsettings.h"
#include <QFile>
#include <QTextStream>

PortSettings::PortSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettings)
{
    ui->setupUi(this);
    addValueToSettings();
    getPortsInfo();
    ui->leAddres->setValidator(new QIntValidator(0, 100000, this));
    connect(ui->cbBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onOkClick()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(onCacnelClick()));
    port_settings = new QSettings("settings.conf", QSettings::IniFormat);
    getPortSettingsFromFile();
}

PortSettings::PortSettings(int addr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettings)
{
    ui->setupUi(this);
    addValueToSettings();
    getPortsInfo();
    ui->leAddres->setValidator(new QIntValidator(0, 100000, this));
    connect(ui->cbBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onOkClick()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(onCacnelClick()));
    port_settings = new QSettings("settings.conf", QSettings::IniFormat);
    getPortSettingsFromFile();
    ui->leAddres->setText(QString::number(addr));
}

PortSettings::PortSettings(QString ad, QString br, QString bop, QString db, QString fc, QString na, QString pa, QString sb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettings)
{
    ui->setupUi(this);
    addValueToSettings();
    getPortsInfo();
    ui->leAddres->setValidator(new QIntValidator(0, 100000, this));
    connect(ui->cbBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onOkClick2()));

    ui->cbPortName->setCurrentText(na);
    ui->cbBaudRate->setCurrentText(br);
    ui->cbDataBist->setCurrentText(db);
    ui->cbPairity->setCurrentText(pa);
    ui->cbStopBits->setCurrentText(sb);
    ui->cbFlowControl->setCurrentText(fc);
    ui->leAddres->setText(ad);
    ui->leByteInPacket->setText(bop);


    SettingsPort.name = ui->cbPortName->itemText(ui->cbPortName->currentIndex());
    SettingsPort.baudRate = (QSerialPort::BaudRate) ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()).toInt();
    SettingsPort.dataBits = (QSerialPort::DataBits) ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()).toInt();
    SettingsPort.parity = (QSerialPort::Parity) ui->cbPairity->itemText(ui->cbPairity->currentIndex()).toInt();
    SettingsPort.stopBits = (QSerialPort::StopBits) ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()).toInt();
    SettingsPort.flowControl = (QSerialPort::FlowControl) ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()).toInt();
    SettingsPort.addres = ui->leAddres->text();
    SettingsPort.byteOnPackage = ui->leByteInPacket->text().toInt();
}

PortSettings::~PortSettings()
{
    delete ui;
}

void PortSettings::addValueToSettings() {
    //data bits
    ui->cbDataBist->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui->cbDataBist->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui->cbDataBist->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui->cbDataBist->addItem(QLatin1String("8"), QSerialPort::Data8);
    ui->cbDataBist->setCurrentIndex(3);
    //baud rate
    ui->cbBaudRate->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->cbBaudRate->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->cbBaudRate->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->cbBaudRate->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->cbBaudRate->addItem(QLatin1String("Custom"));
    //fill party
    ui->cbPairity->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui->cbPairity->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui->cbPairity->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui->cbPairity->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);
    ui->cbPairity->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
    //fill stop bits
    ui->cbStopBits->addItem(QLatin1String("1"), QSerialPort::OneStop);
    #ifdef Q_OS_WIN
    ui->cbStopBits->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
    #endif
    ui->cbStopBits->addItem(QLatin1String("2"), QSerialPort::TwoStop);
    //fill flow control
    ui->cbFlowControl->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui->cbFlowControl->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui->cbFlowControl->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);
}

void PortSettings::checkCustomBaudRatePolicy(int idx) {
    bool isCustomBaudRate = !ui->cbBaudRate->itemData(idx).isValid();
    ui->cbBaudRate->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->cbBaudRate->clearEditText();
    }
}

void PortSettings::getPortSettingsFromFile() {
    QFile file("settings.ini");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split("=", QString::SkipEmptyParts);
        if (list.length() == 1) continue;
        if (list[0] == "ComPort" && ui->cbPortName->findText(list[1]) + 1) {
            ui->cbPortName->setCurrentText(list[1]);
            continue;
        }
        if (list[0] == "BaudRate" && ui->cbBaudRate->findText(list[1]) + 1) {
            ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText(list[1]));
            continue;
        }
        if (list[0] == "DataBits" && ui->cbDataBist->findText(list[1]) + 1) {
            ui->cbDataBist->setCurrentIndex(ui->cbDataBist->findText(list[1]));
            continue;
        }
        if (list[0] == "Pairity" && ui->cbPairity->findText(list[1]) + 1) {
            ui->cbPairity->setCurrentIndex(ui->cbPairity->findText(list[1]));
            continue;
        }
        if (list[0] == "StopBits" && ui->cbStopBits->findText(list[1]) + 1) {
            ui->cbStopBits->setCurrentIndex(ui->cbStopBits->findText(list[1]));
            continue;
        }
        if (list[0] == "FlowControl" && ui->cbFlowControl->findText(list[1]) + 1) {
            ui->cbFlowControl->setCurrentIndex(ui->cbFlowControl->findText(list[1]));
            continue;
        }
        if (list[0] == "Addres") {
            ui->leAddres->setText(list[1]);
            continue;
        }
        if (list[0] == "ByteInPacket") {
            ui->leByteInPacket->setText(list[1]);
            continue;
        }
    }
    file.close();

    SettingsPort.name = ui->cbPortName->itemText(ui->cbPortName->currentIndex());
    SettingsPort.baudRate = (QSerialPort::BaudRate) ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()).toInt();
    SettingsPort.dataBits = (QSerialPort::DataBits) ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()).toInt();
    SettingsPort.parity = (QSerialPort::Parity) ui->cbPairity->itemText(ui->cbPairity->currentIndex()).toInt();
    SettingsPort.stopBits = (QSerialPort::StopBits) ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()).toInt();
    SettingsPort.flowControl = (QSerialPort::FlowControl) ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()).toInt();
    SettingsPort.addres = ui->leAddres->text();
    SettingsPort.byteOnPackage = ui->leByteInPacket->text().toInt();

    isChanged = true;
}

void PortSettings::onCacnelClick() {
    isChanged = false;
}

void PortSettings::onOkClick2() {
    SettingsPort.name = ui->cbPortName->itemText(ui->cbPortName->currentIndex());
    SettingsPort.baudRate = (QSerialPort::BaudRate) ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()).toInt();
    SettingsPort.dataBits = (QSerialPort::DataBits) ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()).toInt();
    SettingsPort.parity = (QSerialPort::Parity) ui->cbPairity->itemText(ui->cbPairity->currentIndex()).toInt();
    SettingsPort.stopBits = (QSerialPort::StopBits) ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()).toInt();
    SettingsPort.flowControl = (QSerialPort::FlowControl) ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()).toInt();
    SettingsPort.addres = ui->leAddres->text();
    SettingsPort.byteOnPackage = ui->leByteInPacket->text().toInt();

    emit settingsIsChanged();
}

void PortSettings::onOkClick() {
    SettingsPort.name = ui->cbPortName->itemText(ui->cbPortName->currentIndex());
    SettingsPort.baudRate = (QSerialPort::BaudRate) ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()).toInt();
    SettingsPort.dataBits = (QSerialPort::DataBits) ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()).toInt();
    SettingsPort.parity = (QSerialPort::Parity) ui->cbPairity->itemText(ui->cbPairity->currentIndex()).toInt();
    SettingsPort.stopBits = (QSerialPort::StopBits) ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()).toInt();
    SettingsPort.flowControl = (QSerialPort::FlowControl) ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()).toInt();
    SettingsPort.addres = ui->leAddres->text();

    bool ok;
    int onPackage = ui->leAddres->text().toInt(&ok);
    SettingsPort.byteOnPackage = (ok && onPackage < 256 && onPackage > 0) ? onPackage : 100;

    port_settings->beginGroup("PortSettings");
    port_settings->setValue("comPort", SettingsPort.name);
    port_settings->setValue("baudRate", ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()));
    port_settings->setValue("dataBits", ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()));
    port_settings->setValue("pairity", ui->cbPairity->itemText(ui->cbPairity->currentIndex()));
    port_settings->setValue("stopBits", ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()));
    port_settings->setValue("flowControl", ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()));
    port_settings->setValue("address", ui->leAddres->text());
    port_settings->setValue("byteInPacket", ui->leByteInPacket->text());
    port_settings->endGroup();
    port_settings->sync();

    QFile file("settings.ini");
    file.open(QIODevice::ReadWrite);

    QTextStream out(&file);
    out << "[Baseparams]" << "\n";
    out << "ComPort=" << SettingsPort.name << "\n";
    out << "BaudRate=" << ui->cbBaudRate->itemText(ui->cbBaudRate->currentIndex()) << "\n";
    out << "DataBits=" << ui->cbDataBist->itemText(ui->cbDataBist->currentIndex()) << "\n";
    out << "Pairity=" << ui->cbPairity->itemText(ui->cbPairity->currentIndex()) << "\n";
    out << "StopBits=" << ui->cbStopBits->itemText(ui->cbStopBits->currentIndex()) << "\n";
    out << "FlowControl=" << ui->cbFlowControl->itemText(ui->cbFlowControl->currentIndex()) << "\n";
    out << "Addres=" << ui->leAddres->text() << "\n";
    out << "ByteInPacket=" << ui->leByteInPacket->text() << "\n";
    file.close();

    isChanged = true;
    emit settingsIsChanged();
}

void PortSettings::getPortsInfo()
{
    ui->cbPortName->clear();
    port_names.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        port_names.push_back(info.portName());
    }
    ui->cbPortName->addItems(port_names);
}
