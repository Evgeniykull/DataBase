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
    void ShowObjectData(int ObjectID,QSqlDatabase db);
    void DeleteObject(int object_id,QSqlDatabase db);
    QString getSettings(int object_id,QSqlDatabase db);
    void setSettings(int object_id,QString sett,QSqlDatabase db);
    ~AddObjectDialog();

signals:
    void needUpdate();
    void toUser(QString title,QString message);

private slots:
    void onButtonOkClick();

private:
    Ui::AddObjectDialog *ui;
    int objectId = -1;
    QSqlDatabase dataBase;
    void ShowErrorMessage(QString title,QString text);
};

#endif // ADDOBJECTDIALOG_H
