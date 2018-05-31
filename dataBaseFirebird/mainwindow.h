#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
#include <tree/treemodel.h>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProgressDialog>
#include "port/port.h"
#include "port/portsettings.h"

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
    void changeCurrentPointNum();

    void updateLimits();
    void addLimits();
    void deleteLimits();
    void editLimits();

    void updateObject();
    void addObject();
    void deleteObject();
    void editObject();
    void changedObject(QModelIndex);
    void setObjectSettings();
    void writeObjectSettings();
    void changedObjectSettings(QByteArray);

    void updateHistory();
    void getHistory();

    void startConfigurate();
    void getUserCard();
    void changeUserCard();
    void updateUserOnDevice(int);

    void currentTabChanged(int);
    void currentTab2Changed(int);

    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

    void changeAccess();
    void accessCheck();

    //my
    void getUserIndex();
    void parseArray(QByteArray, int&, int&);
    QVector<int>* getDataByLoop(QByteArray, int, int, QString);
    QMap<int, QJsonObject>* getUsersOnDevice(QVector<int> users_ids);
    void timerEnd();

private:
    Ui::MainWindow *ui;
    QLabel *status_bar;
    QLabel *port_status;
    QActionGroup *menu_bar;

    QSqlDatabase db;
    QSqlRelationalTableModel *model_users;
    QSqlRelationalTableModel *model_fuels;
    QSqlRelationalTableModel *model_tanks;
    QSqlRelationalTableModel *model_points;
    QSqlRelationalTableModel *model_limits;
    QSqlRelationalTableModel *model_object;
    QSqlRelationalTableModel *model_history;
    TreeModel *model;

    void renderToolbar();
    QString getCartMemoryData(int);
    QString getValueFromState(QString, QString);

    QString db_status;

    QStringList port_names;
    Port *port;
    Port *card_reader;
    PortSettings *port_settings;
    int selected_user_row = -1;
    int selected_user_id = -1;

    int timer_end = 0;
    int last_select_tab2 = 0;
    QSettings *settings;
    QString azsNum = 0;
    QString pointNum = "";
    QMessageBox *mbx;
    QProgressDialog *progr_dialog;

    bool isAdmin = false;
};

#endif // MAINWINDOW_H
