#include "mainwindow.h"

#include "linedelegate.h"
#include "delegatedlinedata.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QTableView(this))
    , m_model(new QStandardItemModel(this))
    , m_delegate(new LineDelegate(this))
{
    static constexpr int rowsCount {100};
    static constexpr int columnsCount {10};

    for(int row = 0; row <rowsCount; ++row)
    {
        m_model->insertRow(row);
        for(int column = 0; column < columnsCount; ++column)
        {
            if(m_model->columnCount() < columnsCount)
                m_model->insertColumn(column);

            const QModelIndex &id = m_model->index(row,column);
            m_model->setData(id, QString("[%1x%2]").arg(QString::number(row+1),QString::number(column+1)));
        }
    }

    {
        const QModelIndex testDelegate = m_model->index(1,1);
        const Line line { 0.2, 0.8 };
        m_model->setData(testDelegate, QVariant::fromValue(line), Line::DataRole);
    }

    {
        const QModelIndex testDelegate = m_model->index(3,3);
        const Line line { 0.1, 0.9 };
        m_model->setData(testDelegate, QVariant::fromValue(line), Line::DataRole);
    }

    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    setCentralWidget(m_view);
}

MainWindow::~MainWindow()
{
}

