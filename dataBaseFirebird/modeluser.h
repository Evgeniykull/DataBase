#ifndef MODELUSER_H
#define MODELUSER_H

#include <QObject>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include "adduserdialog.h"

class ModelUser : public QObject
{
    Q_OBJECT
public:
    explicit ModelUser(QObject *parent = nullptr);
    explicit ModelUser(QSqlDatabase, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addUser();
    void deleteUsers(int);
    void editUsers(int);

private slots:
    void finishAddUser(int, int, QString, QString, int);
    void finishEditUser(int, int, int, QString, QString, int);

private:
    QSqlDatabase data_base;
};

#endif // MODELUSER_H
