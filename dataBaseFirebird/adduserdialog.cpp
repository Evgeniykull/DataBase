#include "adduserdialog.h"
#include "ui_adduserdialog.h"

addUserDialog::addUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addUserDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

addUserDialog::~addUserDialog()
{
    delete ui;
}

void addUserDialog::onButtonOkClick() {
    int parentId = ui->leParentId->text().toInt();
    int cardId = ui->leCardId->text().toInt();
    QString name = ui->leViewName->text();
    QString shortName = ui->leShortName->text();
    int indexId = ui->leIndexId->text().toInt();

    emit onOkClick(parentId, cardId, name, shortName, indexId);
    this->close();
}
