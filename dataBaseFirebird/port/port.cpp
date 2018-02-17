#include "port.h"
#include "ui_port.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QThread>
#include <QTextCodec>
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
    connect(ui->pbSetAccess, SIGNAL(clicked(bool)), SLOT(onChangeAccessClick()));
    connect(ui->pbUpdateAccess, SIGNAL(clicked(bool)), SLOT(onAccessUpdateClick()));
}

Port::~Port()
{
    delete ui;
}

void Port::onSettingsClick() {
    port_settings->exec();
}

void Port::rewriteSettings() {
    if (port_settings->isChanged) {
        sett.name = port_settings->SettingsPort.name;
        sett.baudRate = port_settings->SettingsPort.baudRate;
        sett.dataBits = port_settings->SettingsPort.dataBits;
        sett.parity = port_settings->SettingsPort.parity;
        sett.stopBits = port_settings->SettingsPort.stopBits;
        sett.flowControl = port_settings->SettingsPort.flowControl;
        sett.addres = port_settings->SettingsPort.addres;
        sett.byteOnPackage = port_settings->SettingsPort.byteOnPackage;
        ui->lblPortName->setText(sett.name);
    }
    openPort();
    closePort();
}

void Port::onChangeAccessClick() {
    openPort();
    if (!port->isOpen()) {
        QMessageBox::information(0, QObject::tr("Невозможно записать на устройство"), QObject::tr("Порт закрыт"));
        return;
    }

    int acc_level = ui->cbAccess->currentIndex() - 1;
    QByteArray req_acc_level = "ReqAccLevel:" + QByteArray::number(acc_level);
    QByteArray alpassword = "ALPassword:";

    if (acc_level > 0) {
        bool ok;
        QString password = QInputDialog::getText(0, "Изменение режима доступа", "Введите пароль:",
                                                  QLineEdit::Normal, "", &ok);
        alpassword += password.toUtf8();
        if (!ok) {
            return;
        }
    }

    QByteArray req = "run AccessCmd:{\n" + req_acc_level + "\n";
    if (acc_level) {
        req += alpassword;
    }
    req += "}";

    QByteArray req_data = writeData(req);
    req_data = writeData("get AccessCmd");

    //распарсить
    //достать ALMessage и вывести
    int pos = req_data.indexOf("ALMessage:");
    QString answ_message = QString(req_data).mid(pos + 11);
    pos = answ_message.indexOf('"');
    answ_message = answ_message.mid(0, pos);

    if (answ_message != "Ok") {
        QMessageBox::information(0, QObject::tr("Изменение режима доступа"), QString(req_data));
    }

    closePort();
}

void Port::onAccessUpdateClick() {
    openPort();
    if (!port->isOpen()) {
        QMessageBox::information(0, QObject::tr("Невозможно записать на устройство"), QObject::tr("Порт закрыт"));
        return;
    }

    QByteArray req_data = writeData("get AccessCmd");
    int pos = req_data.indexOf("CurAccLevel:");
    QString answ_message = QString(req_data).mid(pos + 12, 1);

    ui->cbAccess->setCurrentIndex(answ_message.toInt() + 1);
    closePort();
}

void Port::openPort()
{
    //writePortSettings(); //просто перезаписать настройки -- достаточно один раз делать! либо при их изменений
    port->setPortName(sett.name);
    if(port->isOpen()) {
        return;
    }
    if (port->open(QIODevice::ReadWrite)) {
        if (port->setBaudRate(sett.baudRate)
                && port->setDataBits(sett.dataBits)
                && port->setParity(sett.parity)
                && port->setStopBits(sett.stopBits)
                && port->setFlowControl(sett.flowControl)) {
            if (port->isOpen()) {
                ui->lblColour->setStyleSheet("QLabel { background-color : green; }");
            }
        } else {
            port->close();
            ui->lblColour->setStyleSheet("QLabel { background-color : red; }");
        }
    } else {
        port->close();
        ui->lblColour->setStyleSheet("QLabel { background-color : red; }");
    }
}

void Port::closePort()
{
    if (port->isOpen()) {
        port->close();
    }
}

