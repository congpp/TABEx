#include "qhistorytabledelegate.h"
#include "../Project/tabproject.h"
#include "../Project/qtpf.h"
#include "resourceloader.h"
#include <QtWidgets>

const int ROW_PADDING = 4;
const int COL_PADDING = 4;

QHistoryTableDelegate::QHistoryTableDelegate(QWidget *parent): QStyledItemDelegate(parent)
{
}

void QHistoryTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid()) return;


    QVariant var = index.model()->data(index);
    QStringList phi = var.toStringList();
    if (phi.size() != 2)
        return;

    QString fileName = phi.at(0);
    QString timeAccess = phi.at(1);
    QImagePtr img = g_resLoader.findExternalImage(fileName);
    if (!img)
    {
        img = QTPF::loadCoverImage(fileName);
        if (img)
            g_resLoader.registerExternalImage(fileName, img);
        else
            img = g_resLoader.getImage(RID_IMG_COVER);
    }

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);
    QRect rc(option.rect);
    rc.setBottom(rc.bottom() - ROW_PADDING);
    rc.setRight(rc.right() - COL_PADDING);

    //QPainterPath pp;
    //pp.addRoundedRect(rc, 8, 8);
    //painter->setClipPath(pp);
    painter->setClipRect(rc);

    //resources
    static const QBrush brBG(QColor(255,255,255,10));
    static const QBrush brBGHover(QColor(127,200,255,64));
    static const QPen   penHover(QColor(127,200,255,128));
    static const QPen   penTitle(QColor(0xEE,0xEE,0xEE,0xFF)), penInfo(QColor(0xCD,0xCD,0xCD,0xFF));
    static const QFont  fontTitle("微软雅黑", 13), fontInfo("微软雅黑", 10);

    //bg
    painter->fillRect(rc, brBG);

    //border if hot
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(rc, brBGHover);
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        painter->setPen(penHover);
        painter->drawRect(rc);
    }

    int l=rc.left(), t=rc.top(), w=rc.width()/*, h=rc.height()*/;

    QRect rcCover(l+4, t+4, 64, 64);
    if (img)
        painter->drawImage(rcCover, *img, img->rect());

    QRect rcName(l + 84, t + 4, w - 84, 24);
    QRect rcPath(l + 84, t + 28, w - 84, 18);
    QRect rcDate(l + 84, t + 48, w - 84, 18);

    painter->setPen(penTitle);
    painter->setFont(fontTitle);
    QString strName = QFileUtil::getFileName(fileName);
    painter->drawText(rcName, Qt::AlignLeft, strName);

    painter->setPen(penInfo);
    painter->setFont(fontInfo);
    QFontMetrics fm(fontInfo);
    QString strPath = fm.elidedText(fileName, Qt::ElideMiddle, rcPath.width());
    painter->drawText(rcPath, Qt::AlignLeft, strPath);

    timeAccess = timeAccess.isEmpty() ? tr("00-00-00 00:00:00") : timeAccess;
    painter->drawText(rcDate, Qt::AlignLeft, timeAccess);

    painter->restore();
}

QSize QHistoryTableDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSize result;
    result.setWidth(200);
    result.setHeight(140);
    return result;
}

bool QHistoryTableDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(view);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return false;
}
