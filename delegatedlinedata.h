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

#include <QObject>
#include <QLineF>
#include <QRect>

#pragma once

/*!
 * \brief The Line struct is a wrapper for a line to be drawn by delegate
 * \a from & \a to are the X coordinates of start and end point in cell in logical range [0.0, 1.0].
 */

struct Line
{
    static constexpr int DataRole = Qt::UserRole+1;
    qreal from;
    qreal to;

    bool isEmpty() const {return qFuzzyCompare(from,to);}
    bool isFull() const {return !isEmpty() && qFuzzyIsNull(from) && qFuzzyCompare(1., to);}

    QLineF toQLine( const QRect& viewport ) const
    {
        const QPoint &center = viewport.center();
        QPointF p1(viewport.left(), center.y());
        QPointF p2(viewport.right(), center.y());

        if(!isFull())
        {
            p1.rx() = viewport.left() + viewport.width()*qreal(from);
            p2.rx() = viewport.left() + viewport.width()*qreal(to);
        }

        return {p1,p2};
    }
};

Q_DECLARE_METATYPE(Line);
