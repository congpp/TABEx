#ifndef QPREVIEWLISTDELEGATE_H
#define QPREVIEWLISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>
#include <QPainter>

#include <QDebug>
#include <QMouseEvent>


class QPreviewListDelegate  : public QStyledItemDelegate
{

    Q_OBJECT
public:
    QPreviewListDelegate (QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    virtual bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    int intSelID = -1;
};


#endif // QPREVIEWLISTDELEGATE_H
