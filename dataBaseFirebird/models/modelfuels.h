#ifndef MODELFUELS_H
#define MODELFUELS_H

#include <QObject>
#include <QTableView>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include "dialogs/addfuelsdialog.h"

class ModelFuels : public QObject
{
    Q_OBJECT
public:
    explicit ModelFuels(QObject *parent = nullptr);
    explicit ModelFuels(QSqlDatabase, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addFuels();
    void deleteFuels(int);
    void editFuels(int);
    QList<QString> configureFuels();

private slots:
    void finishAddFuels(QString, QString, QString);
    void finishEditFuels(int, QString, QString, QString);

private:
    QSqlDatabase data_base;
    QMessageBox *mBx;
};

#endif
