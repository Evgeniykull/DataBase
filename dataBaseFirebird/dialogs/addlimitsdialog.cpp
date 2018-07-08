#include "addlimitsdialog.h"
#include "ui_addlimitsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

AddLimitsDialog::AddLimitsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLimitsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getFuelMap();
    getLimitsType();
    QDate date = QDate::currentDate();
    ui->dtEd->setDate(date);
}

QString convertData1(QString data) {
    QString newData = data.split("-")[2];
    newData.append(".");
    newData.append(data.split("-")[1]);
    newData.append(".");
    newData.append(data.split("-")[0].mid(2,2));
    return newData;
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

    QSqlQuery query(dataBase);
    //не удалось заменить
    QString statament = QString("SELECT fuelid, valuel, typed, ENDDATE FROM limits WHERE id=%1").arg(QString::number(limits_id));
    qDebug() << statament;
    query.exec(statament);
    query.next();

    QSqlRecord rec = query.record();
    qDebug() << rec;

    QString endDate = convertData1(rec.value("ENDDATE").toString());
    QStringList ed = endDate.split(".");
    QDate date = QDate::fromString(ed[0]+"."+ed[1]+"."+ed[2], "dd.MM.yy");
    ui->dtEd->setDate(date);

    ui->cbFuels->setCurrentText(fuel_map->value(rec.value("fuelid").toInt()));
    ui->leValue->setText(rec.value("valuel").toString());
    ui->cbType->setCurrentText(lim_type->key(rec.value("typed").toString()));
}

AddLimitsDialog::~AddLimitsDialog()
{
    delete lim_type;
    delete ui;
}

void AddLimitsDialog::onButtonOkClick() {
    QString fuel_name = ui->cbFuels->currentText();
    int fuel_id = fuel_map->key(fuel_name);
    QString value = ui->leValue->text();
    QString type = lim_type->value(ui->cbType->currentText());
    QString endDate = ui->dtEd->date().toString("dd.MM.yy");

    if (limitsId > -1) {
        emit onOkClick(limitsId, QString::number(fuel_id), value, type, endDate);
    } else {
        emit onOkClick(QString::number(fuel_id), value, type, endDate);
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
