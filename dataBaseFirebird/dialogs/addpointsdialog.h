#ifndef ADDPOINTSDIALOG_H
#define ADDPOINTSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class AddPointsDialog;
}

class AddPointsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPointsDialog(QWidget *parent = 0);
    explicit AddPointsDialog(int, QSqlDatabase, QWidget *parent = 0);
    ~AddPointsDialog();

signals:
    void onOkClick(int, int, int, int, int);
    void onOkClick(int, int, int, int, int, int);

private slots:
    void onButtonOkClick();

private:
    Ui::AddPointsDialog *ui;
    int pointId = -1;
    QSqlDatabase dataBase;
};

#endif // ADDPOINTSDIALOG_H
