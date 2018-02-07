#ifndef TREEITEM_H
#define TREEITEM_H
#include <QList>
#include <QSqlRecord>
#include <QTextEdit>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
    explicit TreeItem(const QList<QVariant> &data, QSqlRecord record);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();

    int getElemIndex();
    int getParentId();

public slots:
    void Clicked(QTextEdit *tE);

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
    QSqlRecord item_record;
};

#endif // TREEITEM_H
