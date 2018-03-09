#ifndef ADDTANKSDIALOG_H
#define ADDTANKSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class AddTanksDialog;
}

class AddTanksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTanksDialog(QWidget *parent = 0);
    explicit AddTanksDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddTanksDialog();

signals:
    void onOkClick(int, int, int, int, QString);
    void onOkClick(int, int , int, QString);

private slots:
    void onButtonOkClick();

private:
    Ui::AddTanksDialog *ui;
    int tankId = -1;
    QSqlDatabase dataBase;
};

#endif // ADDTANKSDIALOG_H
