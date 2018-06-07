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
    m_map = new QMap<int, TreeItem*>;
    not_visible_item = new QList<TreeItem*>;

    for (int i=0; i<data.rowCount(); i++) {
        QList<QVariant> *chld_list = new QList<QVariant>;
        chld_list->append(data.record(i).value("viewname"));
        TreeItem *item = new TreeItem(*chld_list, data.record(i));

        m_map->insert(data.record(i).value("userid").toInt(), item);
        if (item->getParentId() == 0) {
            rootItem->appendChild(item);
        } else {
            not_visible_item->push_back(item);
        }
    }

    //цикл и сопоставление parent и child
    //получился сложный, по возможности переделать
    //работает только c 'правильными' данными
    //если parentId будет такой, которого нет - бесконечный цикл выйдет
    while (!not_visible_item->isEmpty()) {
        QList<int> index_keys = m_map->keys();
        QList<TreeItem*> added_index;
        for (int i = 0; i < not_visible_item->size(); i++){
            TreeItem * nv_item = not_visible_item->value(i);
            int item_parent_id = nv_item->getParentId();
            //здесь другая проверка?
            int idx_in_map = index_keys.indexOf(item_parent_id);
            if (idx_in_map >= 0) {
                TreeItem * it = m_map->value(item_parent_id); //получение родетеля
                it->appendChild(not_visible_item->value(i)); //добавление дочернего
                added_index.push_back(nv_item); //запоминаем элемент, чтобы в дальнейшем удалить
            }
        }

        for(int j = 0; j < added_index.size(); j++) {
            not_visible_item->removeOne(added_index[j]);
        }
        added_index.clear();
    }
}

TreeModel::~TreeModel()
{
    delete rootItem;
    delete items_list;
    delete m_map;
    delete not_visible_item;
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
