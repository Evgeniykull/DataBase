#ifndef ADDLIMITSDIALOG_H
#define ADDLIMITSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QMap>

namespace Ui {
class AddLimitsDialog;
}

class AddLimitsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLimitsDialog(QWidget *parent = 0);
    explicit AddLimitsDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddLimitsDialog();

signals:
    void onOkClick(int, QString, QString, QString, QString);
    void onOkClick(QString, QString, QString, QString);

private slots:
    void onButtonOkClick();
    void getFuelMap();
    void getLimitsType();

private:
    Ui::AddLimitsDialog *ui;
    int limitsId = -1;
    QSqlDatabase dataBase;
    QMap<int, QString>* fuel_map;
    QMap <QString, QString>* lim_type;
};

#endif // ADDLIMITSDIALOG_H
