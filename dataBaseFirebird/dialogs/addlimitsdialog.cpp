#include "addlimitsdialog.h"
#include "ui_addlimitsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>

AddLimitsDialog::AddLimitsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLimitsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getFuelMap();
    getLimitsType();
}

AddLimitsDialog::AddLimitsDialog(int limits_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLimitsDialog)
{
    ui->setupUi(this);
    limitsId = limits_id;
    dataBase = db;
    getLimitsType();
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getFuelMap();

    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = "SELECT * FROM limits WHERE id=";
    statament.append(QString::number(limits_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->cbFuels->setCurrentText(fuel_map->value(rec.value(2).toInt()));
    ui->leValue->setText(rec.value("valuel").toString());
    ui->leDays->setText(rec.value("days").toString());
    ui->cbType->setCurrentText(lim_type->key(rec.value("typed").toString()));
}

AddLimitsDialog::~AddLimitsDialog()
{
    delete ui;
}

void AddLimitsDialog::onButtonOkClick() {
    QString fuel_name = ui->cbFuels->currentText();
    int fuel_id = fuel_map->key(fuel_name);
    QString value = ui->leValue->text();
    QString days = ui->leDays->text();
    QString type = lim_type->value(ui->cbType->currentText());

    if (limitsId > -1) {
        emit onOkClick(limitsId, QString::number(fuel_id), value, days, type);
    } else {
        emit onOkClick(QString::number(fuel_id), value, days, type);
    }
    this->close();
}

void AddLimitsDialog::getFuelMap() {
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

void AddLimitsDialog::getLimitsType() {
    lim_type = new QMap<QString, QString>;
    lim_type->insert("Лимит отключен", "0");
    lim_type->insert("Безлимитный", "1");
    lim_type->insert("Одноразовый, с заданным сроком и количеством", "2");
    lim_type->insert("Однодневный, возобновляемый", "3");
    lim_type->insert("Недельный, возобновляемый", "4");
    lim_type->insert("Месячный, возобновляемый", "5");
}
