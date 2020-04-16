#include <QObject>
#include <qnamespace.h>

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
};

Q_DECLARE_METATYPE(Line);
