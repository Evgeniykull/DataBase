#ifndef ADDTANKSDIALOG_H
#define ADDTANKSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMap>

namespace Ui {
class AddTanksDialog;
}

class AddTanksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTanksDialog(QWidget *parent = 0);
//    explicit AddTanksDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddTanksDialog();

    void ShowTankDialog(int tid,int oid,QSqlDatabase db);
    void DeleteTank(int tid,int oid,QSqlDatabase db);

signals:
  void needUpdate();
  void toUser(QString title,QString message);

private slots:
    void onButtonOkClick();

private:
    Ui::AddTanksDialog *ui;
    int tankId = -1;
    int ObjectId = -1;
    QSqlDatabase dataBase;
    QMap<int, QString>* fuel_map;

    void ShowErrorMessage(QString title,QString text);

    void getFuelMap();
};

#endif // ADDTANKSDIALOG_H
