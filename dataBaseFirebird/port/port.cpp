#include "port.h"
#include "ui_port.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QTextCodec>
#include <QDebug>
#include "utils/treadworker.h"
#include "utils/catalogswrither.h"

Port::Port(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Port)
{
    ui->setupUi(this);
    port = new QSerialPort(this);
    port_settings = new PortSettings();
    rewriteSettings(); //начальная установка настроек
    //при изменение настроек срабатывает сигнал settingsIsChanged
    connect(port_settings, SIGNAL(settingsIsChanged()), SLOT(rewriteSettings()));
    connect(ui->pbSettings, SIGNAL(clicked(bool)), SLOT(onSettingsClick()));
}

Port::Port(int port_addr, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Port)
{
    ui->setupUi(this);
    port = new QSerialPort(this);
    port_settings = new PortSettings(port_addr);
    rewriteSettings(port_addr); //начальная установка настроек
    //при изменение настроек срабатывает сигнал settingsIsChanged
    connect(port_settings, SIGNAL(settingsIsChanged()), SLOT(rewriteSettings()));
    connect(ui->pbSettings, SIGNAL(clicked(bool)), SLOT(onSettingsClick()));
}

Port::Port(PortSettings *ps, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::Port)
{
    ui->setupUi(this);
    port = new QSerialPort(this);
    port_settings = ps;

    sett.name = port_settings->SettingsPort.name;
    sett.baudRate = port_settings->SettingsPort.baudRate;
    sett.dataBits = port_settings->SettingsPort.dataBits;
    sett.parity = port_settings->SettingsPort.parity;
    sett.stopBits = port_settings->SettingsPort.stopBits;
    sett.flowControl = port_settings->SettingsPort.flowControl;
    sett.byteOnPackage = port_settings->SettingsPort.byteOnPackage;
    ui->lblPortName->setText(sett.name);
    sett.addres = port_settings->SettingsPort.addres;
}

Port::~Port()
{
    closePort();  // На всякий случай и для возможности раннего выхода при использовании статического создания
    delete ui;
}

void Port::onSettingsClick() {
    port_settings->exec();
}

void Port::rewriteSettings(int addr) {
    if (port_settings->isChanged) {
        sett.name = port_settings->SettingsPort.name;
        sett.baudRate = port_settings->SettingsPort.baudRate;
        sett.dataBits = port_settings->SettingsPort.dataBits;
        sett.parity = port_settings->SettingsPort.parity;
        sett.stopBits = port_settings->SettingsPort.stopBits;
        sett.flowControl = port_settings->SettingsPort.flowControl;
        sett.byteOnPackage = port_settings->SettingsPort.byteOnPackage;
        ui->lblPortName->setText(sett.name);

        if (addr > -1) {
            sett.addres = addr;
        } else {
            sett.addres = port_settings->SettingsPort.addres;
        }
    }
    openPort();
    closePort();
}

bool Port::changeAccess(int level,QString passwd) {
    QByteArray req_acc_level = "ReqAccLevel:" + QByteArray::number(level);
    QByteArray alpassword = "ALPassword:"+passwd.toUtf8();
    QByteArray req = "run AccessCmd:{\n" + req_acc_level + "\n";
    if (level) {
        req += alpassword;
    }
    req += "}";

    QByteArray req_data = writeData(req).toUtf8();
    req_data = writeData("get AccessCmd").toUtf8();

    int pos = req_data.indexOf("CurAccLevel:")+12;
    while ((req_data[pos]<'0')&&(pos<req_data.length())) pos++;
    int curlev=(req_data[pos]-'0');
    if (curlev!=level) {
      QMessageBox::information(0, QObject::tr("Изменение режима доступа"), QString(req_data));
      return 0;
    }
    return 1;
}

int Port::openPort()
{
  int result=0;
    //writePortSettings(); //просто перезаписать настройки -- достаточно один раз делать! либо при их изменений
    port->setPortName(sett.name);
    if(port->isOpen()) {
        return 1;
    }
    if (port->open(QIODevice::ReadWrite)) {
        if (port->setBaudRate(sett.baudRate)
                && port->setDataBits(sett.dataBits)
                && port->setParity(sett.parity)
                && port->setStopBits(sett.stopBits)
                && port->setFlowControl(sett.flowControl)) {
            if (port->isOpen()) {
               ui->lblColour->setStyleSheet("QLabel { background-color : green; }");
               result=1;
            }
        } else {
            port->close();
            ui->lblColour->setStyleSheet("QLabel { background-color : red; }");
        }
    } else {
        port->close();
        ui->lblColour->setStyleSheet("QLabel { background-color : red; }");
    }
    return result;
}

void Port::closePort()
{
    if (port->isOpen()) {
        port->close();
    }
    transfer_data = false;
}

// Открывает порт (возвращает 1 если всё нормально)
bool Port::StartComm() {
  openPort();
  if (!port->isOpen()) {
      QMessageBox::information(0, QObject::tr("Невозможно записать на устройство"), QObject::tr("Порт закрыт"));
      return false;
  }
  return true;
};

// Закрывает порт
void Port::EndComm() {
  closePort();
}


/*void Port::sendData(QByteArray data) {
    QThread *thread = new QThread;
    treadWorker *worker = new treadWorker();
    worker->setTransferParams(data, port, sett.addres.toInt(), sett.byteOnPackage);

    connect(thread, SIGNAL(started()), worker, SLOT(transferData()));
    connect(worker, SIGNAL(finished(QByteArray)), this, SLOT(endTransmitData(QByteArray)));
    worker->moveToThread(thread);
    thread->start();
}*/

