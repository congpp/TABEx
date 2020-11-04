#include "qtablinewidget.h"
#include "qtablineconfigdialog.h"
#include "qtablineitem.h"
#include "qtablineitemlistdelegate.h"
#include "../Project/tabproject.h"

QTabLineWidget::QTabLineWidget(QWidget *parent) : QListView(parent)
{    
    for (int i=0; i<PMA_COUNT; ++i)
    {
        QAction* pAct = new QAction(PopupMenuActionName[i], this);
        pAct->setData(i);
        connect(pAct, SIGNAL(triggered(bool)), this, SLOT(onActionToggled(bool)));
        m_actions.append(pAct);
    }

    this->setModel(TAB_INST->getTabLineModel());
    this->setItemDelegate(new QTabLineItemListDelegate);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void QTabLineWidget::onContextMenuEvent(int iItem)
{
    TabLineList tl = getTabLineFromClipboard();
    int itemCount = model()->rowCount();
    QMenu* pMenu = new QMenu(this);
    if (itemCount == 0)
    {
        if (!tl.isEmpty())
            pMenu->addAction(m_actions[PMA_PASTE]);
    }
    else
    {
        int i = iItem;
        m_lastMenuClickedIndex = i;
        qDebug() << "contextMenuEvent item " << i;
        if (-1 == i)
        {
            if (!tl.isEmpty())
                pMenu->addAction(m_actions[PMA_PASTE]);
        }
        else if (i == 0)
        {
            pMenu->addAction(m_actions[PMA_MOVE_RIGHT]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_COPY]);
            pMenu->addAction(m_actions[PMA_CUT]);
            if (!tl.isEmpty())
            {
                pMenu->addAction(m_actions[PMA_PASTE_BEFORE]);
                pMenu->addAction(m_actions[PMA_PASTE_AFTER]);
            }
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
        else if (i == itemCount - 1)
        {
            pMenu->addAction(m_actions[PMA_MOVE_LEFT]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_COPY]);
            pMenu->addAction(m_actions[PMA_CUT]);
            if (!tl.isEmpty())
            {
                pMenu->addAction(m_actions[PMA_PASTE_BEFORE]);
                pMenu->addAction(m_actions[PMA_PASTE_AFTER]);
            }
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
        else
        {
            pMenu->addAction(m_actions[PMA_MOVE_LEFT]);
            pMenu->addAction(m_actions[PMA_MOVE_RIGHT]);
            pMenu->addSeparator();
            pMenu->addAction(m_actions[PMA_COPY]);
            pMenu->addAction(m_actions[PMA_CUT]);
            if (!tl.isEmpty())
            {
                pMenu->addAction(m_actions[PMA_PASTE_BEFORE]);
                pMenu->addAction(m_actions[PMA_PASTE_AFTER]);
                pMenu->addSeparator();
            }
            pMenu->addAction(m_actions[PMA_DELETE]);
        }
    }

    if (!pMenu->isEmpty())
        pMenu->exec(cursor().pos());
    delete pMenu;
}

//int QTabLineWidget::findItemByPoint(QPoint pt)
//{
//    QHBoxLayout* pLayout = static_cast<QHBoxLayout*>(this->layout());
//    int itemCount = pLayout->count() - 1;
//    int i=0;
//    for (i=0; i<itemCount; ++i)
//    {
//        QRect rcItem = pLayout->itemAt(i)->widget()->geometry();
//        rcItem.adjust(0,0,pLayout->spacing(),0);
//        if (rcItem.contains(pt))
//            return i;
//    }
//
//    return -1;
//}

TabLineList QTabLineWidget::getTabLineFromClipboard()
{
    TabLineList li;
    QClipboard *cb = QGuiApplication::clipboard();
    const QMimeData* pMime = cb->mimeData();
    if (!pMime)
        return li;

    QByteArray bytes = pMime->data("TabLineData");
    qDebug() << "Paste: " << bytes;
    if (bytes.isEmpty())
        return li;

    QJsonDocument doc = QJsonDocument::fromBinaryData(bytes);
    if (doc.isNull())
        return li;

    QJsonArray arr = doc.array();
    for (auto it : arr)
    {
        if (!it.isObject())
            continue;

        TabLinePtr tl = TAB_INST->json2TabLine(it.toObject());
        if (tl)
            li.push_back(tl);
    }

    qDebug() << li.size() << " tabline item(s) read from clibboard";
    return li;
}

int QTabLineWidget::getSelectedCount()
{
    return selectedIndexes().size();
}

int QTabLineWidget::getFirstSelectedIndex()
{
    QModelIndexList l = selectedIndexes();
    if (l.isEmpty())
        return -1;

    return l.front().row();
}

int QTabLineWidget::getLastSelectedIndex()
{
    QModelIndexList l = selectedIndexes();
    if (l.isEmpty())
        return -1;

    return l.back().row();
}

void QTabLineWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex idx = indexAt(event->pos());
    onContextMenuEvent(idx.row());
}

