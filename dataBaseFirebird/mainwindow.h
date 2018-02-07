#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRelationalTableModel>
#include <tree/treemodel.h>

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
    void deleteUsers();
    void onEditUsersClick();

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
    TreeModel *model;

    void renderToolbar();
};

#endif // MAINWINDOW_H
