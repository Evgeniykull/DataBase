#ifndef GETCARDDIALOG_H
#define GETCARDDIALOG_H

#include <QDialog>

namespace Ui {
class GetCardDialog;
}

class GetCardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GetCardDialog(QWidget *parent = 0);
    ~GetCardDialog();

signals:
    void onAcceptClick();

private slots:
    void sendSignal();

private:
    Ui::GetCardDialog *ui;
};

#endif // GETCARDDIALOG_H
