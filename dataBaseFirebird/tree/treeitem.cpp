#include "treeitem.h"
#include <QDebug>

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

TreeItem::TreeItem(const QList<QVariant> &data, QSqlRecord record)
{
    m_itemData = data;
    item_record = record;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
    item->m_parentItem = this;
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

//в конструктор передавать ссылку на форму
//в функции выводить на этой форме нужные элементы
void TreeItem::Clicked(QTextEdit *tE) {
    qDebug() << "cl1";
    tE->setText("WWWW");
}

int TreeItem::getElemIndex() {
    return item_record.value("userid").toInt();
}
int TreeItem::getParentId() {
    return item_record.value("parentid").toInt();
}
