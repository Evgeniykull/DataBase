#include "addtanksdialog.h"
#include "ui_addtanksdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

AddTanksDialog::AddTanksDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTanksDialog) {
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    fuel_map=0;
}


AddTanksDialog::~AddTanksDialog() {
  if (fuel_map) delete fuel_map;
  delete ui;
}

void AddTanksDialog::ShowTankDialog(int tid,int oid,QSqlDatabase db) {
  QSqlQuery query(db);
  tankId = tid;
  ObjectId = oid;
  dataBase = db;
  getFuelMap();
  if (fuel_map->size() <= 0) {
    ShowErrorMessage("Добавление резервуара","Справочник топлива пустой");
  } else {
    if (tid>0) {
      QString statament = QString("SELECT fuelid, sendaddr, comment FROM tanks WHERE id=%1 AND objectid=%2")
                                .arg(QString::number(tankId))
                                .arg(QString::number(ObjectId));
      query.exec(statament);
      query.first();
      ui->cbFuels->setCurrentText(fuel_map->value(query.record().value("fuelid").toInt()));
      ui->leAdress->setText(query.record().value("sendaddr").toString());
      ui->leComment->setText(query.record().value("comment").toString());
      ui->leID->setText(QString::number(tankId));
      ui->leID->setDisabled(true);
    } else {
      QString statament = QString("SELECT MAX(id) FROM tanks WHERE objectid=%1")
                                .arg(QString::number(ObjectId));
      query.exec(statament);
      query.first();
      if (query.isValid()) {
        ui->leID->setText(QString::number(query.record().value(0).toInt()+1));
      } else ui->leID->setText("1");
      ui->cbFuels->setCurrentText(fuel_map->value(0));
      ui->leAdress->setText("");
      ui->leComment->setText("");
      ui->leID->setDisabled(false);
    }
  }
  show();
}

void AddTanksDialog::DeleteTank(int tid,int oid,QSqlDatabase db) {
  QSqlQuery query(db);
  QString statement ;
  statement = QString("SELECT * FROM POINTS WHERE TANKID=%1");
  query.exec(statement);
  query.first();
  if (query.isValid()) {
    ShowErrorMessage("Удаление резервуара","Данный резервуар используется в системе");
    return;
  }
  statement = QString("DELETE FROM tanks WHERE id=%1 AND objectid=%2")
                         .arg(QString::number(tid))
                         .arg(QString::number(oid));
  query.exec(statement);
  if (query.lastError().isValid()) {
    ShowErrorMessage("Удаление резервуара",query.lastError().databaseText());
  }
  emit needUpdate();
}

void AddTanksDialog::ShowErrorMessage(QString title,QString text) {
  emit toUser(title,text);
}

void AddTanksDialog::onButtonOkClick() {
  QSqlQuery query(dataBase);
  QString fuel_name = ui->cbFuels->currentText();
  QString statament;
  int fuel_id = fuel_map->key(fuel_name);
  int addr = ui->leAdress->text().toInt();
  QString comment = ui->leComment->text();
  int inputtid = ui->leID->text().toInt();
  int cnt;
  if (tankId<=0) {
// Добавление нового резервуара
    statament = QString("SELECT COUNT(*) FROM TANKS WHERE ID=%1 AND OBJECTID=%2")
            .arg(QString::number(inputtid))
            .arg(ObjectId);
    query.exec(statament);
    query.first();
    cnt=query.record().value(0).toInt();
    if (cnt>0) {
      ShowErrorMessage("Добавление резервуара","Резервуар с таким ID уже существует");
    } else {
      statament = QString("INSERT INTO tanks (id, fuelId, sendAddr, objectId, comment) VALUES (%1, %2, %3, %4, %5)")
              .arg(QString::number(inputtid))
              .arg(QString::number(fuel_id))
              .arg(QString::number(addr))
              .arg(ObjectId)
              .arg("'" + comment + "'");
      query.exec(statament);
      if (query.lastError().isValid()) {
        ShowErrorMessage("Добавление резервуара",query.lastError().databaseText());
      } else close();
    }
  } else {
// Редактирование существующего резервуара
    statament = QString("UPDATE tanks SET fuelid=%1, sendaddr=%2, comment=%3 WHERE id=%4 AND objectid=%5")
            .arg(QString::number(fuel_id))
            .arg(QString::number(addr))
            .arg("'" + comment + "'")
            .arg(QString::number(tankId))
            .arg(ObjectId);
    query.exec(statament);
    if (query.lastError().isValid()) {
      ShowErrorMessage("Редактирование резервуара",query.lastError().databaseText());
    } else close();
  }
  emit needUpdate();
}

void AddTanksDialog::getFuelMap() {
  int i;
  QSqlQuery query(dataBase);
  if (fuel_map) delete fuel_map;
  fuel_map = new QMap<int, QString>;
  query.exec("SELECT fueldid, name from FUELS");
  ui->cbFuels->clear();
  i=0;
  while(query.next()) {
    fuel_map->insert(query.record().value("fueldid").toInt(),
                     query.record().value("name").toString());
    ui->cbFuels->insertItem(i, query.record().value("name").toString());
    i++;
  }
}
