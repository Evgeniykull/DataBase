#include "getcarddialog.h"
#include "ui_getcarddialog.h"

GetCardDialog::GetCardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetCardDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(sendSignal()));
}

GetCardDialog::~GetCardDialog()
{
    delete ui;
}

void GetCardDialog::sendSignal() {
    emit onAcceptClick();
    close();
}
