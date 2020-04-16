#include "delegatedlinedata.h"
#include "linedelegate.h"
#include <QDebug>

#define LOG qDebug() << Q_FUNC_INFO

LineDelegate::LineDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void LineDelegate::paint(QPainter *painter,
           const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    const QVariant &lineData = index.data(Line::DataRole);
    if(lineData.isValid() && lineData.canConvert<Line>())
    {
        const Line& line = lineData.value<Line>();
        LOG << index.row()<< index.column() << line.from << line.to;
    }
}
