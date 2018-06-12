#include "addfuelsdialog.h"
#include "ui_addfuelsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QDebug>

AddFuelsDialog::AddFuelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFuelsDialog) {
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

AddFuelsDialog::~AddFuelsDialog()
{
    delete ui;
}

void AddFuelsDialog::ShowFuelData(int FuelID,QSqlDatabase db) {
  QSqlQuery query(db);
  QString statament;
  fuelsId = FuelID;
  dataBase = db;
  if (fuelsId<=0) {
// Новое топливо
    ui->leName->setText("");
    ui->leViewName->setText("");
    ui->lePrice->setText("00.00");
  } else {
// Существующее топливо
    statament = "SELECT name, viewname, price FROM fuels WHERE fueldid=";
    statament.append(QString::number(fuelsId));
    query.exec(statament);
    query.first();
    ui->leName->setText(query.record().value("name").toString());
    ui->leViewName->setText(query.record().value("viewname").toString());
    ui->lePrice->setText(query.record().value("price").toString());
  }
  show();
}

void AddFuelsDialog::ShowErrorMessage(QString title,QString text) {
  emit toUser(title,text);
}

void AddFuelsDialog::onButtonOkClick() {
    QString price = ui->lePrice->text();
    QString name = ui->leName->text();
    QString viewName = ui->leViewName->text();
    QSqlQuery query(dataBase);
    QString statament;      // Строка запроса
    if (fuelsId > 0) {
// Редактируем существующее топливо
      statament = QString("UPDATE fuels SET name='%1', viewname='%2', price=%3 WHERE fueldid=%4")
            .arg(name)
            .arg(viewName)
            .arg(price)
            .arg(QString::number(fuelsId));
    } else {
// Добавляем новое топливо
      statament = QString("INSERT INTO fuels (name, viewname, price) VALUES ('%1', '%2', %3)")
            .arg(name)
            .arg(viewName)
            .arg(price);
    }

    query.exec(statament);
    if (query.lastError().isValid()) {
      ShowErrorMessage("Редактирование топлива", query.lastError().databaseText());
    } else{
        close();
    }
    emit needUpdate();
}

void AddFuelsDialog::DeleteFuelData(int FuelID, QSqlDatabase db) {
  QSqlQuery query(db);
  QString statement;
  int cnt;
  statement = QString("SELECT COUNT(*) FROM TANKS WHERE FUELID=%1")
      .arg(QString::number(FuelID));
  query.exec(statement);
  query.first();
  cnt=query.record().value(0).toInt();
  if (cnt>0) {
    ShowErrorMessage("Удаление топлива","Данное топливо используется в резервуарах");
    return;
  }
  statement = "DELETE FROM fuels WHERE fueldid=";
  statement.append(QString::number(FuelID));
  query.exec(statement);
  if (query.lastError().isValid()) {
    ShowErrorMessage("Удаление топлива",
                    query.lastError().databaseText());
  }
  emit needUpdate();
}
