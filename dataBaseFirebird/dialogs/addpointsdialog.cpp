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
    ui->leWorkFlag->setText(rec.value("workflag").toString());
}

AddPointsDialog::~AddPointsDialog()
{
    delete ui;
}

void AddPointsDialog::onButtonOkClick() {
    int dispdId = ui->leDispsId->text().toInt();
    int sendAddr = ui->leAddress->text().toInt();
    int tankId = ui->cbTankId->currentText().toInt();
    int workFlag = ui->leWorkFlag->text().toInt();

    if (pointId > -1) {
        emit onOkClick(pointId, dispdId, sendAddr, tankId, workFlag);
    } else {
        emit onOkClick(dispdId, sendAddr, tankId, workFlag);
    }

    this->close();
}

void AddPointsDialog::getTankMap() {
    tank_map = new QMap<int, QString>;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = QString("SELECT id from TANKS WHERE objectid=%1").arg(objectId);
    query->exec(statament);

    int i = 0;
    while(query->next()) {
        tank_map->insert(query->record().value("id").toInt(),
                         query->record().value("id").toString());
        ui->cbTankId->insertItem(i, query->record().value("id").toString());
        i += 1;
    }
}
