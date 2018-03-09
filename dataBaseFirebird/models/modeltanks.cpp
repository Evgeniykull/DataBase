#include "modeltanks.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


ModelTanks::ModelTanks(QObject *parent) : QObject(parent)
{
}

ModelTanks::ModelTanks(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
}

void ModelTanks::addTanks() {
    AddTanksDialog * add_tank = new AddTanksDialog();
    add_tank->show();
    connect(add_tank, SIGNAL(onOkClick(int,int,int,QString)), SLOT(finishAddTanks(int,int,int,QString)));
}

void ModelTanks::deleteTanks(int user_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM tanks WHERE id=";
    statament.append(QString::number(user_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "RemoveRow:" << query->lastError();
    }
    emit needUpdate();
}

void ModelTanks::editTanks(int tank_id) {
    AddTanksDialog * add_tank = new AddTanksDialog(tank_id, data_base);
    add_tank->show();
    connect(add_tank, SIGNAL(onOkClick(int,int,int,int,QString)), SLOT(finishEditTanks(int,int,int,int,QString)));
}

void ModelTanks::finishAddTanks(int objectId, int tankId, int addr, QString comment) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "INSERT INTO tanks (objectId, tankid, sendaddr, comment) VALUES (";
    statament.append(QString::number(objectId) + ", ");
    statament.append(QString::number(tankId) + ", ");
    statament.append(QString::number(addr) + ", ");
    statament.append("'" + comment + "'");
    statament.append(")");

    query->exec(statament);
    if (query->lastError().isValid()) {
        //добавить QMessage
        qDebug() << "AddUser:" << query->lastError();
    }
    emit needUpdate();
}

void ModelTanks::finishEditTanks(int id, int objectId, int tankId, int addr, QString comment) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE tanks SET objectid=%1, tankid=%2, sendaddr=%3, comment=%4 WHERE id=%6")
            .arg(QString::number(objectId))
            .arg(QString::number(tankId))
            .arg(QString::number(addr))
            .arg("'" + comment + "'")
            .arg(QString::number(id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "Error:" << query->lastError();
    }
    emit needUpdate();
}
