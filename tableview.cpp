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

QPainterPath createBlock(qreal w, qreal h)
{
    QPainterPath path;
    path.addRect(0., 0., w, h);
    return path;
}

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    auto createCircle = [](qreal r) {
        QPainterPath p;
        p.addEllipse(r, r, r, r);
        return p;
    };

    m_gripStart = createCircle(10.);
    m_gripEnd = createCircle(15.);

    m_marker.setWidthF(1.5);
    m_highlighter.setWidthF(2.);
    m_gripPen.setWidthF(2.5);

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
        if (line->intersectsProlonged(rect))
            drawLine(painter, line);
}

void TableView::drawLine(QPainter *painter, const Line *line)
{
    const QPainterPath &path = m_linePaths[line];

    if (line->isSelected()) {
        painter->save();
        painter->setPen(m_highlighter);

        //        painter->strokePath(path, m_highlighter);
        painter->drawLine(line->graphicLine());

        painter->drawPath(m_gripStart);
        painter->drawPath(m_gripEnd);
        if (m_gripActive)
            painter->fillPath(*m_gripActive, m_gripPen.color());

        painter->restore();
    } else {
        painter->fillPath(path, m_marker.color());
        painter->drawPath(path);
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
        m_gripActive = nullptr;
        return;
    }

    const QLineF &l = line->graphicLine();

    if (l.x2() < l.x1()) {
        qSwap(line->start(), line->end());
        m_currLine->updateItemViewStart(visualRect(line->constStart().m_index));
        m_currLine->updateItemViewEnd(visualRect(line->constEnd().m_index));

        if (m_gripActive)
            selectLine(m_gripActive->boundingRect().center());

        updateCells(line);
    }
}

void TableView::updateCells(const Line *forLine)
{
    if (!forLine)
        return;

    const int currRow = forLine->constStart().m_index.row();
    int firstColumn = qMin(forLine->constStart().m_index.column(), forLine->constEnd().m_index.column());
    int lastColumn = qMax(forLine->constStart().m_index.column(), forLine->constEnd().m_index.column());

    // columns extended by one to the left and right to avoid drawningartefacts
    // when a cell is excluded from the line
    const QModelIndex first = model()->index(currRow, firstColumn - 1);
    const QModelIndex last = model()->index(currRow, lastColumn + 1);
    model()->dataChanged(first, last);
    update();
}

void TableView::createNewLine(const QPointF &pos)
{
    m_currLine = createLine(pos);
    if (m_currLine) {
        m_currLine->start().m_percent = 0.25;
        m_currLine->end().m_percent = 0.75;

        const QRectF &itemRect = visualRect(m_currLine->constStart().m_index);
        m_currLine->updateItemViewStart(itemRect);
        m_currLine->updateItemViewEnd(itemRect);

        updateLine(m_currLine);
        updateCells(m_currLine);
    }
}

void TableView::activateGripAt(const QPointF &pos)
{
    m_gripActive = nullptr;

    if (m_currLine)
        for (const auto path : { &m_gripStart, &m_gripEnd })
            if (path->contains(pos)) {
                m_gripActive = path;
                break;
            }
}

bool TableView::selectLine(const QPointF &pos)
{
    static constexpr qreal tolerancePixels { 15. };
    QRectF searchArea(0., 0., tolerancePixels, tolerancePixels);
    searchArea.moveCenter(pos);

    m_currLine = nullptr;
    m_gripActive = nullptr;

    for (auto &line : m_lines) {
        if (line->isSelected()) {
            line->setSelected(false);
            updateCells(line);
        }

        if (line->intersectsStrict(searchArea)) {
            m_currLine = line;
            m_currLine->setSelected(true);
            const QLineF &graphicLine = line->graphicLine();
            m_gripStart.translate(graphicLine.p1() - m_gripStart.boundingRect().center());
            m_gripEnd.translate(graphicLine.p2() - m_gripEnd.boundingRect().center());
            activateGripAt(pos);
            updateCells(m_currLine);
        }
    }
    return m_currLine;
}

void TableView::mousePressEvent(QMouseEvent *e)
{
    const QPointF cursor = e->pos();

    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier) {
        createNewLine(cursor);
        selectLine(cursor);
        return;
    }

    if (m_currLine && m_currLine->isSelected()) {
        activateGripAt(e->pos());
        return;
    }

    if (selectLine(cursor))
        return;

    QTableView::mousePressEvent(e);
}

void TableView::mouseMoveEvent(QMouseEvent *e)
{

    if (m_currLine) {
        if (m_gripActive == &m_gripStart)
            m_currLine->start() = linePointForPos(e->pos());
        else if (m_gripActive == &m_gripEnd)
            m_currLine->end() = linePointForPos(e->pos());

        updateLine(m_currLine);
        updateCells(m_currLine);
        return;
    }

    QTableView::mouseMoveEvent(e);
}

void TableView::mouseReleaseEvent(QMouseEvent *e)
{
    commitLine(m_currLine);
    m_currLine = nullptr;

    QTableView::mouseReleaseEvent(e);
}

void TableView::updateGraphicLines()
{
    for (auto &line : m_lines)
        updateLine(line);
}

void TableView::updateLine(Line *line)
{
    if (!line)
        return;

    line->updateItemViewStart(visualRect(line->constStart().m_index));
    line->updateItemViewEnd(visualRect(line->constEnd().m_index));

    const QLineF &graphicLine = line->graphicLine();

    QPainterPath linePath;

    QPainterPath verticalEdge;
    verticalEdge.addPath(createBlock(2.5, 20));

    verticalEdge.translate(graphicLine.p1() - verticalEdge.boundingRect().center());
    linePath.addPath(verticalEdge);

    verticalEdge.translate(graphicLine.p2() - verticalEdge.boundingRect().center());
    linePath.addPath(verticalEdge);

    linePath.moveTo(graphicLine.p1());
    linePath.lineTo(graphicLine.p2());

    m_linePaths[line] = linePath;

    if (line->isSelected()) {
        m_gripStart.translate(graphicLine.p1() - m_gripStart.boundingRect().center());
        m_gripEnd.translate(graphicLine.p2() - m_gripEnd.boundingRect().center());
    }
}
