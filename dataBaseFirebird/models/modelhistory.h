#ifndef MODELHISTORY_H
#define MODELHISTORY_H

#include <QObject>
#include <QSqlDatabase>

class modelHistory : public QObject
{
    Q_OBJECT
public:
    explicit modelHistory(QObject *parent = nullptr);
    //explicit modelHistory(QSqlDatabase, QObject *parent = nullptr);

signals:

public slots:
};

#endif // MODELHISTORY_H
