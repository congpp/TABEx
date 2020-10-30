#include "qpreviewwidget.h"
#include "qpreviewitem.h"
#include "qpreviewlistdelegate.h"
#include "../Project/tabproject.h"

QPreviewWidget::QPreviewWidget(QWidget *parent) : QListView(parent)
{
    for (int i=0; i<PMA_COUNT; ++i)
    {
        QAction* pAct = new QAction(PopupMenuActionName[i], this);
        pAct->setData(i);
        connect(pAct, SIGNAL(triggered(bool)), this, SLOT(onActionToggled(bool)));
        m_actions.append(pAct);
    }

    this->setItemDelegate(new QPreviewListDelegate);
    this->setModel(TAB_INST->getPreviewImageModel());
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setAutoScroll(false);
}

void QPreviewWidget::onActionToggled(bool)
{
    QAction* pAct = qobject_cast<QAction*>(sender());
    if (!pAct)
        return;

    PopupMenuActionIndex pi = static_cast<PopupMenuActionIndex>(pAct->data().toInt());
    qDebug() << "onActionToggled " << pi;
    switch (pi)
    {
    case PMA_ADD_IMG:
    {
        QString strImg = QFileDialog::getOpenFileName(this,
            tr("Add image"), QDir::currentPath(), tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));
        TAB_INST->addImage(strImg, &strImg);
        //emit newImageAdded(strImg, -1);
        break;
    }
    case PMA_INSERT_BEFORE:
    {
        QString strImg = QFileDialog::getOpenFileName(this,
            tr("Add image"), QDir::currentPath(), tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));
        TAB_INST->insertImage(strImg, m_lastMenuClickedIndex, &strImg);
        //emit newImageAdded(strImg, m_lastMenuClickedIndex);
        break;
    }
    case PMA_INSERT_AFTER:
    {
        QString strImg = QFileDialog::getOpenFileName(this,
            tr("Add image"), QDir::currentPath(), tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));
        TAB_INST->insertImage(strImg, m_lastMenuClickedIndex + 1, &strImg);
        //emit newImageAdded(strImg, m_lastMenuClickedIndex + 1);
        break;
    }
    case PMA_DELETE:
    {
        QModelIndexList li = selectedIndexes();
        for (auto it : li)
        {
            TAB_INST->removeImage(it.row());
            //emit imageDeleted(pPreviewItem->getImagePath());
        }
        break;
    }
    case PMA_MOVE_UP:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty() || li.size() > 1)
            break;

        int r = li.first().row();
        TAB_INST->moveImage(r, r-1);
        break;
    }
    case PMA_MOVE_DOWN:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty() || li.size() > 1)
            break;

        int r = li.first().row();
        TAB_INST->moveImage(r, r+1);
        break;
    }
    case PMA_COUNT:
        break;
    }
}

//void QPreviewWidget::onItemChecked(bool bChecked)
//{
//    if(!bChecked)
//        return;
//
//    int itemCount = this->layout()->count();
//    for (int i=0; i<itemCount-1; ++i)
//    {
//        QPreviewItem* pItem = static_cast<QPreviewItem*>(this->layout()->itemAt(i)->widget());
//        if (pItem->isChecked())
//            emit itemChecked(pItem->getImagePath());
//    }
//}
//
//void QPreviewWidget::resizeEvent(QResizeEvent *event)
//{
//    QSize sz = event->size();
//    Q_UNUSED(sz);
//    adjustItems(sz);
//    QWidget::resizeEvent(event);
//}


void QPreviewWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    TABProject* proj = TAB_INST;

    int itemCount = proj->getImageCount();
    QMenu* pMenu = new QMenu(this);
    if (itemCount == 0)
    {
        pMenu->addAction(m_actions[PMA_ADD_IMG]);
    }
    else
    {
        int i = indexAt(event->pos()).row();
        m_lastMenuClickedIndex = i;
        qDebug() << "contextMenuEvent item " << i;
        if (-1 == i)
        {
            pMenu->addAction(m_actions[PMA_ADD_IMG]);
        }

        //QPreviewItem* pItem = static_cast<QPreviewItem*>(this->layout()->itemAt(i)->widget());

        else if (i == 0)
        {
            pMenu->addAction(m_actions[PMA_MOVE_DOWN]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
        else if (i == itemCount - 1)
        {
            pMenu->addAction(m_actions[PMA_MOVE_UP]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
        else
        {
            pMenu->addAction(m_actions[PMA_MOVE_UP]);
            pMenu->addAction(m_actions[PMA_MOVE_DOWN]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
    }
    pMenu->exec(cursor().pos());
    delete pMenu;
}

void QPreviewWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    emit itemChecked(current.row());
}

//int QPreviewWidget::findItemByPoint(QPoint pt)
//{
//    int itemCount = this->layout()->count() - 1;
//    int i=0;
//    for (i=0; i<itemCount; ++i)
//    {
//        QRect rcItem = this->layout()->itemAt(i)->widget()->geometry();
//        if (rcItem.contains(pt))
//            return i;
//    }
//
//    return -1;
//}
