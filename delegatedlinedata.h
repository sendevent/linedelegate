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
