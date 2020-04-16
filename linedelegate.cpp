/*
   Copyright (C) 2020 Denis Gofman - <sendevent@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program. If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#include "linedelegate.h"

#include "delegatedlinedata.h"

#include <QDebug>
#include <QPainter>

#define LOG qDebug() << Q_FUNC_INFO

LineDelegate::LineDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void LineDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const QVariant &lineData = index.data(Line::DataRole);
    if (lineData.isValid() && lineData.canConvert<Line>()) {
        const Line &line = lineData.value<Line>();
        const QLineF &lineF = line.toQLine(option.rect);
        LOG << index.row() << index.column() << option.rect << line.from << line.to << lineF;

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