QByteArray Port::getPortDataOnly(int len) {
  QByteArray qba;
  int tosumm=0;  // Счетчик общего времени ожидания
  int timeout;
  timeout=len;
  if (timeout<40) timeout=40;
  qba.clear();
  tosumm=0;
  while ((qba.length()<len)&&(tosumm<timeout)) {    // Здесь - время межбайтового таймаута
    int llen=qba.length();
    int crn=len-llen;
    QByteArray readed;
    readed=port->read(crn);
    if (readed.length()==0) {
      port->waitForReadyRead(5);  // Ожидаем - если ничего не пришло ещё
      tosumm+=5;
    } else {
      qDebug() << " GPDO " << readed.length();
      qba.append(readed);
      tosumm=0;
    }
  }
  if (tosumm>=timeout) qDebug() << " GPDO Timeout";
  return qba;
}

QByteArray Port::getPortData(char prebyte,int len) {
  QByteArray qba;
  int tosumm=0;  // Счетчик общего времени ожидания
  qba.clear();
// Ожидаем начальный байт пакета
  do {
    port->waitForReadyRead(10);
    qba.append(port->read(1));
// Считываем по одному байту до тех пор, пока не найдем преамбулу
// До преамбулы может быть мусор - его откидываем
    if (qba.length()<1) {
      tosumm+=10;
    } else if (qba[0]!=prebyte) {
      qba.clear();
    }
  } while ((qba.length()==0)&&(tosumm<200)); // Здесь - время в миллисекундах максимального ожидания ответа
  if (qba.length()!=0) { // Преамбула пришла - иначе ответа нет
    tosumm=0;
    while ((qba.length()<len)&&(tosumm<50)) {    // Здесь - время межбайтового таймаута
      int llen=qba.length();
      int crn=len-llen;
      QByteArray readed;
      readed=port->read(crn);
      if (readed.length()==0) {
        port->waitForReadyRead(5);
        tosumm+=5;
      } else {
        qba.append(readed);
        tosumm=0;
      }
    }
  }
  return qba;
}

void Port::putPortData(QByteArray tr_data) {
  port->flush();
  port->write(tr_data);
}

QString Port::write(QString text) {
  return intWriteData(text);
}

QString Port::writeData(QString text) {
  return intWriteData(text);
}

void Port::Add_hCRC(QByteArray * data) {
  uint n;
  ushort crc = 0xFFFF;
  ushort dt;
  uint len;
  len=data->length();
  for (n = 1; n < len; n++) {
      dt=(ushort)(*data)[n];
      dt&=0xFF;
      crc += dt*44111;
      crc = crc ^ (crc >> 8);
  }
  data->append(crc&0xFF);
  data->append((crc>>8)&0xFF);
}

int Port::Check_hCRC(QByteArray data) {
  uint n;
  ushort crc = 0xFFFF;
  ushort crc2;
  ushort dt;
  uint len;
  len=data.length()-2;
  for (n = 1; n < len; n++) {
      dt=(ushort)data[n];
      dt&=0xFF;
      crc += dt*44111;
      crc = crc ^ (crc >> 8);
  }
  crc2=((ushort)data[n+1])&0x00FF;
  crc2<<=8;
  crc2+=((ushort)data[n])&0x00FF;
  return crc==crc2;
}

QString Port::intWriteData(QString text) {
  unsigned char addres = sett.addres.toInt();
  int allerrnum=0;
  int n;
  QByteArray senddata;
  QByteArray rd_data;
//  QByteArray getdata;
// Это здесь делать некорректно, логика неправильная - и тем более возвращать пустую строку
  if (transfer_data) {
    errcode=0x89;  // Коды ошибок нужно делать разные, чтобы было понятно что к чему (хотя бы нам, а лучше и пользователю)
    return "";
  } else {
    transfer_data = true;
  }
// Перекодировали в CP1251 - хорошо
  QTextCodec *codec1 = QTextCodec::codecForName( "CP1251" );
  text = codec1->fromUnicode(QString(text));
//Подготавливаем данные - при пустой строке возвращаем пустую строку - логично.
  QString data = text;
  int data_len = data.length();
  if (data_len == 0) {
    transfer_data=false;
    return "";
  }
  do {
// формируем и передаем пакет запроса
    senddata.clear();
    senddata.append(0xB5);
    senddata.append(addres);
    senddata.append(data_len&0x00FF);
    senddata.append((data_len>>8)&0xFF);
    senddata.append(data);
    Add_hCRC(&senddata);
    putPortData(senddata);
// Процесс пошел. Ждём ответа
    rd_data=getPortData(0xB5,4);
    if (rd_data.length()) {
//      getdata=rd_data;
      rd_data.append(getPortDataOnly((int)((unsigned char)rd_data[2])+(((int)((unsigned char)rd_data[3]))*256)+2));
      if (Check_hCRC(rd_data)) {
        if ((((unsigned char)(rd_data[1]&0x7F)==addres)||(addres==0))&&(rd_data[1]&0x80)) {
          rd_data.remove(0,4);
          rd_data.remove(rd_data.length()-2,2);
          errcode=0;
          transfer_data=false;
          rd_data.append((char)0);
          QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
          QString data_1 = codec->toUnicode(rd_data);  //то, что пришло в формате читаемой строки
          return data_1;
        }
      }
    }
    allerrnum++;
  } while (allerrnum<3);
  errcode=0x88;
  transfer_data=false;
  return "";
}
