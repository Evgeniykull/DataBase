#ifndef ADDFUELSDIALOG_H
#define ADDFUELSDIALOG_H

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
    explicit AddFuelsDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddFuelsDialog();

signals:
    void onOkClick(int, QString, QString, QString);
    void onOkClick(QString, QString, QString);

private slots:
    void onButtonOkClick();

private:
    Ui::AddFuelsDialog *ui;
    int fuelsId = -1;
    QSqlDatabase dataBase;
};

#endif // ADDFUELSDIALOG_H
