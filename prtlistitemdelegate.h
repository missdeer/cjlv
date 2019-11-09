#ifndef PRTLISTITEMDELEGATE_H
#define PRTLISTITEMDELEGATE_H

#include <QAbstractItemDelegate>

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionViewItem;
class QListWidget;
QT_END_NAMESPACE

class PRTListItemDelegate : public QAbstractItemDelegate
{
public:
    explicit PRTListItemDelegate(QListWidget *parent);

    void  paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QListWidget *m_parentListWidget;
};

#endif // PRTLISTITEMDELEGATE_H