void Port::sendData(QByteArray data) {
    QThread *thread = new QThread;
    treadWorker *worker = new treadWorker();
    worker->setTransferParams(data, port, sett.addres.toInt(), sett.byteOnPackage);

    connect(thread, SIGNAL(started()), worker, SLOT(transferData()));
    connect(worker, SIGNAL(finished(QByteArray)), this, SLOT(endTransmitData(QByteArray)));
    worker->moveToThread(thread);
    thread->start();
}

QByteArray Port::getPortDataOnly(int len) {
  QByteArray qba;
  int tosumm=0;  // Счетчик общего времени ожидания
  qba.clear();
  tosumm=0;
  while ((qba.length()<len)&&(tosumm<40)) {    // Здесь - время межбайтового таймаута
    int llen=qba.length();
    int crn=len-llen;
    QByteArray readed;
    readed=port->read(crn);
    if (readed.length()==0) {
      port->waitForReadyRead(5);  // Ожидаем - если ничего не пришло ещё
      tosumm+=5;
    } else {
      qba.append(readed);
      tosumm=0;
    }
  }
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
    if ((qba.length()<1)?(1):(qba[0]!=prebyte)) {
      qba.clear();
      tosumm+=10;
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

//пзц
QByteArray Port::writeData(QByteArray text)
{
    openPort();
// fromAlex - это здесь делать некорректно, логика неправильная - и тем более возвращать пустую строку
// лучше тогда проверку делать в самом начале процедур, использующих запись, и либо блокировать выполнение
// до освобождения, либо выыдавать сообщение. Данную ветвь можно оставить только как резервную,
// чтобы в случае оплошности не было диких глюков. Но тогда НУЖНО установить errcode - чтобы было понятно что случилось
    //TODO вернуть
    /*if (transfer_data) {
        errcode=0x89;  // Коды ошибок нужно делать разные, чтобы было понятно что к чему (хотя бы нам, а лучше и пользователю)
        return "";
    } else {
        transfer_data = true;
    }*/
// Перекодировали в CP1251 - хорошо
    QTextCodec *codec1 = QTextCodec::codecForName( "CP1251" );
    text = codec1->fromUnicode(QString(text));
//Подготавливаем данные - при пустой строке возвращаем пустую строку - логично
    QByteArray data = text;
    int data_len = data.length();
    if (data_len == 0) { return "";  }
// Объявления переменных... надеюсь, современные компиляторы позволяют их делать в середине. Мне мой не позволяет
    int iter;
    unsigned char addres = sett.addres.toInt();
    unsigned char tranz_num = 0;
    uint data_pos = 0;
    uint max_pkg_len;
    unsigned char answ_kadr;
//    (data_len < 16) ? max_pkg_len = 16 : max_pkg_len = byteOnPackage;  - неожиданное применение, мне понравилось - но я привык к обычной записи
// fromAlex - Вообще назначение данной конструкции не очень понятно - думаю, логичнее провреять byteOnPackage
    if (sett.byteOnPackage<16) max_pkg_len=16; else max_pkg_len = sett.byteOnPackage;
//
    while (data_len) {
// Формируем один кадр
        buff[0] = 0xB5;
        buff[1] = addres;
        if (data_len <= max_pkg_len) { // Все данные помещаются в один пакет - он или единственный, или завершающий
            buff[2] = data_len;
            buff[3] = (data_pos == 0) ? 0x00 : 0x03;
        } else {
            buff[2] = max_pkg_len;
            buff[3] = (data_pos == 0) ? 0x01 : 0x02;
        }
        buff[4] = tranz_num;
        for(iter = 0; iter < buff[2]; iter++) {
            buff[iter + 5] = data[iter + data_pos];
        }
        data_len -= iter;
        Calc_hCRC(buff, iter + 5);
//Цикл обмена одним кадром
        errnum = 0;
        errcode = 0x88;
        answc = 0;
        QByteArray rd_data;
        do {
            rd_data.clear();
            //отправка
            int size = buff[2] + 7;
            QByteArray tr_data = QByteArray((char *)buff, size);
// Отправка-прием
            putPortData(tr_data);
            rd_data=getPortData(0x3A,8);

            if (rd_data.length()) {
                if (Calc_hCRC((unsigned char*)rd_data.data(), 6)) {
                    if ((rd_data[0] == 0x3A)&&
                        (rd_data[1] == tr_data[1])&&
                        (rd_data[2] == tr_data[2])&&
                        (rd_data[3] == tr_data[3])&&
                        (rd_data[4] == tr_data[4])) {
                        //Ответный кадр верный
                        answc = rd_data[5]&0xF9;
                        //Проверка на правильность принятия
                        (answc == 0x50 || answc == 0x51) ? errcode = 0 : errcode = rd_data[5];
                    }
                }
            }
            if (errcode) errnum++;
            if (errcode >= 0x80 && errcode != 0x88) break;
        } while (errnum < 3 && errcode != 0);
        if (errcode) {
            QMessageBox::information(0, QObject::tr("Can not write to device"),
                    "Обмен завершен с ошибкой 0x" + QString("%1").arg(errcode, 0, 16).toUpper());
            transfer_data = false;
            return "";
        }
        //Обмен завершен успешно
        tranz_num++;
    }

    //Инициируем получение данных
    unsigned char keysign[7]; // Ключ для проверки кадра на повтор
    unsigned char answ[8];
    int answlen;
    errnum = 0;
    data_pos = 0;
    data_len = 0;
    for (int n = 0; n < 7; n++) keysign[n] = 0;
    for (int n = 0; n < 8; n++) answ[n] = 0;
    answlen = 0;
    unsigned char answer;
    bool not_end = true;

    do {
        errcode = 0;
        //Формируем запрос данных или подтверждение приема пакета
        if (data_pos == 0 && answlen == 0) {
            buff[0]=0xB5;
            buff[1]=addres;
            buff[2]=0;
            buff[3]=0x0F;
            buff[4]=(tranz_num + 1);
            Calc_hCRC(buff,5);
            QByteArray tr_data = QByteArray((char *)buff, 7);
            putPortData(tr_data);
        } else {
            // По какой-то причине нужно повторить запрос - ответ не пришел/поврежден или продолжение отправки
            putPortData(QByteArray((char*)answ, answlen));
        }
        // Ожидаем прихода следующего кадра
        QByteArray rd_data;
        rd_data=getPortData(0xB5,5);
        if (rd_data[0] == 0xB5) {
            data_len = abs(rd_data[2]);
            QByteArray rd_data1;
            rd_data1=getPortDataOnly(data_len+2);
            if (rd_data1.length() == data_len + 2) {
                //Кадр нормальной длинны получен
                rd_data += rd_data1;
                if (Calc_hCRC((unsigned char*)rd_data.data(), data_len + 5)) {
                    if (rd_data[1] == addres) {
                        // Проверяем кадр на повтор
                        int nn;
                        for (int n = 0; n < 5; n++) if (keysign[n] != rd_data[n]) nn = 1;
                        if (rd_data[data_len + 5] != keysign[5] || rd_data[data_len + 6] != keysign[6]) nn = 1;
                        if (nn) {
                            // Запоминаем пришедший кадр - он прошел проверки на нормальность
                            for (int n = 0; n < 5; n++) keysign[n] = rd_data[n];
                            keysign[5] = rd_data[data_len + 5];
                            keysign[6] = rd_data[data_len + 6];
                            // Анализируем что за кадр пришел
                            answ_kadr = rd_data[3];
                            switch (answ_kadr) {
                                case (0x80): // Прием за 1 раз
                                    for (uint n = 0; n < data_len; n++) data[n] = rd_data[5 + n];
                                    answer = 0x51;
                                    not_end = false;
                                    break;
                                case (0x81): // Начало приема сообщения
                                    data_pos ? answer = 0x57 : answer = 0x50;
                                    for (uint n = 0; n < data_len; n++) data[data_pos++] = rd_data[5 + n];
                                    break;
                                case (0x82): // Продолжение приема сообщения
                                    if (data_pos) {
                                        answer = 0x50;
                                        for (uint n = 0; n < data_len; n++) data[data_pos++] = rd_data[5 + n];
                                    } else {
                                        answer = 0x84;
                                        data_len = -1;
                                    }
                                    break;
                                case (0x83):  // Окончание приема сообщения
                                    if (data_pos) {
                                        answer=0x51;
                                        for (uint n = 0;n < data_len; n++) data[data_pos++]=rd_data[5 + n];
                                            data_len = data_pos;
                                    } else {
                                        answer = 0x84;
                                        data_len = -1;
                                    }
                                    not_end = false;
                                    break;
                                default: answer = 0x83;
                            }
                            answ[0]=0x3A;
                            answ[1]=rd_data[1];
                            answ[2]=rd_data[2];
                            answ[3]=rd_data[3];
                            answ[4]=rd_data[4];
                            answ[5]=answer;
                            Calc_hCRC(answ,6);
                            answlen=8;
                        }
                        // Отправляем ответ, или сформированный или предыдущий
                        // Ответ отправляется в начале цикла
                    } // Иначе - не ошибка, а чужой пакет
                } else errcode=0x88;
            } else errcode=0x88;
        } else errcode=0x88;
        if (errcode) errnum++;
    } while ((errnum < 3) && (data_len >= 0) && not_end);
    if (answer == 0x51 || answer == 0x57) {
        putPortData(QByteArray((char*)answ, answlen));
    }
    if (data_len > 0) {
        data[data_len++] = 0;
        transfer_data = false;
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        QString data_1 = codec->toUnicode(data);  //то, что пришло в формате читаемой строки
        closePort();
        return QByteArray(data_1.toStdString().c_str());
    }
    transfer_data = false;
    QMessageBox::information(0, QObject::tr("Write to device"),
            QObject::tr("Ошибка обмена"));
    closePort();
    return "";
}

/*
//to do
//разобраться и по возможности переделать это
QByteArray ListenPort::analizeData(QByteArray data) {
    openPort();
    QByteArray substr;
    int gtt_len = 0;
    int pos = data.indexOf("]");
    while(pos > 0) {
        if (data[pos-1] == '[') {
            int posa = data.indexOf("{", pos+1);
            int posb = data.indexOf("[", pos+1);
            if ((posb < posa || posa == -1) && posb > 0) {
                pos = data.indexOf("]", pos+1+gtt_len);
                continue;
            }
            substr = data.mid(0, pos);
            int pos_start = substr.lastIndexOf('\n');
            substr = substr.mid(pos_start + 1, pos - pos_start - 2);
            if (substr == "") {
                pos = data.indexOf("]", pos+1+gtt_len);
                continue;
            }
            QByteArray new_data = writeData("get " + SETTINGS + "." + substr);
            int fst_kv = new_data.indexOf("{");
            int last_kv = new_data.lastIndexOf("}");
            new_data = new_data.mid(fst_kv, last_kv - fst_kv + 1); //json object

            QString st = dataToJson(new_data);
            QJsonDocument json = QJsonDocument::fromJson(st.toUtf8());
            if (json.isNull()) {
                pos = data.indexOf("]", pos+1+gtt_len);
                continue;
            }
            if (!json.isObject()) {
                pos = data.indexOf("]", pos+1+gtt_len);
                continue;
            }
            QJsonObject obj = json.object();
            int st1 = obj["start"].toInt();
            int ln = obj["length"].toInt();

            QByteArray getted_data;
            for (int i = st1; i < ln; i++) {
                QByteArray part = writeData("get " + SETTINGS + "." + substr + "[" + QByteArray::number(i) + "]");
                //обрезать answ...
                int dv_pos = part.indexOf(":") + 1;
                int end_pos = part.lastIndexOf("}");
                part = part.mid(dv_pos, end_pos - dv_pos+1);
                getted_data.append("[" + QByteArray::number(i) + "]:");
                getted_data.append(part);
                getted_data.append("\n");
            }
            getted_data = getted_data.mid(0, getted_data.length());
            QByteArray getted_data_2;
            getted_data_2.append("{");
            getted_data_2.append(getted_data);
            getted_data_2.append("}");

            //правильно добавить
            int pppos = data.indexOf("}", pos+1);
            QByteArray sss = data.mid(pppos+1);

            QByteArray firs = data.mid(0, pos+1);
            data = firs + ":" + getted_data_2 + sss;
            gtt_len = getted_data_2.length();
        }
        pos = data.indexOf("]", pos+1+gtt_len);
        gtt_len = 0;
    }
    port->close();
    return data;
}
*/
