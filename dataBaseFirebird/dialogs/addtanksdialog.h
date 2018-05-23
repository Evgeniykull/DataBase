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
    explicit AddTanksDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddTanksDialog();

signals:
    void onOkClick(int, int, int, QString);
    void onOkClick(int, int, QString);

private slots:
    void onButtonOkClick();

private:
    Ui::AddTanksDialog *ui;
    int tankId = -1;
    QSqlDatabase dataBase;
    QMap<int, QString>* fuel_map;

    void getFuelMap();
};

#endif // ADDTANKSDIALOG_H
