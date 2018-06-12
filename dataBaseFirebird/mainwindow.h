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
#include <QJsonObject>

#include "port/port.h"
#include "port/portsettings.h"
#include "models/modeluser.h"
#include "models/modellimit.h"
#include "dialogs/getcarddialog.h"
#include "dialogs/addtanksdialog.h"
#include "dialogs/addfuelsdialog.h"
#include "dialogs/addpointsdialog.h"
#include "dialogs/addobjectdialog.h"

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
    void onActAboutClick();

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
    void changedObjectSettings(QString);

    void updateHistory();
    void getHistory();

    void startConfigurate();
    void getUserCard();
    void changeUserCard();

    void currentTabChanged(int);
    void currentTab2Changed(int);

    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

    void changeAccess();
    void accessCheck();

    void declOfVaribles();

    //my Всякие дополнительные полезные функции
    void getUserIndex();
    void parseArray(QByteArray, int&, int&);
    QJsonValue getJSONField(QByteArray data,QString fieldname);
    void timerEnd();

    void MessageToUser(QString title,QString text);

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
    int azsNum = 0;
    QString pointNum = "";
    QMessageBox *mbx;

    bool isAdmin = false;
    bool stopCard = false;

    //new
    ModelUser *add_user_model;
    ModelLimit *add_limits_model;
    GetCardDialog *card_dialog;

//// Диалоги редактирования BD
    AddFuelsDialog * add_fuels;
    AddTanksDialog * add_tanks;
    AddPointsDialog * add_points;
    AddObjectDialog * add_object;
};

#endif // MAINWINDOW_H
