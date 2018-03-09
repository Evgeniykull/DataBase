#include "addpointsdialog.h"
#include "ui_addpointsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

AddPointsDialog::AddPointsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPointsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

AddPointsDialog::AddPointsDialog(int point_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPointsDialog)
{
    pointId = point_id;
    dataBase = db;

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT * FROM points WHERE id=";
    statament.append(QString::number(point_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    qDebug() << rec;

    ui->leObjectId->setText(rec.value(1).toString());
    ui->leDispsId->setText(rec.value(2).toString());
    ui->leAddress->setText(rec.value(3).toString());
    ui->leTankId->setText(rec.value(4).toString());
    ui->leWorkFlag->setText(rec.value(5).toString());
}

AddPointsDialog::~AddPointsDialog()
{
    delete ui;
}

void AddPointsDialog::onButtonOkClick() {
    int objectId = ui->leObjectId->text().toInt();
    int dispdId = ui->leDispsId->text().toInt();
    int sendAddr = ui->leAddress->text().toInt();
    int tankId = ui->leTankId->text().toInt();
    int workFlag = ui->leWorkFlag->text().toInt();

    if (pointId > -1) {
        emit onOkClick(pointId, objectId, dispdId, sendAddr, tankId, workFlag);
    } else {
        emit onOkClick(objectId, dispdId, sendAddr, tankId, workFlag);
    }

    this->close();
}
