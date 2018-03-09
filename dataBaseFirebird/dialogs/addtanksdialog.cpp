#include "addtanksdialog.h"
#include "ui_addtanksdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

AddTanksDialog::AddTanksDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTanksDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

AddTanksDialog::AddTanksDialog(int tank_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTanksDialog)
{
    tankId = tank_id;
    dataBase = db;

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT * FROM tanks WHERE id=";
    statament.append(QString::number(tank_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->leObject->setText(rec.value(1).toString());
    ui->leTanks->setText(rec.value(2).toString());
    ui->leAdress->setText(rec.value(3).toString());
    ui->leComment->setText(rec.value(4).toString());
}

AddTanksDialog::~AddTanksDialog()
{
    delete ui;
}

void AddTanksDialog::onButtonOkClick() {
    int object_id = ui->leObject->text().toInt();
    int tank_id = ui->leTanks->text().toInt();
    int addr = ui->leAdress->text().toInt();
    QString comment = ui->leComment->text();

    if (tankId > -1) {
        emit onOkClick(tankId, object_id, tank_id, addr, comment);
    } else {
        emit onOkClick(object_id, tank_id, addr, comment);
    }
    this->close();

    /*int price = ui->lePrice->text().toInt();
    QString name = ui->leName->text();
    QString viewName = ui->leViewName->text();

    if (fuelsId > -1) {
        emit onOkClick(fuelsId, name, viewName, price);
    } else {
        emit onOkClick(name, viewName, price);
    }
    this->close();
    */
}
