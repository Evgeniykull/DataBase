#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRelationalTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onViewClick();
    void onActUserClick();
    void onActDictionaryClick();
    void onActConnectClick();
    void onConnectClick();

    void updateUsers();
    void onAddUsersClick();
    void addUser(int,int,QString,QString,int);
    void deleteUsers();
    void saveUsers();
    void onEditUsersClick();
    void editUsers(int,int,int,QString,QString,int);

    //все add в одну функцию, передавать только модель как аргумент
    void updateFuels();
    void addFuels();
    void deleteFuels();
    void saveFuels();

private:
    Ui::MainWindow *ui;
    QLabel *status_bar;

    QSqlDatabase db;
    QSqlRelationalTableModel *model_users;
    QSqlRelationalTableModel *model_fuels;

    void renderToolbar();
};

#endif // MAINWINDOW_H
