#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
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

    void updateFuels();
    void addFuels();
    void deleteFuels();
    void onEditFuelsClick();

    void updateTanks();
    void addTanks();
    void deleteTanks();
    void onEditTanksClick();

    void updatePoints();
    void addPoints();
    void deletePoints();
    void onEditPointsClick();

    void getFuelIndex();

    //my
    void getUserIndex();
    void parseArray(QByteArray, int&, int&);
    QVector<int>* getDataByLoop(QByteArray, int, int, QString);
    QMap<int, QJsonObject>* getUsersOnDevice(QVector<int> users_ids);

private:
    Ui::MainWindow *ui;
    QLabel *status_bar;
    QLabel *port_status;

    QSqlDatabase db;
    QSqlRelationalTableModel *model_users;
    QSqlRelationalTableModel *model_fuels;
    QSqlRelationalTableModel *model_tanks;
    QSqlRelationalTableModel *model_points;
    TreeModel *model;

    void renderToolbar();

    //new
    QStringList port_names;
    Port *port;
    int selected_user_row = -1;
};

#endif // MAINWINDOW_H
