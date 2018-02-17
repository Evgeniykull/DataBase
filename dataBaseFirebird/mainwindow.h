#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
#include <QtSerialPort/QSerialPort>
#include <tree/treemodel.h>
#include "port/port.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

struct Settings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Settings SettingsPort;

private slots:
    void onViewClick();
    void onActUserClick();
    void onActDictionaryClick();
    void onActConnectClick();
    void onConnectClick();

    void updateUsers();
    void onAddUsersClick();
    void deleteUsers();
    void onEditUsersClick();

    //все add в одну функцию, передавать только модель как аргумент
    void updateFuels();
    void addFuels();
    void deleteFuels();
    void saveFuels();

    //new
    void openPort();
    void closePort();
    void changePort(QString name);

    //my
    void getUserIndex();
    void endTransmitData(QByteArray);

private:
    Ui::MainWindow *ui;
    QLabel *status_bar;
    QLabel *port_status;

    QSqlDatabase db;
    QSqlRelationalTableModel *model_users;
    QSqlRelationalTableModel *model_fuels;
    TreeModel *model;

    void renderToolbar();

    //new
    QSerialPort *port;
    QStringList port_names;
    Port *port_;


    int byteOnPackage;
    unsigned char buff[255];
    unsigned char errnum;  // Количество ошибочных обменов
    unsigned char errcode; // Код ошибки обмена
    unsigned char answc;   // Код ответа
};

#endif // MAINWINDOW_H