void QTabLineWidget::onActionToggled(bool)
{
    QAction* pAct = qobject_cast<QAction*>(sender());
    if (!pAct)
        return;

    TABProject* proj = TAB_INST;
    PopupMenuActionIndex pi = static_cast<PopupMenuActionIndex>(pAct->data().toInt());
    qDebug() << "onActionToggled " << pi;
    switch (pi)
    {
    case PMA_MOVE_LEFT:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty() || li.size() > 1)
            break;

        TabLinePtr pTL = proj->getTabLineAt(m_lastMenuClickedIndex);
        proj->moveLeftTabLine(pTL);
        break;
    }
    case PMA_MOVE_RIGHT:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty() || li.size() > 1)
            break;

        TabLinePtr pTL = proj->getTabLineAt(m_lastMenuClickedIndex);
        proj->moveRightTabLine(pTL);
        break;
    }
    case PMA_COPY:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty())
            break;

        std::sort(li.begin(), li.end(), [&](QModelIndex& l, QModelIndex& r)->bool{
            return l.row() < r.row();
        });
        qDebug() << li.size() << " tabline item copied!";
        QClipboard *cb = QGuiApplication::clipboard();
        cb->clear();
        QJsonDocument doc;
        QJsonArray arr;
        for (auto it : li)
        {
            TabLinePtr pTL = proj->getTabLineAt(it.row());
            arr.append(proj->tabLine2Json(pTL));
        }
        doc.setArray(arr);
        QMimeData* pMime = new QMimeData;
        pMime->setData("TabLineData", doc.toBinaryData());
        cb->setMimeData(pMime);
        break;
    }
    case PMA_CUT:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty())
            break;

        std::sort(li.begin(), li.end(), [&](QModelIndex& l, QModelIndex& r)->bool{
            return l.row() < r.row();
        });

        qDebug() << li.size() << " tabline item copied!";
        TabLineList tls;
        QClipboard *cb = QGuiApplication::clipboard();
        cb->clear();
        QJsonDocument doc;
        QJsonArray arr;
        for (auto it : li)
        {
            TabLinePtr tl = proj->getTabLineAt(it.row());
            arr.append(proj->tabLine2Json(tl));
            tls.push_back(tl);
        }
        doc.setArray(arr);
        QMimeData* pMime = new QMimeData;
        pMime->setData("TabLineData", doc.toBinaryData());
        cb->setMimeData(pMime);

        for (auto tl : tls)
            proj->removeTabLine(tl);
        break;
    }
    case PMA_PASTE:
    {
        TabLineList tl = getTabLineFromClipboard();
        proj->addTabLine(tl);
        break;
    }
    case PMA_PASTE_BEFORE:
    {
        TabLineList tl = getTabLineFromClipboard();
        proj->insertTabLine(m_lastMenuClickedIndex, tl);
        break;
    }
    case PMA_PASTE_AFTER:
    {
        TabLineList tl = getTabLineFromClipboard();
        proj->insertTabLine(m_lastMenuClickedIndex+1, tl);
        break;
    }
    case PMA_DELETE:
    {
        QModelIndexList li = selectedIndexes();
        if (li.isEmpty())
            break;

        for (auto it : li)
            proj->removeTabLine(it.row());
        break;
    }
    case PMA_COUNT:
        break;
    }

    this->clearSelection();
    update();
}

void QTabLineWidget::ensureVisible(int index)
{
    if (index == -1)
        index = this->model()->rowCount() - 1;

    if (index < 0 || index >= this->model()->rowCount())
        return;

    scrollTo(this->model()->index(index,0));
}

void QTabLineWidget::selectItem(int index)
{
    if (index == -1)
        index = this->model()->rowCount() - 1;

    if (index < 0 || index >= this->model()->rowCount())
        return;

    QModelIndex idx = this->model()->index(index, 0);
    this->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
}


void QTabLineWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    int pos = this->verticalScrollBar()->sliderPosition();
    if (delta > 0)
    {
        this->verticalScrollBar()->setSliderPosition(pos - 40);
    }
    else
    {
        this->verticalScrollBar()->setSliderPosition(pos + 40);
    }
}
