#ifndef ADDFUELSDIALOG_H
#define ADDFUELSDIALOG_H

#include <QMessageBox>
#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class AddFuelsDialog;
}

class AddFuelsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFuelsDialog(QWidget *parent = 0);
//    explicit AddFuelsDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddFuelsDialog();
    void ShowFuelData(int FuelID, QSqlDatabase db);
    void DeleteFuelData(int FuelID,QSqlDatabase db);

signals:
    void needUpdate();
    void toUser(QString title,QString message);

private slots:
    void onButtonOkClick();

private:
    Ui::AddFuelsDialog *ui;
    int fuelsId = -1;
    QSqlDatabase dataBase;
    void ShowErrorMessage(QString title,QString text);
};

#endif // ADDFUELSDIALOG_H
