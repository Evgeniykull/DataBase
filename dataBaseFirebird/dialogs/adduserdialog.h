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
    void onOkClick(int, QString, QString, int, QString);
    void onOkClick(int, int, QString, QString, int, QString);

private slots:
    void onButtonOkClick();
    void typeChanged(QString);

private:
    Ui::addUserDialog *ui;
    int userId = -1;
    QSqlDatabase dataBase;
    QString sldate1 = "";
    void addUserType();
    void getParents();
    QMap <QString, QString>* parent_map;
    QMap <QString, QString>* us_type;
};

#endif // ADDUSERDIALOG_H
