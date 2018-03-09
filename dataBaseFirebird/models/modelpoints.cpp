#include "modelpoints.h"
#include "dialogs/addpointsdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


ModelPoints::ModelPoints(QObject *parent) : QObject(parent)
{
}

ModelPoints::ModelPoints(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
}

void ModelPoints::addPoint() {
    AddPointsDialog * add_point = new AddPointsDialog();
    add_point->show();
    connect(add_point, SIGNAL(onOkClick(int,int,int,int,int)), SLOT(finishAddPoint(int,int,int,int,int)));
}

void ModelPoints::deletePoint(int point_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM points WHERE id=";
    statament.append(QString::number(point_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "RemoveRow:" << query->lastError();
    }
    emit needUpdate();
}

void ModelPoints::editPoint(int point_id) {
    AddPointsDialog * add_point = new AddPointsDialog(point_id, data_base);
    add_point->show();
    connect(add_point, SIGNAL(onOkClick(int, int,int,int,int,int)), SLOT(finishEditPoint(int,int,int,int,int,int)));
}

void ModelPoints::finishAddPoint(int objectId, int dispsId, int sendAddr, int tankId, int workFrag) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "INSERT INTO point (objectid, dispsid, sendaddr, tankid, workflag) VALUES (";
    statament.append(QString::number(objectId) + ", ");
    statament.append(QString::number(dispsId) + ", ");
    statament.append(QString::number(sendAddr) + ", ");
    statament.append(QString::number(tankId) + ", ");
    statament.append(QString::number(workFrag));
    statament.append(")");

    query->exec(statament);
    if (query->lastError().isValid()) {
        //добавить QMessage
        qDebug() << "AddUser:" << query->lastError();
    }
    emit needUpdate();
}

void ModelPoints::finishEditPoint(int pointId, int objectId, int dispsId, int sendAddr, int tankId, int workFrag) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE points SET objectid=%1, dispsid=%2, sendaddr=%3, tankid=%4, workflag=%5 WHERE id=%6")
            .arg(QString::number(objectId))
            .arg(QString::number(dispsId))
            .arg(QString::number(sendAddr))
            .arg(QString::number(tankId))
            .arg(QString::number(workFrag))
            .arg(QString::number(pointId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        //QMessage
        qDebug() << "Error:" << query->lastError();
    }
    emit needUpdate();
}
