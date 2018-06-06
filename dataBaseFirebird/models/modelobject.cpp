#include "modelobject.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

ModelObject::ModelObject(QObject *parent) : QObject(parent)
{
}

ModelObject::ModelObject(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
    mBx = new QMessageBox();
}

void ModelObject::addObject() {
    AddObjectDialog * add_object = new AddObjectDialog();
    add_object->show();
    connect(add_object, SIGNAL(onOkClick(QString,QString)), SLOT(finishAddObject(QString,QString)));
}

void ModelObject::deleteObject(int object_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QSqlQuery* tank_query = new QSqlQuery(data_base);
    QSqlQuery* point_query = new QSqlQuery(data_base);
    QString tank_bd_query = QString("SELECT COUNT(*) FROM TANKS WHERE OBJECTID=%1").arg(QString::number(object_id));
    QString point_bd_query = QString("SELECT COUNT(*) FROM POINTS WHERE OBJECTID=%1").arg(QString::number(object_id));

    tank_query->exec(tank_bd_query);
    point_query->exec(point_bd_query);

    tank_query->next();
    point_query->next();

    if (tank_query->record().value(0).toInt() != 0 || point_query->record().value(0).toInt() != 0) {
        mBx->setWindowTitle("Удаление объекта");
        mBx->setText("У объекта есть зависимости");
        mBx->show();
        return;
    }

    QString statament = QString("DELETE FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(object_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление объекта");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelObject::editObject(int object_id) {
    AddObjectDialog * add_object = new AddObjectDialog(object_id, data_base);
    add_object->show();
    connect(add_object, SIGNAL(onOkClick(QString,QString,QString)), SLOT(finishEditObject(QString,QString,QString)));
}

void ModelObject::finishAddObject(QString name, QString pass) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO OBJECT_TABLES (objectName, ADMINPASSWORD) VALUES ('%1', '%2')")
            .arg(name)
            .arg(pass);

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Добавление объекта");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelObject::finishEditObject(QString objectId, QString name, QString pass) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE OBJECT_TABLES SET objectName=%1, ADMINPASSWORD=%2 WHERE OBJECTID=%3")
            .arg("'" + name + "'")
            .arg("'" +  pass + "'")
            .arg(objectId);

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Редактирование объекта");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

QString ModelObject::getSettings(int object_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("SELECT CONNECTIONPROPERTY FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(object_id));
    query->exec(statament);
    query->next();
    return query->record().value(0).toString();
}

void ModelObject::setSettings(int object_id, QString sett) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE OBJECT_TABLES SET CONNECTIONPROPERTY='%1' WHERE OBJECTID=%2")
            .arg(sett)
            .arg(QString::number(object_id));
    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Обновление настроек объекта");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}
