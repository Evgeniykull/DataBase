#ifndef MODELTANKS_H
#define MODELTANKS_H

#include <QObject>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include "dialogs/addtanksdialog.h"

class ModelTanks : public QObject
{
    Q_OBJECT
public:
    explicit ModelTanks(QObject *parent = nullptr);
    explicit ModelTanks(QSqlDatabase, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addTanks();
    void deleteTanks(int);
    void editTanks(int);

private slots:
    void finishAddTanks(int, int, int, QString);
    void finishEditTanks(int, int, int, int, QString);

private:
    QSqlDatabase data_base;
};

#endif // MODELTANKS_H
