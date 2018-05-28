#include "modelobject.h"
#include <QSqlQuery>
#include <QSqlError>
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
    connect(add_object, SIGNAL(onOkClick(QString,QString,QString)), SLOT(finishAddObject(QString,QString,QString)));
}

void ModelObject::deleteObject(int object_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("DELETE FROM OBJECT_TABLES WHERE OBJECTID=%1").arg(QString::number(object_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление объекта");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelObject::editObject(int user_id) {
    AddObjectDialog * add_object = new AddObjectDialog(user_id, data_base);
    add_object->show();
    connect(add_object, SIGNAL(onOkClick(QString,QString,QString)), SLOT(finishEditObject(QString,QString,QString)));
}

void ModelObject::finishAddObject(QString objectId, QString name, QString pass) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO OBJECT_TABLES (objectid, objectName, ADMINPASSWORD) VALUES ('%1', '%2', %3)")
            .arg(objectId)
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
