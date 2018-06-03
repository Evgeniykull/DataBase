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
    QDate date = QDate::currentDate();
    ui->dEDate->setDate(date);
}

QString convertData1(QString data) {
    QString newData = data.split("-")[2];
    newData.append(".");
    newData.append(data.split("-")[1]);
    newData.append(".");
    newData.append(data.split("-")[0].mid(2,2));
    return newData;
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
    QString statament = "SELECT PARENTID, SHORTNAME, VIEWNAME, SLDATE, FLAGS FROM users WHERE userid=";
    statament.append(QString::number(user_id));
    query->exec(statament);
    query->next();

    QSqlRecord rec = query->record();
    int perm = rec.value("FLAGS").toInt();
    if (perm > 3) {
        ui->cb3->setChecked(true);
        perm = perm - 4;
    }
    if (perm > 1) {
        ui->cb2->setChecked(true);
        perm = perm - 2;
    }
    if (perm > 0) {
        ui->cb1->setChecked(true);
    }

    ui->leParentId->setText(rec.value("PARENTID").toString());
    ui->leShortName->setText(rec.value("SHORTNAME").toString());
    ui->leViewName->setText(rec.value("VIEWNAME").toString());
    sldate1 = convertData1(rec.value("SLDATE").toString());

    QStringList sl = sldate1.split(".");
    QDate date = QDate::fromString(sl[0]+"."+sl[1]+"."+sl[2], "dd.MM.yy");
    ui->dEDate->setDate(date);
}

addUserDialog::~addUserDialog()
{
    delete ui;
}

void addUserDialog::onButtonOkClick() {
    int parentId = ui->leParentId->text().toInt();
    QString name = ui->leViewName->text();
    QString shortName = ui->leShortName->text();
    int perm = 0;

    if (ui->cb1->isChecked()) {
        perm++;
    }
    if (ui->cb2->isChecked()) {
        perm+=2;
    }
    if (ui->cb3->isChecked()) {
        perm+=4;
    }
    QString SLDate = ui->dEDate->date().toString("dd.MM.yy");

    bool dateUpd = sldate1 == SLDate;

    if (userId > -1) {
        emit onOkClick(userId, parentId, name, shortName, perm, SLDate, dateUpd);
    } else {
        emit onOkClick(parentId, name, shortName, perm, SLDate);
    }
    this->close();
}
