#ifndef MODELOBJECT_H
#define MODELOBJECT_H

#include <QObject>
#include <QTableView>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include "dialogs/addobjectdialog.h"

class ModelObject : public QObject
{
    Q_OBJECT
public:
    explicit ModelObject(QObject *parent = nullptr);
    explicit ModelObject(QSqlDatabase, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addObject();
    void deleteObject(int);
    void editObject(int);

private slots:
    void finishAddObject(QString, QString, QString);
    void finishEditObject(QString, QString, QString);

private:
    QSqlDatabase data_base;
    QMessageBox *mBx;
};

#endif // MODELOBJECT_H
