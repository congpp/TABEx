#ifndef QHistoryTableDelegate_H
#define QHistoryTableDelegate_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPainter>

#include <QDebug>
#include <QMouseEvent>


class QHistoryTableDelegate  : public QStyledItemDelegate
{

    Q_OBJECT
public:
    QHistoryTableDelegate (QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    int intSelID = -1;
};


#endif // QHistoryTableDelegate_H
