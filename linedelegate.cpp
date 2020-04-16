#include "delegatedlinedata.h"
#include "linedelegate.h"

#include <QPainter>
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
        const QLineF &lineF = line.toQLine(option.rect);
        LOG << index.row()<< index.column() << option.rect << line.from << line.to << lineF;

        painter->save();

        QPen marker(Qt::darkGreen);
        marker.setWidthF(2.);
        painter->setPen(marker);
        painter->drawLine(lineF);

        marker.setColor(Qt::red);
        marker.setWidthF(1.5);
        painter->setPen(marker);
        painter->drawRect(option.rect);

        painter->restore();
    }
}
