#include "qpreviewwidget.h"
#include "qpreviewitem.h"
#include "qpreviewlistdelegate.h"
#include "../Project/tabproject.h"

QPreviewWidget::QPreviewWidget(QWidget *parent) : QListView(parent)
{
    const QString PopupMenuActionName[PMA_COUNT] =
    {
        tr("Add image"),
        tr("Insert image before this"),
        tr("Insert image after this"),
        tr("Remove"),
        tr("Move up"),
        tr("Move down"),
        tr("Export"),
        tr("Relpace"),
    };

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

    static const QString filters(tr("Image Files (*.bmp *.jpg *.jpeg *.png *.gif)"));

    PopupMenuActionIndex pi = static_cast<PopupMenuActionIndex>(pAct->data().toInt());
    qDebug() << "onActionToggled " << pi;
    switch (pi)
    {
    case PMA_ADD_IMG:
    {
        QStringList imgs = QFileDialog::getOpenFileNames(this, tr("Add image"), "", filters);
        for (auto img : imgs)
            TAB_INST->addImage(img, nullptr);
        //emit newImageAdded(strImg, -1);
        break;
    }
    case PMA_INSERT_BEFORE:
    {
        QStringList imgs = QFileDialog::getOpenFileNames(this, tr("Add image"), "", filters);
        for (auto img : imgs)
            TAB_INST->insertImage(img, m_lastMenuClickedIndex, nullptr);
        //emit newImageAdded(strImg, m_lastMenuClickedIndex);
        break;
    }
    case PMA_INSERT_AFTER:
    {
        QStringList imgs = QFileDialog::getOpenFileNames(this, tr("Add image"), "", filters);
        for (auto img : imgs)
            TAB_INST->insertImage(img, m_lastMenuClickedIndex + 1, nullptr);
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
    case PMA_EXPORT:
    {
        QModelIndexList li = selectedIndexes();
        QString saveDir = QFileDialog::getExistingDirectory(this);
        if (saveDir.isEmpty())
            break;

        bool bYesToAll = false;
        bool bNotToAll = false;
        QString songTitle = TAB_INST->getMusicTitle();
        if (songTitle.isEmpty())
            songTitle = QFileUtil::getFileNameNoExt(TAB_INST->currentProject());

        QString firstSelected;
        for (auto it : li)
        {
            QString pathFrom = TAB_INST->getImageTempPath(it.row());
            QString pathTo = saveDir + "/" + songTitle + "_" + QFileUtil::getFileName(pathFrom);
            if (QFile::exists(pathTo))
            {
                if (!bYesToAll && !bNotToAll)
                {
                    int ret = QMessageBox::warning(this, tr("Warning"),
                                                   tr("File already exists: \n[") + pathTo + tr("]\nDo you want to replace it anyway?"),
                                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll,
                                                   QMessageBox::Yes);
                    bYesToAll = ret == QMessageBox::YesToAll;
                    bNotToAll = ret == QMessageBox::NoToAll;
                    if (ret == QMessageBox::No)
                        continue;
                }

                if (bNotToAll)
                    continue;

                QFile::remove(pathTo);
            }

            if (!QFile::copy(pathFrom, pathTo))
            {
                QMessageBox::critical(this, tr("Cannot explort image"),
                                      tr("Cannot export image to path:\r\n[") + pathTo + tr("]\r\nPlease make sure you have enough access right."));
                break;
            }
            if (firstSelected.isEmpty())
                firstSelected = pathTo;
        }

#ifdef _MSC_VER
        if (!firstSelected.isEmpty())
        {
            QProcess proc;
            proc.setProgram("explorer");
            QString arg = " /select,\"" + firstSelected.replace('/', '\\') + "\"";
            proc.setNativeArguments(arg);
            proc.startDetached();
        }
#endif
        break;
    }
    case PMA_REPLACE:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty() || li.size() > 1)
            break;

        QString img = QFileDialog::getOpenFileName(this, tr("Add image"), "", filters);
        int r = li.first().row();
        TAB_INST->replaceImage(r, img);
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
        int selectedCount = selectedIndexes().size();
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
            if (selectedCount == 1)
            {
                pMenu->addAction(m_actions[PMA_MOVE_DOWN]);
                pMenu->addSeparator();
            }
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_EXPORT]);
            if (selectedCount == 1)
                pMenu->addAction(m_actions[PMA_REPLACE]);
        }
        else if (i == itemCount - 1)
        {
            if (selectedCount == 1)
            {
                pMenu->addAction(m_actions[PMA_MOVE_UP]);
                pMenu->addSeparator();
            }
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_EXPORT]);
            if (selectedCount == 1)
                pMenu->addAction(m_actions[PMA_REPLACE]);
        }
        else
        {
            if (selectedCount == 1)
            {
                pMenu->addAction(m_actions[PMA_MOVE_UP]);
                pMenu->addAction(m_actions[PMA_MOVE_DOWN]);
                pMenu->addSeparator();
            }
            pMenu->addAction(m_actions[PMA_INSERT_BEFORE]);
            pMenu->addAction(m_actions[PMA_INSERT_AFTER]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_EXPORT]);
            if (selectedCount == 1)
                pMenu->addAction(m_actions[PMA_REPLACE]);
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
