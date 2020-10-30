#include "qworkingareawidget.h"
#include "qworkingitemlistdelegate.h"
#include "../Project/tabproject.h"

QWorkingAreaWidget::QWorkingAreaWidget(QWidget *parent) : QListView(parent)
{
    this->setItemDelegate(new QWorkingItemListDelegate);
    this->setModel(TAB_INST->getImageModel());
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setAutoScroll(false);
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
}

void QWorkingAreaWidget::onItemChecked(bool bChecked)
{
    if(!bChecked)
        return;

    //int itemCount = this->layout()->count();
    //for (int i=0; i<itemCount-1; ++i)
    //{
    //    QWorkingAreaItem* pItem = static_cast<QWorkingAreaItem*>(this->layout()->itemAt(i)->widget());
    //    if (pItem->isChecked())
    //    {
    //        pItem->setDrawAnchor(true);
    //        emit itemChecked(pItem->getImagePath());
    //    }
    //    else if (itemCount > 1)
    //        pItem->setDrawAnchor(false);
    //}
}

void QWorkingAreaWidget::ensureVisible(int index)
{
    if (index == -1)
        index = this->model()->rowCount() - 1;

    if (index < 0 || index >= this->model()->rowCount())
        return;

    scrollTo(this->model()->index(index,0));
}

void QWorkingAreaWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    int pos = this->verticalScrollBar()->sliderPosition();
    if (delta > 0)
    {
        this->verticalScrollBar()->setSliderPosition(pos - 60);
    }
    else
    {
        this->verticalScrollBar()->setSliderPosition(pos + 60);
    }
}
