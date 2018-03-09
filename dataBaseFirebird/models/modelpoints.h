#ifndef MODELPOINTS_H
#define MODELPOINTS_H

#include <QObject>
#include <QTableView>
#include <QtSql/QSqlDatabase>
#include <QSqlRelationalTableModel>

class ModelPoints : public QObject
{
    Q_OBJECT
public:
    explicit ModelPoints(QObject *parent = nullptr);
    explicit ModelPoints(QSqlDatabase, QObject *parent = nullptr);

signals:
    void needUpdate();

public slots:
    void addPoint();
    void deletePoint(int);
    void editPoint(int);

private slots:
    void finishAddPoint(int, int, int, int, int);
    void finishEditPoint(int, int, int, int, int, int);

private:
    QSqlDatabase data_base;
};

#endif // MODELPOINTS_H
