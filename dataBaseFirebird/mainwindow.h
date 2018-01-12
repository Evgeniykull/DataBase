#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *model_users;

    void renderToolbar();
};

#endif // MAINWINDOW_H
