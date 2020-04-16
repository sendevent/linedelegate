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

#include "mainwindow.h"

#include "delegatedlinedata.h"
#include "linedelegate.h"

#include <QStandardItemModel>
#include <QTableView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QTableView(this))
    , m_model(new QStandardItemModel(this))
    , m_delegate(new LineDelegate(this))
{
    static constexpr int rowsCount { 100 };
    static constexpr int columnsCount { 10 };

    for (int row = 0; row < rowsCount; ++row) {
        m_model->insertRow(row);
        for (int column = 0; column < columnsCount; ++column) {
            if (m_model->columnCount() < columnsCount)
                m_model->insertColumn(column);

            const QModelIndex &id = m_model->index(row, column);
            m_model->setData(id, QString("[%1x%2]").arg(QString::number(row + 1), QString::number(column + 1)));
        }
    }

    {
        const QModelIndex testDelegate = m_model->index(0, 1);
        const Line line { 0.5, 1.0 };
        m_model->setData(testDelegate, QVariant::fromValue(line), Line::DataRole);
    }

    {
        const QModelIndex testDelegate = m_model->index(0, 2);
        const Line line { 0.0, 0.5 };
        m_model->setData(testDelegate, QVariant::fromValue(line), Line::DataRole);
    }

    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    setCentralWidget(m_view);
}

MainWindow::~MainWindow() {}
