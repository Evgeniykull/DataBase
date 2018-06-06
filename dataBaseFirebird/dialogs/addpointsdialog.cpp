#include "addpointsdialog.h"
#include "ui_addpointsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMessageBox>
#include <QDebug>

AddPointsDialog::AddPointsDialog(QString object_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPointsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    objectId = object_id;
    dataBase = db;
    getTankMap();
}

AddPointsDialog::AddPointsDialog(QString object_id, int point_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPointsDialog)
{
    pointId = point_id;
    objectId = object_id;
    dataBase = db;

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    getTankMap();

    if (tank_map->size() <= 0) {
        QMessageBox *mess = new QMessageBox();
        mess->setWindowTitle("Добавление колонки");
        mess->setText("Добавте сначала резервуар");
        mess->exec();
        return;
    }

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT dispsid, sendaddr, tankid, workflag FROM points WHERE id=";
    statament.append(QString::number(point_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->leDispsId->setText(rec.value("dispsid").toString());
    ui->leAddress->setText(rec.value("sendaddr").toString());
    ui->cbTankId->setCurrentText(tank_map->value(rec.value("tankid").toInt()));
    if (rec.value("workflag").toString() == "1") {
        ui->cbWork->setChecked(true);
    } else {
        ui->cbWork->setChecked(false);
    }
    ui->leID->setText(QString::number(pointId));
    ui->leID->setDisabled(true);
}

AddPointsDialog::~AddPointsDialog()
{
    delete ui;
}

void AddPointsDialog::onButtonOkClick() {
    int dispdId = ui->leDispsId->text().toInt();
    int sendAddr = ui->leAddress->text().toInt();
    int tankId = ui->cbTankId->currentText().toInt();
    int workFlag = ui->cbWork->isChecked() ? 1 : 0;
    QString _id = ui->leID->text();
    emit onOkClick(_id.toInt(), dispdId, sendAddr, tankId, workFlag);

    this->close();
}

void AddPointsDialog::getTankMap() {
    tank_map = new QMap<int, QString>;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = QString("SELECT id, SENDADDR, COMMENT from TANKS WHERE objectid=%1").arg(objectId);
    query->exec(statament);

    int i = 0;
    while(query->next()) {
        QString vis = query->record().value("SENDADDR").toString() + "-" + query->record().value("COMMENT").toString();
        tank_map->insert(query->record().value("id").toInt(), vis);
        ui->cbTankId->insertItem(i, vis);
        i += 1;
    }
}
