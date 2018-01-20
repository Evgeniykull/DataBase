#include "modeluser.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ModelUser::ModelUser(QObject *parent) : QObject(parent)
{
}

ModelUser::ModelUser(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
}

void ModelUser::addUser() {
    addUserDialog * add_user = new addUserDialog();
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int,int,QString,QString,int)), SLOT(finishAddUser(int,int,QString,QString,int)));
}

void ModelUser::deleteUsers(int user_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM users WHERE userid=";
    statament.append(QString::number(user_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "RemoveRow:" << query->lastError();
    }
    emit needUpdate();
}

void ModelUser::editUsers(int user_id) {
    addUserDialog * add_user = new addUserDialog(user_id, data_base);
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int, int,int,QString,QString,int)), SLOT(finishEditUser(int,int,int,QString,QString,int)));
}

void ModelUser::finishAddUser(int parentId, int cardId, QString name, QString shortName, int indexId) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "INSERT INTO users (parentId, shortname, viewname, cardid, indexid) VALUES (";
    statament.append(QString::number(parentId) + ", ");
    statament.append("'" + shortName + "', ");
    statament.append("'" + name + "', ");
    statament.append(QString::number(cardId) + ", ");
    statament.append(QString::number(indexId));
    statament.append(")");

    query->exec(statament);
    if (query->lastError().isValid()) {
        //добавить QMessage
        qDebug() << "AddUser:" << query->lastError();
    }
    emit needUpdate();
}

void ModelUser::finishEditUser(int userId, int parentId, int cardId, QString name, QString shortName, int indexId) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE users SET parentid=%1, shortname=%2, viewname=%3, cardid=%4, indexid=%5 WHERE userid=%6")
            .arg(QString::number(parentId))
            .arg("'" +  shortName + "'")
            .arg("'" + name + "'")
            .arg(QString::number(cardId))
            .arg(QString::number(indexId))
            .arg(QString::number(userId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "Error:" << query->lastError();
    }
    emit needUpdate();
}
