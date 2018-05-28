#ifndef ADDOBJECTDIALOG_H
#define ADDOBJECTDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class AddObjectDialog;
}

class AddObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddObjectDialog(QWidget *parent = 0);
    explicit AddObjectDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddObjectDialog();

signals:
    void onOkClick(QString, QString, QString);

private slots:
    void onButtonOkClick();

private:
    Ui::AddObjectDialog *ui;
    int objectId = -1;
    QSqlDatabase dataBase;
};

#endif // ADDOBJECTDIALOG_H
