#ifndef ADDPOINTSDIALOG_H
#define ADDPOINTSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMap>

namespace Ui {
class AddPointsDialog;
}

class AddPointsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPointsDialog(QWidget *parent = 0);
//    explicit AddPointsDialog(QString, int, QSqlDatabase, QWidget *parent = 0);
    ~AddPointsDialog();

    void ShowPointsDialog(int pid, int oid, QSqlDatabase db);
    void DeletePoint(int pid,int oid,QSqlDatabase db);
    void SetDID(int newdid);

signals:
  void needUpdate();
  void toUser(QString title,QString message);

private slots:
    void onButtonOkClick();
    void getTankMap();

private:
    void ShowErrorMessage(QString title,QString text);
    Ui::AddPointsDialog *ui;
    int pointId = -1;
    int did;   // Идентификатор стороны
    int objectId = 0;
    QSqlDatabase dataBase;
    QMap<int, QString>* tank_map;
};

#endif // ADDPOINTSDIALOG_H
