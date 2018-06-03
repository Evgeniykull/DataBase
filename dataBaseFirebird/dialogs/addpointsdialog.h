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
    explicit AddPointsDialog(QString, QSqlDatabase, QWidget *parent = 0);
    explicit AddPointsDialog(QString, int, QSqlDatabase, QWidget *parent = 0);
    ~AddPointsDialog();

signals:
    void onOkClick(int, int, int, int);
    void onOkClick(int, int, int, int, int);

private slots:
    void onButtonOkClick();
    void getTankMap();

private:
    Ui::AddPointsDialog *ui;
    int pointId = -1;
    QString objectId = "0";
    QSqlDatabase dataBase;
    QMap<int, QString>* tank_map;
};

#endif // ADDPOINTSDIALOG_H
