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

#include "line.h"

#include <QPainterPath>
#include <QPen>
#include <QTableView>
#include <QVector>

class QPainter;
class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private slots:
    void updateGraphicLines();

private:
    QVector<Line *> m_lines {};
    QPen m_marker { Qt::darkGreen };
    QPen m_highlighter { Qt::blue };
    QPen m_gripPen { Qt::yellow };

    Line *m_currLine { nullptr };

    QPainterPath m_gripStart;
    QPainterPath m_gripEnd;
    QPainterPath *m_gripActive { nullptr };

    void drawLines(QPainter *painter, const QRectF &rect);
    void drawLine(QPainter *painter, const Line *line);

    Line *createLine(const QPointF &start);
    void commitLine(Line *line);

    Line::Point linePointForPos(const QPointF &pos) const;

    void createNewLine(const QPointF &pos);
    bool selectLine(const QPointF &pos);

    void updateCells(const Line *forLine);

    void activateGripAt(const QPointF &pos);
    void updateLine(Line *line);
};
