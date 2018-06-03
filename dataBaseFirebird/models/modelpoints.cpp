#include "modelpoints.h"
#include "dialogs/addpointsdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ModelPoints::ModelPoints(QObject *parent) : QObject(parent)
{
}

ModelPoints::ModelPoints(QSqlDatabase db, QString azsNum, QObject *parent) : QObject(parent) {
    data_base = db;
    azs_num = azsNum;
    mBx = new QMessageBox();
}

void ModelPoints::addPoint() {
    AddPointsDialog * add_point = new AddPointsDialog(azs_num, data_base);
    add_point->show();
    connect(add_point, SIGNAL(onOkClick(int,int,int,int)), SLOT(finishAddPoint(int,int,int,int)));
}

void ModelPoints::deletePoint(int point_id) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "DELETE FROM points WHERE id=";
    statament.append(QString::number(point_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление колонки");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelPoints::editPoint(int point_id) {
    AddPointsDialog * add_point = new AddPointsDialog(azs_num, point_id, data_base);
    add_point->show();
    connect(add_point, SIGNAL(onOkClick(int,int,int,int,int)), SLOT(finishEditPoint(int,int,int,int,int)));
}

void ModelPoints::finishAddPoint(int dispsId, int sendAddr, int tankId, int workFrag) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO points (objectid, dispsid, sendaddr, tankid, workflag) VALUES (%1, %2, %3, %4, %5)")
            .arg(azs_num)
            .arg(QString::number(dispsId))
            .arg(QString::number(sendAddr))
            .arg(QString::number(tankId))
            .arg(QString::number(workFrag));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Добавление колонки");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelPoints::finishEditPoint(int pointId, int dispsId, int sendAddr, int tankId, int workFrag) {
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE points SET objectid=%1, dispsid=%2, sendaddr=%3, tankid=%4, workflag=%5 WHERE id=%6")
            .arg(azs_num)
            .arg(QString::number(dispsId))
            .arg(QString::number(sendAddr))
            .arg(QString::number(tankId))
            .arg(QString::number(workFrag))
            .arg(QString::number(pointId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Редактирование колонки");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

#include <QSet>
QList<QString> ModelPoints::configurePoints() {
    QList<QString> *fnsh = new QList<QString>;
    QSet<int> *disps_id_set = new QSet<int>;
    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = "SELECT * FROM points";
    query->exec(statament);

    while(query->next()) {
        disps_id_set->insert(query->value(2).toInt());
    }
    query->first();

    QList<int> disps_id_list = disps_id_set->toList();
    int p = 0;
    for (int i=0; i<disps_id_list.size(); i++) {
        p++;
        QString statament2 = QString("SELECT * FROM points WHERE dispsid=%1").arg(disps_id_list[i]);
        query->exec(statament2);

        QString data = QString("set Установки.Колонки[%1]:{Рукава[]:[").arg(QString::number(i));
        QString mid_data="";
        int j = 0;
        while(query->next()) {
            j++;
            mid_data += QString("{Работа:%1 Адрес:%2 Резервуар:%3},")
                    .arg(query->value(5).toString())
                    .arg(query->value(3).toString())
                    .arg(query->value(4).toString());
            if (j == 5) {
                break;
            }
        }
        while (j < 5) {
            mid_data += "{Работа:0 Адрес:0 Резервуар:0},";
            j++;
        }

        mid_data = mid_data.mid(0, mid_data.length()-1);
        data += mid_data;
        data.append("]}");
        fnsh->push_back(data);
        if (p == 12) {
            break;
        }
    }

    while (p < 12) {
        QString data = QString("set Установки.Колонки[%1]:{Рукава[]:[").arg(QString::number(p));
        for (int i=0; i<5; i++) {
            data+= "{Работа:0 Адрес:0 Резервуар:0},";
        }
        data = data.mid(0, data.length()-1);
        data.append("]}");
        fnsh->push_back(data);
        p++;
    }

    return *fnsh;
}
