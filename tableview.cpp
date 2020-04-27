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

#include "tableview.h"

#include "line.h"

#include <QDebug>
#include <QHeaderView>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    m_marker.setWidthF(1.5);
    m_highlighter.setWidthF(2.);

    for (auto headerView : { horizontalHeader(), verticalHeader() })
        connect(headerView, &QHeaderView::sectionResized, this, &TableView::updateGraphicLines);

    for (auto scrollBar : { horizontalScrollBar(), verticalScrollBar() })
        connect(scrollBar, &QScrollBar::valueChanged, this, &TableView::updateGraphicLines);
}

void TableView::paintEvent(QPaintEvent *e)
{
    QTableView::paintEvent(e);

    QPainter painter(this->viewport());
    painter.setPen(m_marker);
    painter.setRenderHint(QPainter::Antialiasing);

    drawLines(&painter, e->rect());
}

void TableView::drawLines(QPainter *painter, const QRectF &rect)
{
    for (auto &line : m_lines)
        if (line->intersects(rect))
            drawLine(painter, line);
}

void TableView::drawLine(QPainter *painter, const Line *line)
{
    const QLineF &lineF = line->graphicLine();
    painter->drawLine(lineF);

    if (line->isSelected()) {
        const QPainterPath stroke = [&lineF] {
            QPainterPath path;
            path.moveTo(lineF.p1());
            path.lineTo(lineF.p2());

            return path;
        }();

        painter->strokePath(stroke, m_highlighter);
    }
}

Line::Point TableView::linePointForPos(const QPointF &pos) const
{
    QModelIndex id = indexAt(pos.toPoint());
    if (m_currLine) {
        const QModelIndex &startId = m_currLine->start().m_index;
        if (startId.isValid() && id.row() != startId.row())
            id = model()->index(startId.row(), id.column());
    }

    const QRectF &viewport = visualRect(id);
    const qreal w(viewport.width());
    const QPointF mappedPos = pos - viewport.topLeft();
    const qreal percent = qreal(mappedPos.x()) / w;

    return { id, percent };
}

Line *TableView::createLine(const QPointF &start)
{
    Line *line { nullptr };
    const Line::Point linePoint = linePointForPos(start);
    if (linePoint.m_index.isValid()) {
        line = new Line(linePoint, linePoint);
        m_lines.append(line);
    }

    return line;
}

void TableView::commitLine(Line *line)
{
    if (!line)
        return;

    if (!line->isComplete()) {
        m_lines.removeAll(line);
        delete line;
    }
}

void TableView::mousePressEvent(QMouseEvent *e)
{
    QTableView::mousePressEvent(e);

    const QPoint &cursor = e->pos();
    m_currLine = createLine(cursor);
    if (m_currLine) {
        const QRectF &itemRect = visualRect(m_currLine->constStart().m_index);
        m_currLine->updateItemViewStart(itemRect);
        m_currLine->updateItemViewEnd(itemRect);
        update();
    }
}

void TableView::mouseMoveEvent(QMouseEvent *e)
{
    QTableView::mouseMoveEvent(e);

    if (m_currLine) {
        m_currLine->end() = linePointForPos(e->pos());
        m_currLine->updateItemViewEnd(visualRect(m_currLine->constEnd().m_index));

        const int currRow = m_currLine->constStart().m_index.row();
        int firstColumn = qMin(m_currLine->constStart().m_index.column(), m_currLine->constEnd().m_index.column());
        int lastColumn = qMax(m_currLine->constStart().m_index.column(), m_currLine->constEnd().m_index.column());

        const QModelIndex first = model()->index(currRow, firstColumn);
        const QModelIndex last = model()->index(currRow, lastColumn);
        model()->dataChanged(first, last);
    }
}

void TableView::mouseReleaseEvent(QMouseEvent *e)
{
    QTableView::mouseReleaseEvent(e);
    commitLine(m_currLine);
    m_currLine = nullptr;
}

void TableView::updateGraphicLines()
{
    for (auto &line : m_lines) {
        line->updateItemViewStart(visualRect(line->constStart().m_index));
        line->updateItemViewEnd(visualRect(line->constEnd().m_index));
    }
}
