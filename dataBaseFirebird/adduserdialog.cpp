#include "adduserdialog.h"
#include "ui_adduserdialog.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

addUserDialog::addUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addUserDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
}

addUserDialog::addUserDialog(int user_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addUserDialog)
{
    userId = user_id;
    dataBase = db;
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT * FROM users WHERE userid=";
    statament.append(QString::number(user_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();

    ui->leParentId->setText(rec.value(1).toString());
    ui->leShortName->setText(rec.value(2).toString());
    ui->leViewName->setText(rec.value(3).toString());
    ui->leCardId->setText(rec.value(4).toString());
    ui->leIndexId->setText(rec.value(5).toString());
}

addUserDialog::~addUserDialog()
{
    delete ui;
}

void addUserDialog::onButtonOkClick() {
    int parentId = ui->leParentId->text().toInt();
    int cardId = ui->leCardId->text().toInt();
    QString name = ui->leViewName->text();
    QString shortName = ui->leShortName->text();
    int indexId = ui->leIndexId->text().toInt();

    if (userId > -1) {
        emit onOkClick(userId, parentId, cardId, name, shortName, indexId);
    } else {
        emit onOkClick(parentId, cardId, name, shortName, indexId);
    }
    this->close();
}
