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

#pragma once

#include <QLineF>
#include <QModelIndex>
#include <QObject>
#include <QPair>
#include <QRectF>

class Line
{
public:
    struct Point {
        Point(const QModelIndex &id = QModelIndex(), const qreal percent = -1)
            : m_index(id)
            , m_percent(percent)
        {
        }

        QModelIndex m_index {};
        qreal m_percent { -1. };

        bool isValid() const { return m_index.isValid() && m_percent >= 0; }

        QPointF toQPointF(const QRectF &viewport) const;
    };

    Line(const Line::Point &start = Point(), const Line::Point &end = Point());

    bool isComplete() const;

    Point &start();
    Point &end();

    const Point &constStart() const;
    const Point &constEnd() const;

    bool isSelected() const;
    void setSelected(bool selected);

    bool intersects(const QRectF &rect) const;

    QLineF graphicLine() const;

    void updateItemViewStart(const QRectF &rect);
    void updateItemViewEnd(const QRectF &rect);

private:
    Point m_start {};
    Point m_end {};
    bool m_selected { false };

    QRectF m_itemViewStart {}, m_itemViewEnd {};
    QLineF m_line;
};
