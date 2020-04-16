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
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#define LOG qDebug() << Q_FUNC_INFO

LineDelegate::LineDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , linePen(Qt::darkGreen)
    , framePen(Qt::red)
{
    linePen.setWidthF(2.);
    framePen.setWidthF(1.5);
}

void LineDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const QVariant &lineData = index.data(Line::DataRole);
    if (lineData.isValid() && lineData.canConvert<Line>()) {
        const Line &line = lineData.value<Line>();
        const QLineF &lineF = line.toQLine(option.rect);
        //        LOG << index.row() << index.column() << option.rect << line.from << line.to << lineF;

        painter->save();

        painter->setPen(linePen);
        painter->drawLine(lineF);

        painter->setPen(framePen);
        painter->drawRect(option.rect);

        painter->restore();
    }
}

bool LineDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                               const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        handleMousePress(static_cast<QMouseEvent *>(event), model, option, index);
        break;
    case QEvent::MouseMove:
        handleMouseMove(static_cast<QMouseEvent *>(event), model, option, index);
        break;
    case QEvent::MouseButtonRelease:
        handleMouseRelease(static_cast<QMouseEvent *>(event), model, option, index);
        break;
    default:
        break;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void LineDelegate::handleMousePress(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                    const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(model);

    if (index.isValid()) {
        startIndex = index;
        startPoint = { event->x() - option.rect.x(), option.rect.center().y() };
    }
}

void LineDelegate::handleMouseMove(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    const QPoint endPoint = { event->x() - option.rect.x(), option.rect.center().y() };
    if (startIndex != index) {
        startIndex = index;
        startPoint = endPoint;
    }

    //    LOG << startPoint << endPoint;
    const Line &line = Line::fromQLine({ startPoint, endPoint }, option.rect);

    model->setData(index, QVariant::fromValue(line), Line::DataRole);
}

void LineDelegate::handleMouseRelease(QMouseEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                      const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(model);
    Q_UNUSED(option);
    Q_UNUSED(index);

    startIndex = QModelIndex();
    startPoint = { -1, -1 };
}
