#include "addobjectdialog.h"
#include "ui_addobjectdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

AddObjectDialog::AddObjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddObjectDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

AddObjectDialog::AddObjectDialog(int object_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddObjectDialog)
{
    objectId = object_id;
    dataBase = db;

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = QString("SELECT * FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(objectId));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->leObjectId->setText(rec.value("objectId").toString());
    ui->leName->setText(rec.value("objectName").toString());
    ui->lePass->setText(rec.value("ADMINPASSWORD").toString());
}


AddObjectDialog::~AddObjectDialog()
{
    delete ui;
}

void AddObjectDialog::onButtonOkClick() {
    QString objectId = ui->leObjectId->text();
    QString name = ui->leName->text();
    QString pass = ui->lePass->text();
    emit onOkClick(objectId, name, pass);
    this->close();
}
