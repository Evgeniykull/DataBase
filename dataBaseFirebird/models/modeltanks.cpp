#include "modeltanks.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ModelTanks::ModelTanks(QObject *parent) : QObject(parent)
{
}

ModelTanks::ModelTanks(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
    mBx = new QMessageBox();
}

void ModelTanks::addTanks() {
    AddTanksDialog * add_tank = new AddTanksDialog(data_base);
    add_tank->show();
    connect(add_tank, SIGNAL(onOkClick(int,int,int,QString)), SLOT(finishAddTanks(int, int,int,QString)));
}

void ModelTanks::deleteTanks(int user_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM tanks WHERE id=";
    statament.append(QString::number(user_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление резервуара");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelTanks::editTanks(int tank_id) {
    AddTanksDialog * add_tank = new AddTanksDialog(tank_id, data_base);
    add_tank->show();
    connect(add_tank, SIGNAL(onOkClick(int,int,int,QString)), SLOT(finishEditTanks(int,int,int,QString)));
}

void ModelTanks::finishAddTanks(int id, int fuelId, int addr, QString comment) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO tanks (id, fuelId, sendAddr, objectId, comment) VALUES (%1, %2, %3, %4, %5)")
            .arg(QString::number(id))
            .arg(QString::number(fuelId))
            .arg(QString::number(addr))
            .arg(azs_num)
            .arg("'" + comment + "'");

    qDebug() << statament;
    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Добавление резервуара");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelTanks::finishEditTanks(int id, int fuelId, int addr, QString comment) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE tanks SET fuelid=%1, sendaddr=%2, objectid=%3, comment=%4 WHERE id=%5")
            .arg(QString::number(fuelId))
            .arg(QString::number(addr))
            .arg(azs_num)
            .arg("'" + comment + "'")
            .arg(QString::number(id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Редактирование резервуара");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelTanks::setAzsNum(QString azsNum) {
    azs_num = azsNum;
}

/*QList<QString> ModelTanks::configureTanks() {
    QList<QString> *fnsh = new QList<QString>;
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "SELECT ID,FUELID,SENDADDR FROM tanks";
    query->exec(statament);

    int i = 0;
    while(query->next()) {
        QString data = QString("set Установки.Резервуары[%1]:").arg(QString::number(i));
        QString json_data = QString("{Резервуар:%1 Топливо:%2 Адрес:%3}")
                .arg(query->value(0).toString())
                .arg(query->value(1).toString())
                .arg(query->value(2).toString());

        data += json_data;
        fnsh->push_back(data);
        i++;
        if (i == 8) {
            break;
        }
    }

    while (i < 8) {
        QString data = QString("set Установки.Резервуары[%1]:{Резервуар:0 Топливо:0 Адрес:0}").arg(QString::number(i));
        fnsh->push_back(data);
        i++;
    }

    return *fnsh;
}*/
