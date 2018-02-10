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

void TreeItem::Clicked(QTextEdit *tE) {
    QString user_info = "ID Пользователя: " + item_record.value("userid").toString() + "\n" +
            "ID Родителя: " + item_record.value("parentid").toString() + "\n" +
            "Короткое имя: " + item_record.value("shortname").toString() + "\n" +
            "Имя пользователя: " + item_record.value("viewname").toString() + "\n" +
            "ID Карты: " + item_record.value("cardid").toString() + "\n";
    tE->setText(user_info);
}

int TreeItem::getElemIndex() {
    return item_record.value("userid").toInt();
}
int TreeItem::getParentId() {
    return item_record.value("parentid").toInt();
}
