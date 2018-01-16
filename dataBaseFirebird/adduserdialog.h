#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include <QDialog>

namespace Ui {
class addUserDialog;
}

class addUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addUserDialog(QWidget *parent = 0);
    ~addUserDialog();

signals:
    void onOkClick(int, int, QString, QString, int);

private slots:
    void onButtonOkClick();

private:
    Ui::addUserDialog *ui;
};

#endif // ADDUSERDIALOG_H
