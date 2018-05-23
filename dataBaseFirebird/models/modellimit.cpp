#include "modellimit.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

ModelLimit::ModelLimit(QObject *parent) : QObject(parent)
{
}

ModelLimit::ModelLimit(QSqlDatabase db, QString user_id, QObject *parent) : QObject(parent) {
    data_base = db;
    mBx = new QMessageBox();
    userId = user_id;
}

void ModelLimit::addLimits() {
    AddLimitsDialog * add_user = new AddLimitsDialog();
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(QString,QString,QString,QString)), SLOT(finishAddLimits(QString,QString,QString,QString)));
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
    connect(add_user, SIGNAL(onOkClick(int,QString,QString,QString,QString)), SLOT(finishEditLimits(int,QString,QString,QString,QString)));
}

void ModelLimit::finishAddLimits(QString fuelName, QString value, QString days, QString type) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO limits (userid, fuelid, valuel, days, typed) VALUES (%1, %2, %3, %4, %5)")
                .arg(userId)
                .arg(fuelName)
                .arg(value)
                .arg(days)
                .arg(type);

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Добавление лимита");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    } else {
        QString dt = QDate::currentDate().toString("dd.MM.yy");
        QSqlQuery* query2 = new QSqlQuery(data_base);
        QString statament2 = QString("UPDATE users SET sldate='%1', refslim=1 WHERE userid=%2").arg(dt).arg(userId);
        query2->exec(statament2);
    }
    emit needUpdate();
}

void ModelLimit::finishEditLimits(int limitsId, QString fuelName, QString value, QString days, QString type) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE limits SET userid=%1, fuelid=%2, valuel=%3, days=%4, typed=%5  WHERE id=%6")
                .arg(userId)
                .arg(fuelName)
                .arg(value)
                .arg(days)
                .arg(type)
                .arg(QString::number(limitsId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Редактирование лимита");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    } else {
        QString dt = QDate::currentDate().toString("dd.MM.yy");
        QSqlQuery* query2 = new QSqlQuery(data_base);
        QString statament2 = QString("UPDATE users SET sldate='%1', refslim=1 WHERE userid=%2").arg(dt).arg(userId);
        query2->exec(statament2);
    }
    emit needUpdate();
}
