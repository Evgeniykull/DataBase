#include "modeluser.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

ModelUser::ModelUser(QObject *parent) : QObject(parent)
{
}

ModelUser::ModelUser(QSqlDatabase db, QObject *parent) : QObject(parent) {
    data_base = db;
    mBx = new QMessageBox();
}

void ModelUser::addUser() {
    addUserDialog * add_user = new addUserDialog();
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int,QString,QString, int, QString)), SLOT(finishAddUser(int,QString,QString, int, QString)));
}

void ModelUser::deleteUsers(int user_id) {
    QSqlQuery* query0 = new QSqlQuery(data_base);
    QString statament0 = QString("SELECT COUNT(userid) FROM users WHERE parentid=%1").arg(QString::number(user_id));
    query0->exec(statament0);
    query0->next();
    if (query0->value(0).toInt() > 0) {
        mBx->setWindowTitle("Удаление пользователя");
        mBx->setText("Невозможно удалить пользователя с зависимостями");
        mBx->show();
        return;
    }

    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("DELETE FROM users WHERE userid=%1").arg(QString::number(user_id));

    query->exec(statament);
    if (query->lastError().isValid()) {
        mBx->setWindowTitle("Удаление пользователя");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }

    QSqlQuery* query2 = new QSqlQuery(data_base);
    QString statament2 = QString("DELETE FROM limits WHERE userid=%1").arg(QString::number(user_id));
    query2->exec(statament2);

    emit needUpdate();
}

void ModelUser::editUsers(int user_id) {
    addUserDialog * add_user = new addUserDialog(user_id, data_base);
    add_user->show();
    connect(add_user, SIGNAL(onOkClick(int,int,QString,QString, int, QString, bool)), SLOT(finishEditUser(int,int,QString,QString, int, QString, bool)));
}

void ModelUser::finishAddUser(int parentId, QString name, QString shortName, int perm, QString date) {
    QString dt = QDate::currentDate().toString("dd.MM.yy");
    if (!date.isEmpty()) {
        dt = date;
    }

    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("INSERT INTO users (parentId, shortname, viewname, cardid, flags, sldate, refslim) VALUES (%1, %2, %3, %4, %5, '%6', 1)")
            .arg(QString::number(parentId))
            .arg("'" + shortName + "'")
            .arg("'" + name + "'")
            .arg("0")
            .arg(QString::number(perm))
            .arg(dt);

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Добавление пользователя");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}

void ModelUser::finishEditUser(int userId, int parentId, QString name, QString shortName, int perm, QString date, bool dateUpd) {
    //date and dt compate
    int dateUpdated = dateUpd ? 0 : 1;

    QSqlQuery* query = new QSqlQuery(data_base);
    QString statament = QString("UPDATE users SET parentid=%1, shortname=%2, viewname=%3, flags=%4, sldate='%5', refslim=%6 WHERE userid=%7")
            .arg(QString::number(parentId))
            .arg("'" +  shortName + "'")
            .arg("'" + name + "'")
            .arg(QString::number(perm))
            .arg(date)
            .arg(dateUpdated)
            .arg(QString::number(userId));

    query->exec(statament);
    if (query->lastError().isValid()) {
        qDebug() << query->lastError().databaseText();
        mBx->setWindowTitle("Редактирование пользователя");
        mBx->setText(query->lastError().databaseText());
        mBx->show();
    }
    emit needUpdate();
}
