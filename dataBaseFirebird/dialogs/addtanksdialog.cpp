#include "addtanksdialog.h"
#include "ui_addtanksdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>
#include <QDebug>

AddTanksDialog::AddTanksDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTanksDialog)
{
    ui->setupUi(this);
    dataBase = db;
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getFuelMap();
    ui->cbFuels->setCurrentIndex(0);
}

AddTanksDialog::AddTanksDialog(int tank_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTanksDialog)
{
    ui->setupUi(this);
    tankId = tank_id;
    dataBase = db;

    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getFuelMap();

    if (fuel_map->size() <= 0) {
        QMessageBox *mess = new QMessageBox();
        mess->setWindowTitle("Добавление резервуара");
        mess->setText("Добавте сначала топлива");
        mess->exec();
        return;
    }

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("SELECT fuelid, sendaddr, comment FROM tanks WHERE id=%1").arg(QString::number(tank_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();
    ui->cbFuels->setCurrentText(fuel_map->value(rec.value("fuelid").toInt()));
    ui->leAdress->setText(rec.value("sendaddr").toString());
    ui->leComment->setText(rec.value("comment").toString());
}

AddTanksDialog::~AddTanksDialog()
{
    delete ui;
}

void AddTanksDialog::onButtonOkClick() {
    QString fuel_name = ui->cbFuels->currentText();
    int fuel_id = fuel_map->key(fuel_name);
    int addr = ui->leAdress->text().toInt();
    QString comment = ui->leComment->text();

    if (tankId > -1) {
        emit onOkClick(tankId, fuel_id, addr, comment);
    } else {
        emit onOkClick(fuel_id, addr, comment);
    }
    this->close();
}

void AddTanksDialog::getFuelMap() {
    fuel_map = new QMap<int, QString>;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = "SELECT fueldid, name from FUELS";
    query->exec(statament);

    int i = 0;
    while(query->next()) {
        fuel_map->insert(query->record().value("fueldid").toInt(),
                         query->record().value("name").toString());
        ui->cbFuels->insertItem(i, query->record().value("name").toString());
        i += 1;
    }
}
