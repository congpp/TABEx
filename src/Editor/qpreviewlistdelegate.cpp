#include "qpreviewlistdelegate.h"
#include "qtablinewidget.h"
#include "qappdata.h"
#include "../Project/tabproject.h"
#include <QtWidgets>

const int ROW_PADDING = 4;

QPreviewListDelegate::QPreviewListDelegate(QWidget *parent): QStyledItemDelegate(parent)
{
}

void QPreviewListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) return;

    if (index.column() != 0) return;

    QImagePtr pImg = TAB_INST->getImagePreview(index.row());
    if (!pImg) return;

    painter->save();
    //painter->setClipRect(option.rect);

    QRect rcPaint = option.rect;
    if (rcPaint.width() > pImg->width())
    {
        rcPaint.adjust((rcPaint.width() - pImg->width())/2,0,0,0);
        rcPaint.setWidth(pImg->width());
    }

    rcPaint.setHeight(rcPaint.height() - ROW_PADDING);

    painter->drawImage(rcPaint, *pImg, pImg->rect());

    QColor clrBorder(0, 162, 232, 128);
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(rcPaint, QBrush(clrBorder));
    }
    else
    {
        QPen p(clrBorder);
        p.setWidth(1);
        painter->setPen(p);
        painter->drawRect(rcPaint);
    }

    painter->restore();
}

QSize QPreviewListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())  return QSize();
    QSize result = option.rect.size();

    int h = result.height(), w = result.width();

    QImagePtr pImg = TAB_INST->getImagePreview(index.row());
    if (pImg)
    {
        if (pImg->width() > w)
        {
            double d = pImg->width() * 1.0 / w;
            h = int(pImg->height() / d);
        }
        else
        {
            h = pImg->height();
        }
    }

    result.setHeight(h + ROW_PADDING);
    return result;
}

bool QPreviewListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    qDebug() << event->type();
    if (event->type() == QEvent::MouseButtonDblClick)
    {
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool QPreviewListDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(view);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return false;
}
