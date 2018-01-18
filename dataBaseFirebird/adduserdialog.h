#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class addUserDialog;
}

class addUserDialog : public QDialog
{
    Q_OBJECT

public:
    addUserDialog(QWidget *parent = 0);
    addUserDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~addUserDialog();

signals:
    void onOkClick(int, int, QString, QString, int);
    void onOkClick(int, int, int, QString, QString, int);

private slots:
    void onButtonOkClick();

private:
    Ui::addUserDialog *ui;
    int userId = -1;
    QSqlDatabase dataBase;
};

#endif // ADDUSERDIALOG_H
