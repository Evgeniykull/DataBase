#ifndef PORT_H
#define PORT_H
/////
//  Логика работы с объектом:
//  - создаем объект
//  - начинаем обмен функцией StatComm (открытие порта)
//  - Если функция вернула ноль - обмен не состоится. Сообщение уже будет выведено
//  - Обмениваемся строками функциями writeData, можно
//    поменять уровень доступа функцией changeAccess, указав требуемый и пароль
//    Функция возвращает 1 если уровень получен
//  - После всего обмена завершаем функцией EndComm (закрытие порта)



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
    explicit Port(PortSettings *ps, QWidget *parent = 0);
    ~Port();
// Функции управления состоянием порта
    bool StartComm();  // Открывает порт (возвращает 1 если всё нормально)
    void EndComm();    // Закрывает порт
//    void sendData(QByteArray);
    QByteArray writeData(QByteArray text); // Первая функция обмена
    QByteArray write(QByteArray text);     // Вторая функция обмена
// Функции, реализующие логику работы с устройством
    bool changeAccess(int,QByteArray passwd);
//

private slots:
    void onSettingsClick();
    void rewriteSettings(int addr = -1);
//    void onChangeAccessClick();
//    void onAccessUpdateClick();

private:
    Ui::Port *ui;
    PortSettings *port_settings;
    Settings sett;
    QSerialPort *port;
    void closePort();
    int openPort();  // Возвращает 1 если открылся
// Получение и передача данных из RS232
    QByteArray getPortDataOnly(int);   // Просто прием кучи байтов
    QByteArray getPortData(char,int);  // С ожиданием преамбулы
    void putPortData(QByteArray);      // Отправка данных в RS232
// Подсчет и проверка CRC
    void Add_hCRC(QByteArray * data);
    int Check_hCRC(QByteArray data);

    QByteArray intWriteData(QByteArray text); // Функция обмена, отсылающая строку и принимающая результат

    bool transfer_data = false; //если true, то запрет на передачу
    unsigned char errnum;  // Количество ошибочных обменов
    unsigned char errcode; // Код ошибки обмена
    unsigned char answc;   // Код ответа
};

#endif // PORT_H
