#ifndef MODELLIMIT_H
#define MODELLIMIT_H

#include <QObject>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QMessageBox>
#include "dialogs/addlimitsdialog.h"

class ModelLimit : public QObject
{
    Q_OBJECT
public:
    explicit ModelLimit(QObject *parent = nullptr);
    explicit ModelLimit(QSqlDatabase, QString, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addLimits();
    void deleteLimits(int);
    void editLimits(int);

private slots:
    void finishAddLimits(QString, QString, QString, QString);
    void finishEditLimits(int, QString, QString, QString, QString);

private:
    QSqlDatabase data_base;
    QMessageBox *mBx;
    QString userId;
};

#endif // MODELLIMIT_H
