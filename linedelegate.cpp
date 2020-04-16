#include "linedelegate.h"

LineDelegate::LineDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void LineDelegate::paint(QPainter *painter,
           const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);
}
