#include "addobjectdialog.h"
#include "ui_addobjectdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

AddObjectDialog::AddObjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddObjectDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

void AddObjectDialog::ShowErrorMessage(QString title,QString text) {
  emit toUser(title,text);
}

void AddObjectDialog::ShowObjectData(int ObjectID,QSqlDatabase db) {
  QSqlQuery query(db);
  QString statament;
  objectId = ObjectID;
  dataBase = db;
  if (objectId<=0) {
// Новый объект
    ui->leName->setText("");
    ui->lePass->setText("");
  } else {
// Существующий объект
    statament = QString("SELECT objectname, ADMINPASSWORD FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(objectId));
    query.exec(statament);
    query.first();
    ui->leName->setText(query.record().value("objectname").toString());
    ui->lePass->setText(query.record().value("ADMINPASSWORD").toString());
  }
  show();
}

void AddObjectDialog::onButtonOkClick() {
    QString name = ui->leName->text();
    QString pass = ui->lePass->text();
    QSqlQuery query(dataBase);
    QString statament;      // Строка запроса
    if (objectId > 0) {
// Редактируем существующий объект
      statament = QString("UPDATE OBJECT_TABLES SET OBJECTNAME='%1', ADMINPASSWORD='%2' WHERE OBJECTID=%3")
            .arg(name)
            .arg(pass)
            .arg(QString::number(objectId));
    } else {
// Добавляем новый объект
      statament = QString("INSERT INTO OBJECT_TABLES (OBJECTNAME, ADMINPASSWORD) VALUES ('%1', '%2')")
            .arg(name)
            .arg(pass);
    }

    query.exec(statament);
    if (query.lastError().isValid()) {
      ShowErrorMessage("Редактирование топлива", query.lastError().databaseText());
    } else{
        close();
    }
    emit needUpdate();
}

void AddObjectDialog::DeleteObject(int object_id,QSqlDatabase db) {
    dataBase = db;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QSqlQuery* tank_query = new QSqlQuery(dataBase);
    QSqlQuery* point_query = new QSqlQuery(dataBase);
    QString tank_bd_query = QString("SELECT COUNT(*) FROM TANKS WHERE OBJECTID=%1").arg(QString::number(object_id));
    QString point_bd_query = QString("SELECT COUNT(*) FROM POINTS WHERE OBJECTID=%1").arg(QString::number(object_id));

    tank_query->exec(tank_bd_query);
    point_query->exec(point_bd_query);

    tank_query->next();
    point_query->next();

    if (tank_query->record().value(0).toInt() != 0) {
        ShowErrorMessage("Удаление объекта", "У объекта есть зависимости");
        return;
    }

    if (point_query->record().value(0).toInt() != 0) {
        ShowErrorMessage("Удаление объекта", "У объекта есть зависимости");
        return;
    }

    QString statament = QString("DELETE FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(object_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        ShowErrorMessage("Удаление объекта", query->lastError().databaseText());
        return;
    }
    emit needUpdate();
}

AddObjectDialog::~AddObjectDialog()
{
    delete ui;
}

QString AddObjectDialog::getSettings(int object_id, QSqlDatabase db) {
    dataBase = db;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = QString("SELECT CONNECTIONPROPERTY FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(object_id));
    query->exec(statament);
    query->next();
    return query->record().value(0).toString();
}

void AddObjectDialog::setSettings(int object_id, QString sett, QSqlDatabase db) {
    dataBase = db;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = QString("UPDATE OBJECT_TABLES SET CONNECTIONPROPERTY='%1' WHERE OBJECTID=%2")
            .arg(sett)
            .arg(QString::number(object_id));
    query->exec(statament);
    if (query->lastError().isValid()) {
        ShowErrorMessage("Обновление настроек объекта", query->lastError().databaseText());
    }
    emit needUpdate();
}
