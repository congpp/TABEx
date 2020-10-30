#ifndef QTABLINEITEMLISTDELEGATE_H
#define QTABLINEITEMLISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPainter>

#include <QDebug>
#include <QMouseEvent>


class QTabLineItemListDelegate  : public QStyledItemDelegate
{

    Q_OBJECT
public:
    QTabLineItemListDelegate (QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    virtual bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    int intSelID = -1;
};


#endif // QTABLINEITEMLISTDELEGATE_H
