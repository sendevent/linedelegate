#pragma once

#include <QMainWindow>

class QTableView;
class QAbstractItemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTableView *m_view {nullptr};
    QAbstractItemModel *m_model {nullptr};
};
