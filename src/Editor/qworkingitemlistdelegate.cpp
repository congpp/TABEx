#include "qworkingitemlistdelegate.h"
#include "qworkingareaitem.h"
#include "qappdata.h"
#include "../Project/tabproject.h"
#include <QtWidgets>

QWorkingItemListDelegate::QWorkingItemListDelegate(QWidget *parent): QStyledItemDelegate(parent)
{
}

void QWorkingItemListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) return;

    if (index.column() != 0) return;

    QImagePtr pImg = TAB_INST->getImage(index.row());
    if (!pImg) return;

    painter->save();

    QRect rcPaint = option.rect;
    if (rcPaint.width() > pImg->width())
    {
        rcPaint.adjust((rcPaint.width() - pImg->width())/2,0,0,0);
        rcPaint.setWidth(pImg->width());
    }

    painter->drawImage(rcPaint, *pImg, pImg->rect());

    QColor clrBorder = CLR_FG;
    //if (option.state & QStyle::State_Selected)
    //    painter->fillRect(rcPaint, QBrush(clrBorder));

    QPen p(clrBorder);
    p.setWidth(1);
    painter->setPen(p);
    painter->drawRect(rcPaint);


    painter->restore();
}

QSize QWorkingItemListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())  return QSize();
    QSize result = option.rect.size();

    int h = result.height(), w = result.width();

    QImagePtr pImg = TAB_INST->getImage(index.row());
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
        //h = std::min(500, h);
        //h = std::min(500, pTL->rcPos.height());
    }

    //result.setWidth(w);
    result.setHeight(h);
    return result;
}

bool QWorkingItemListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    qDebug() << event->type();
    if (event->type() == QEvent::MouseButtonDblClick)
    {
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void QWorkingItemListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QWorkingAreaItem* pItem = static_cast<QWorkingAreaItem*>(editor);
    QRectF rc = pItem->getCropRect();
    QVariant var(rc);
    qDebug() << "setModelData QWorkingAreaItem" << rc;
    model->setData(index, var);
}

void QWorkingItemListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);

    //不要在这 setCropRect 了，因为界面拖拽时，需要更新List大小，导致此处会响应，
    //而 cropRect 也需要动态根据界面调整，二者冲突
    //QWorkingAreaItem* pItem = static_cast<QWorkingAreaItem*>(editor);
    //QVariant var = index.model()->data(index);
    //QRectF rc = var.toRectF();
    //qDebug() << "setEditorData QWorkingAreaItem" << rc;
    //if (!rc.isEmpty())
    //    pItem->setCropRect(rc);
}

QWidget *QWorkingItemListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    qDebug() << "createEditor QWorkingAreaItem";
    QWorkingAreaItem* pItem = new QWorkingAreaItem(parent);
    pItem->setImage(TAB_INST->getImageName(index.row()));
    pItem->setDrawAnchor(true);
    connect(pItem, &QWorkingAreaItem::signalCropRectChanged, this, &QWorkingItemListDelegate::slotCropRectChanged);

    QVariant var = index.model()->data(index);
    QRectF rc = var.toRectF();
    qDebug() << "setEditorData QWorkingAreaItem" << rc;
    if (!rc.isEmpty())
        pItem->setCropRect(rc);
    return pItem;
}

bool QWorkingItemListDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(view);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return false;
}

void QWorkingItemListDelegate::slotCropRectChanged()
{
    qDebug() << "createEditor slotCropRectChanged";
    QWorkingAreaItem* pItem = qobject_cast<QWorkingAreaItem*>(sender());
    emit commitData(pItem);
}
