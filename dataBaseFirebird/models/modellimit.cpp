#include "modellimit.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>
#include <QSqlRecord>

ModelLimit::ModelLimit(QObject *parent) : QObject(parent)
{
}

ModelLimit::ModelLimit(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
    mBx = new QMessageBox();
}

void ModelLimit::addLimits() {
    AddLimitsDialog * add_user = new AddLimitsDialog();
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(QString,QString,QString,QString, QString)), SLOT(finishAddLimits(QString,QString,QString,QString, QString)));
}

void ModelLimit::deleteLimits(int limits_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("DELETE FROM limits WHERE id=%1").arg(QString::number(limits_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление лимита");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelLimit::editLimits(int limits_id) {
    AddLimitsDialog * add_user = new AddLimitsDialog(limits_id, data_base);
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int,QString,QString,QString,QString, QString)), SLOT(finishEditLimits(int,QString,QString,QString,QString, QString)));
}

void ModelLimit::finishAddLimits(QString fuelName, QString value, QString days, QString type, QString endDate) {
    QSqlQuery _limq(data_base);
    QString st = QString("SELECT NEWLIMGROUP FROM USERS WHERE userid=%1").arg(userId);
    _limq.exec(st);
    _limq.first();
    int newlimgr = _limq.record().value("NEWLIMGROUP").toInt();
    if (newlimgr == 0) {
        QSqlQuery _q(data_base);
        _q.exec("SELECT MAX(LIMGROUP) FROM limits");
        _q.first();
        newlimgr = _q.record().value(0).toInt() + 1;
    }

    QSqlQuery query(data_base);
    QString statament = QString("INSERT INTO limits (userid, fuelid, valuel, daysd, typed, enddate, limgroup) VALUES (%1, %2, %3, %4, %5, '%6', %7)")
            .arg(userId)
            .arg(fuelName)
            .arg(value)
            .arg(days)
            .arg(type)
            .arg(endDate)
            .arg(QString::number(newlimgr));

    query.exec(statament);
    if (query.lastError().isValid()) {
        qDebug() << query.lastError().databaseText();
        mBx->setWindowTitle("Добавление лимита");
        mBx->setText(query.lastError().databaseText());
        mBx->show();
    } else {
        //QString dt = QDate::currentDate().toString("dd.MM.yy");
        QSqlQuery* query2 = new QSqlQuery(data_base);
        QString statament2 = QString("UPDATE users SET NEWLIMGROUP=%1 WHERE userid=%2").arg(QString::number(newlimgr)).arg(userId);
        //QString statament2 = QString("UPDATE users SET sldate='%1', NEWLIMGROUP=%2 WHERE userid=%3").arg(dt).arg(newLimgroup).arg(userId);
        query2->exec(statament2);
    }
    emit needUpdate();
}

void ModelLimit::finishEditLimits(int limitsId, QString fuelName, QString value, QString days, QString type, QString endDate) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE limits SET userid=%1, fuelid=%2, valuel=%3, daysd=%4, typed=%5, enddate='%6'  WHERE id=%7")
            .arg(userId)
            .arg(fuelName)
            .arg(value)
            .arg(days)
            .arg(type)
            .arg(endDate)
            .arg(QString::number(limitsId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Редактирование лимита");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    } else {
        /*QString dt = QDate::currentDate().toString("dd.MM.yy");
        QSqlQuery* query2 = new QSqlQuery(data_base);
        QString statament2 = QString("UPDATE users SET sldate='%1', refslim=1 WHERE userid=%2").arg(dt).arg(userId);
        query2->exec(statament2);*/
    }
    emit needUpdate();
}

void ModelLimit::setUserId(QString user_id) {
    userId = user_id;
}
