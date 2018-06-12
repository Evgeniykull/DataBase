#include "addpointsdialog.h"
#include "ui_addpointsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

AddPointsDialog::AddPointsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPointsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    tank_map=0;
}

void AddPointsDialog::SetDID(int newdid) {
  did=newdid;
}

void AddPointsDialog::getTankMap() {
  if (tank_map) delete tank_map;
  tank_map = new QMap<int, QString>;
  QSqlQuery query(dataBase);
  query.exec(QString("SELECT ID, SENDADDR, COMMENT from TANKS WHERE objectid=%1 ORDER BY ID").arg(objectId));
  int i = 0;
  ui->cbTankId->clear();
  while(query.next()) {
    QString vis = query.record().value("SENDADDR").toString() + "-" + query.record().value("COMMENT").toString();
    tank_map->insert(query.record().value("id").toInt(), vis);
    ui->cbTankId->insertItem(i, vis);
    i++;
  }
}

void AddPointsDialog::ShowErrorMessage(QString title,QString text) {
  emit toUser(title,text);
}

/*AddPointsDialog::AddPointsDialog(QString object_id, int point_id, QSqlDatabase db, QWidget *parent) :
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
*/

AddPointsDialog::~AddPointsDialog()
{
  if (tank_map) delete tank_map;
  delete ui;
}

void AddPointsDialog::ShowPointsDialog(int pid,int oid,QSqlDatabase db) {
  QSqlQuery query(db);
  QString statament;
  objectId = oid;
  pointId=pid;
  dataBase = db;
  getTankMap();
  if (tank_map->size() <= 0) {
    ShowErrorMessage("Добавление колонки","Список резервуаров пуст");
    return;
  }
  if (pointId>=0) {
    statament = QString("SELECT dispsid, sendaddr, tankid, workflag FROM points WHERE id=%1 AND DISPSID=%2 and objectid=%3")
                .arg(QString::number(pointId))
                .arg(QString::number(did))
                .arg(QString::number(objectId));
    query.exec(statament);
    query.first();
    ui->leDispsId->setText(query.record().value("dispsid").toString());
    ui->leAddress->setText(query.record().value("sendaddr").toString());
    ui->cbTankId->setCurrentText(tank_map->value(query.record().value("tankid").toInt()));
    ui->cbWork->setChecked((query.record().value("workflag").toString() == "0")?false:true);
    ui->leID->setText(QString::number(pointId));
    ui->leID->setDisabled(true);
  } else {
    int newid;
    statament = QString("SELECT MAX(id) FROM points WHERE disps=%1 AND objectid=%2")
                .arg(QString::number(did))
                .arg(QString::number(objectId));
    query.exec(statament);
    query.first();
    newid=query.record().value(0).toInt()+1;
    if (newid<1) newid=1;
    ui->leDispsId->setText(QString::number(did));
    ui->leAddress->setText("");
    //ui->cbTankId->setCurrentText(tank_map->value());
    ui->cbWork->setChecked(true);
    ui->leID->setText(QString::number(newid));
    ui->leID->setDisabled(false);
  }
  show();
}

void AddPointsDialog::DeletePoint(int pid,int oid,QSqlDatabase db) {
  QSqlQuery query(db);
  QString statement;
  statement=QString("DELETE FROM points WHERE id=%1 AND dispsid=%2 AND objectid=%3")
      .arg(QString::number(pid))
      .arg(QString::number(did))
      .arg(QString::number(oid));
  query.exec(statement);
  if (query.lastError().isValid()) {
    ShowErrorMessage("Удаление колонки",query.lastError().databaseText());
  }
  emit needUpdate();
}

void AddPointsDialog::onButtonOkClick() {
  QSqlQuery query(dataBase);
  QString statament;
  int dispsId = ui->leDispsId->text().toInt();
  int sendAddr = ui->leAddress->text().toInt();
  int tankId;
  int workFlag = ui->cbWork->isChecked() ? 1 : 0;
  QString ids = ui->leID->text();
  QString stid=ui->cbTankId->currentText();
  int tpos=stid.indexOf("-");
  if (tpos) {
    tankId=stid.left(tpos).toInt();
  } else {
    ShowErrorMessage("Колонки","Ошибка получения номера резервуара");
    return;
  }
  if (pointId>0) {
    statament = QString("UPDATE points SET dispsid=%1, sendaddr=%2, tankid=%3, workflag=%4 WHERE id=%5 AND objectid=%6")
            .arg(QString::number(dispsId))
            .arg(QString::number(sendAddr))
            .arg(QString::number(tankId))
            .arg(QString::number(workFlag))
            .arg(QString::number(pointId))
            .arg(objectId);

  } else {
    pointId=ids.toInt();
    statament = QString("INSERT INTO points (id, objectid, dispsid, sendaddr, tankid, workflag) VALUES (%1, %2, %3, %4, %5, %6)")
            .arg(QString::number(pointId))
            .arg(objectId)
            .arg(QString::number(dispsId))
            .arg(QString::number(sendAddr))
            .arg(QString::number(tankId))
            .arg(QString::number(workFlag));
  }
  query.exec(statament);
  if (query.lastError().isValid()) {
      ShowErrorMessage("Редактирование колонки",query.lastError().databaseText());
  } else {
      close();
  }
  emit needUpdate();
}

