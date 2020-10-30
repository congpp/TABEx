#include "qtablineitemlistdelegate.h"
#include "qtablineconfigdialog.h"
#include "qtablinewidget.h"
#include "qappdata.h"
#include "../Project/tabproject.h"
#include <QtWidgets>

const int ROW_PADDING = 4;

QTabLineItemListDelegate::QTabLineItemListDelegate(QWidget *parent): QStyledItemDelegate(parent)
{
}

void QTabLineItemListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) return;

    if (index.column() != 0) return;

    TabLinePtr pTL = TAB_INST->getTabLineAt(index.row());
    if (!pTL) return;

    QImagePtr pImg = TAB_INST->getImage(pTL->strImg);
    if (!pImg) return;

    painter->save();
    painter->setClipRect(option.rect);

    QRect rcPaint = option.rect;
    rcPaint.setHeight(rcPaint.height() - ROW_PADDING);
    rcPaint.setWidth(std::min(rcPaint.width(), pTL->rcPos.width()));

    painter->drawImage(rcPaint, *pImg, pTL->rcPos);

    QColor clrBorder = CLR_FG;
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

QSize QTabLineItemListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())  return QSize();
    QSize result = QStyledItemDelegate::sizeHint(option, index);
    if (!result.isValid())
        result = option.rect.size();

    int h = result.height(), w = result.width();

    TabLinePtr pTL = TAB_INST->getTabLineAt(index.row());
    if (pTL)
    {
        if (pTL->rcPos.width() > w)
        {
            double d = pTL->rcPos.width() * 1.0 / w;
            h = int(pTL->rcPos.height() / d);
        }
        else
        {
            h = pTL->rcPos.height();
        }
        h = std::min(500, h);
        //h = std::min(500, pTL->rcPos.height());
    }

    //result.setWidth(w);
    result.setHeight(h + ROW_PADDING);
    return result;
}

bool QTabLineItemListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    qDebug() << event->type();
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        QTabLineConfigDialog dlg(const_cast<QWidget*>(option.widget));
        dlg.setTabLine(TAB_INST->getTabLineAt(index.row()));
        dlg.setModal(true);
        dlg.show();
        dlg.exec();

        //
        TAB_INST->getTabLineModel()->dataChanged(index, index);
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool QTabLineItemListDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(view);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return false;
}
