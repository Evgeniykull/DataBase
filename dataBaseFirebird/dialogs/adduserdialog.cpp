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
    addUserType();
    getParents();
    QDate date = QDate::currentDate();
    ui->dEDate->setDate(date);
    connect(ui->cmbType, SIGNAL(currentTextChanged(QString)), SLOT(typeChanged(QString)));
    typeChanged("Группа");
}

/*QString convertData1(QString data) {
    QString newData = data.split("-")[2];
    newData.append(".");
    newData.append(data.split("-")[1]);
    newData.append(".");
    newData.append(data.split("-")[0].mid(2,2));
    return newData;
}*/

addUserDialog::addUserDialog(int user_id, QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addUserDialog)
{
    ui->setupUi(this);
    userId = user_id;
    dataBase = db;
    addUserType();
    getParents();
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onButtonOkClick()));
    connect(ui->cmbType, SIGNAL(currentTextChanged(QString)), SLOT(typeChanged(QString)));

    QSqlQuery* query = new QSqlQuery(db);
    QString statament = "SELECT PARENTID, SHORTNAME, VIEWNAME, SLDATE, FLAGS, ustype FROM users WHERE userid=";
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

    ui->cmbParent->setCurrentText(parent_map->key(rec.value("PARENTID").toString()));
    ui->leShortName->setText(rec.value("SHORTNAME").toString());
    ui->leViewName->setText(rec.value("VIEWNAME").toString());
    //sldate1 = convertData1(rec.value("SLDATE").toString());

    //QStringList sl = sldate1.split(".");
    //QDate date = QDate::fromString(sl[0]+"."+sl[1]+"."+sl[2], "dd.MM.yy");
    //ui->dEDate->setDate(date);
    ui->cmbType->setCurrentText(us_type->key(rec.value("ustype").toString()));
    typeChanged(us_type->key(rec.value("ustype").toString()));
}

addUserDialog::~addUserDialog()
{
    delete us_type;
    delete parent_map;
    delete ui;
}

void addUserDialog::onButtonOkClick() {
    QString name = ui->leViewName->text();
    QString shortName = ui->leShortName->text();
    QString type = us_type->value(ui->cmbType->currentText());
    int parentId = 0;
    if (type != "0") {
        parentId = parent_map->value(ui->cmbParent->currentText()).toInt();
    }

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
        emit onOkClick(userId, parentId, name, shortName, perm, type, SLDate, dateUpd);
    } else {
        emit onOkClick(parentId, name, shortName, perm, type, SLDate);
    }
    this->close();
}

void addUserDialog::getParents() {
    parent_map = new QMap<QString, QString>;
    QSqlQuery* query = new QSqlQuery(dataBase);
    QString statament = "SELECT USERID, SHORTNAME FROM users WHERE ustype=0";
    query->exec(statament);

    ui->cmbParent->clear();
    while(query->next()) {
        parent_map->insert(query->record().value("SHORTNAME").toString(), query->record().value("PARENTID").toString());
        ui->cmbParent->addItem(query->record().value("SHORTNAME").toString());
    }
}

void addUserDialog::typeChanged(QString type) {
    if (type == "Группа") {
        ui->cmbParent->setDisabled(true);
    } else {
        ui->cmbParent->setDisabled(false);
    }
}

void addUserDialog::addUserType() {
    us_type = new QMap<QString, QString>;
    us_type->insert("Группа", "0");
    us_type->insert("Пользователь", "1");
    us_type->insert("Машина", "2");
    us_type->insert("Накладная", "3");
}
