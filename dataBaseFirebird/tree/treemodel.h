#ifndef TREEMODEL_H
#define TREEMODEL_H
#include <QVariant>
#include <QTextEdit>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QSqlRelationalTableModel>
#include "tree/treeitem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(const QSqlRelationalTableModel &data, QTextEdit *tE, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void Clicked(QModelIndex);

private:
    //void setupModelData(const QStringList &lines, TreeItem *parent);
    TreeItem *rootItem;
    QMap<int, TreeItem> *m_map;

    struct ITEMS {
        int id;
        int parentId;
        TreeItem *item;
    };
    QList<ITEMS> *items_list;
    QTextEdit * textEdit;
};

#endif // TREEMODEL_H
