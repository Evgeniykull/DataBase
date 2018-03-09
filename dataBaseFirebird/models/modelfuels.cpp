#include "modelfuels.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ModelFuels::ModelFuels(QObject *parent) : QObject(parent)
{
}

ModelFuels::ModelFuels(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
}

void ModelFuels::addFuels() {
    AddFuelsDialog * add_fuels = new AddFuelsDialog();
    add_fuels->show();
    connect(add_fuels, SIGNAL(onOkClick(QString,QString,int)), SLOT(finishAddFuels(QString,QString,int)));
}

void ModelFuels::deleteFuels(int user_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM fuels WHERE fueldid=";
    statament.append(QString::number(user_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "RemoveRow:" << query->lastError();
    }
    emit needUpdate();
}

void ModelFuels::editFuels(int user_id) {
    AddFuelsDialog * add_fuels = new AddFuelsDialog(user_id, data_base);
    add_fuels->show();
    connect(add_fuels, SIGNAL(onOkClick(int,QString,QString,int)), SLOT(finishEditFuels(int,QString,QString,int)));
}

void ModelFuels::finishAddFuels(QString name, QString viewName, int price) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "INSERT INTO fuels (name, viewname, price) VALUES (";
    statament.append("'" + name + "', ");
    statament.append("'" + viewName + "', ");
    statament.append(QString::number(price));
    statament.append(")");

    query->exec(statament);
    if (query->lastError().isValid()) {
        //добавить QMessage
        qDebug() << "AddUser:" << query->lastError();
    }
    emit needUpdate();
}

void ModelFuels::finishEditFuels(int fuelId, QString name, QString viewName, int price) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE fuels SET name=%1, viewname=%2, price=%3 WHERE fueldid=%4")
            .arg("'" + name + "'")
            .arg("'" +  viewName + "'")
            .arg(QString::number(price))
            .arg(QString::number(fuelId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "Error:" << query->lastError();
    }
    emit needUpdate();
}

QList<QString> ModelFuels::configureFuels() {
    QList<QString> *fnsh = new QList<QString>;
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "SELECT * FROM fuels";
    query->exec(statament);

    int i = 0;
    while(query->next()) {
        QString data = QString("set Установки.ВидыТоплива[%1]:").arg(QString::number(i));
        QString json_data = QString("{Топливо:%1 Цена:%2 Наименование:\"%3\"}")
                .arg(query->value(0).toString())
                .arg(query->value(3).toString())
                .arg(query->value(2).toString());

        data += json_data;
        fnsh->push_back(data);
        i++;
    }
    return *fnsh;
}
