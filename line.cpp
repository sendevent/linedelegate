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

#include "line.h"

#include <QRectF>

QPointF Line::Point::toQPointF(const QRectF &viewport) const
{
    const qreal x = viewport.left() + viewport.width() * qreal(m_percent);
    const qreal y = viewport.center().y();

    return { x, y };
}

Line::Line(const Line::Point &start, const Line::Point &end)
    : m_start(start)
    , m_end(end)
    , m_selected(false)
{
}

bool Line::isComplete() const
{
    return constStart().isValid() && constEnd().isValid() && !graphicLine().isNull();
}

Line::Point &Line::start()
{
    return m_start;
}

const Line::Point &Line::constStart() const
{
    return m_start;
}

Line::Point &Line::end()
{
    return m_end;
}

const Line::Point &Line::constEnd() const
{
    return m_end;
}

bool Line::isSelected() const
{
    return m_selected;
}

void Line::setSelected(bool selected)
{
    m_selected = selected;
}

QLineF Line::graphicLine() const
{
    if (m_line.p1() == m_line.p2())
        return {};

    return m_line;
}

bool Line::intersectsProlonged(const QRectF &rect) const
{
    return intersects(rect, { QLineF::BoundedIntersection, QLineF::UnboundedIntersection });
}

bool Line::intersectsStrict(const QRectF &rect) const
{
    return intersects(rect, { QLineF::BoundedIntersection });
}

bool Line::intersects(const QRectF &rect, const QVector<int> &intersectionTypes) const
{
    if (rect.isNull())
        return false;

    const QLineF &myLine = graphicLine();
    if (myLine.isNull())
        return false;

    const QVector<QLineF> edges {
        { rect.topLeft(), rect.topRight() },
        { rect.topRight(), rect.bottomRight() },
        { rect.bottomRight(), rect.bottomLeft() },
        { rect.bottomLeft(), rect.topLeft() },
    };

    for (const auto edge : edges)
        if (intersectionTypes.contains(myLine.intersects(edge, nullptr)))
            return true;

    return false;
}

void Line::updateItemViewStart(const QRectF &rect)
{
    m_itemViewStart = rect;
    m_line.setP1(m_start.toQPointF(m_itemViewStart));
}
void Line::updateItemViewEnd(const QRectF &rect)
{
    m_itemViewEnd = rect;
    m_line.setP2(m_end.toQPointF(m_itemViewEnd));
}
