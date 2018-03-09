#include "addfuelsdialog.h"
#include "ui_addfuelsdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

AddFuelsDialog::AddFuelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFuelsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

AddFuelsDialog::AddFuelsDialog(int fuel_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFuelsDialog)
{
    fuelsId = fuel_id;
    dataBase = db;

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT * FROM fuels WHERE fueldid=";
    statament.append(QString::number(fuel_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->leName->setText(rec.value(1).toString());
    ui->leViewName->setText(rec.value(2).toString());
    ui->lePrice->setText(rec.value(3).toString());
}


AddFuelsDialog::~AddFuelsDialog()
{
    delete ui;
}

void AddFuelsDialog::onButtonOkClick() {
    int price = ui->lePrice->text().toInt();
    QString name = ui->leName->text();
    QString viewName = ui->leViewName->text();

    if (fuelsId > -1) {
        emit onOkClick(fuelsId, name, viewName, price);
    } else {
        emit onOkClick(name, viewName, price);
    }
    this->close();
}
