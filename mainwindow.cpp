#include "mainwindow.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QTableView(this))
    , m_model(new QStandardItemModel(this))
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
            m_model->setData(id, QString("[%1x%2]").arg(QString::number(row+1),QString::number(column+1g)));
        }
    }

    m_view->setModel(m_model);
    setCentralWidget(m_view);
}

MainWindow::~MainWindow()
{
}

