#ifndef MODELUSER_H
#define MODELUSER_H

#include <QObject>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QMessageBox>
#include "dialogs/adduserdialog.h"

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
    void finishAddUser(int, QString, QString, int, QString);
    void finishEditUser(int, int, QString, QString, int, QString);

private:
    QSqlDatabase data_base;
    QMessageBox *mBx;
    QString SLDate;
};

#endif // MODELUSER_H
