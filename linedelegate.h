#pragma once

#include <QStyledItemDelegate>

class LineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    LineDelegate(QObject* parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};
