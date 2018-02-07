#include "treemodel.h"
#include <QSqlRecord>
#include <QDebug>

TreeModel::TreeModel(const QSqlRelationalTableModel &data, QTextEdit *tE, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> lis;
    lis.append("Пользователи");
    rootItem = new TreeItem(lis);
    items_list  = new QList<ITEMS>;
    textEdit = tE;

    for (int i=0; i<data.rowCount(); i++) {
        QList<QVariant> *chld_list = new QList<QVariant>;
        chld_list->append(data.record(i).value("viewname"));
        TreeItem *item = new TreeItem(*chld_list, data.record(i));

        ITEMS *new_item = new ITEMS();
        new_item->id = data.record(i).value("userid").toInt();
        new_item->parentId = data.record(i).value("parentid").toInt();
        new_item->item = item;

        items_list->push_back(*new_item);
        if (item->getParentId() == 0) {
            rootItem->appendChild(item);
        }
    }

    //цикл по map и сопоставление parent и child
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

void TreeModel::Clicked(QModelIndex index) {
    if (!index.isValid())
        return;

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    childItem->Clicked(textEdit);
}
