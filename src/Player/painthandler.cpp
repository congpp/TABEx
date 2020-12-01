#include "painthandler.h"
#include "../Project/tabproject.h"
#include "resourceloader.h"

///////////////////////////////////////////////////////////////////////////
/// \brief IPaintHandler::IPaintHandler
///
IPaintHandler::IPaintHandler()
    : m_szCoverMax(320, 320)
{

}

PaintHandlerType IPaintHandler::type()
{
    return PHT_UNKNOWN;
}

void IPaintHandler::init()
{
    m_imgBg.reset();
    m_imgCover = TAB_INST->getCoverImage();
    m_imgSinger = TAB_INST->getSingerImage();
    m_fps.stop();
    m_iTabLine = -1;
    m_maskPercent = 0;
    m_lastFps = 0;
    m_coverAngle = 0;
    m_coverAngle = 0;
}

void IPaintHandler::setPlayStatus(PlayStatus *ps)
{
    if (!ps)
        return;

    m_ps = ps;
    switch (*m_ps) {
    case PS_IDLE:
    case PS_PAUSED:
        m_fps.stop();
        break;
    case PS_FINISHED:
        m_maskPercent=0;
        m_fps.stop();
        break;
    case PS_PLAYING:
        m_fps.start();
        break;
    }
}

void IPaintHandler::setFixedTabLineSize(QSize szTL)
{
    m_szFixedTabItemUser = szTL;
    m_rcLastPaint=QRect();
}

void IPaintHandler::setBackgroundImage(QImagePtr img, QColor clr)
{
    m_imgBg = img;

    //背景色不能太亮
    qreal h, s, l;
    clr.getHslF(&h, &s, &l);
    if (l > 0.3)
        l = 0.3;
    clr.setHslF(h, s, l);
    m_clrBg = clr;
}

void IPaintHandler::setTabLineIndex(int idx)
{
    m_iTargetTabLine = idx;
    if (m_iTabLine < 0)
        m_iTabLine = idx;

    //平滑滚动
    if (m_iTargetTabLine - m_iTabLine == 1 && *m_ps == PS_PLAYING)
        m_smoothScroll = 1;
    else
        m_iTabLine = idx;

    //重置进度
    m_maskPercent = 0;
}

void IPaintHandler::setTabLineMaskPercent(double percent)
{
    m_maskPercent = percent;
}

float IPaintHandler::updateFPS()
{
    if (!m_fps.isStart())
    {
        m_fps.start();
        m_lastFps = 0;
    }
    else
    {
        m_fps.incFrame();
        float fps = m_fps.getCurrentFPS();
        if (fps > 0)
        {
            qDebug() << "FPS: " << fps;
            m_lastFps = fps;
        }
    }
    return m_lastFps;
}

void IPaintHandler::paintCover(QPainter *painter, QRect rc)
{
    if (m_ps && *m_ps == PS_PLAYING)
    {
        m_coverAngle++;
        m_coverAngle%=360;
    }

    static QImage cd;
    static QImagePtr lastCover;
    QImagePtr imgCover = getCoverImage();
    if (lastCover != imgCover)
    {
        cd = g_resLoader.getImage(RID_IMG_CD)->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QPainter pcd;
        pcd.begin(&cd);
        pcd.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        QPainterPath pp1, pp2;
        pp1.addEllipse(QPointF(160.5,160.5),86,86);
        //pp2.addEllipse(QPoint(160,160),40,40);
        pcd.setClipPath(pp1);
        //pcd.setOpacity(0.8);
        if (!imgCover.isNull())
        {
            pcd.drawImage(pp1.boundingRect(), *imgCover);
        }
        pcd.end();
    }

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    painter->translate(rc.center());
    painter->rotate(m_coverAngle);
    painter->translate(-rc.center());
    painter->drawImage(rc, cd);
    painter->resetTransform();
    painter->restore();
}

void IPaintHandler::paintInfo(QPainter *painter, QRect rc)
{
    Q_UNUSED(painter);
    Q_UNUSED(rc);
}

QImagePtr IPaintHandler::getCoverImage()
{
    static QImagePtr pImgCover;
    QImagePtr pProjCover = TAB_INST->getCoverImage();
    if (pProjCover)
        return pProjCover;

    if (!pImgCover)
        pImgCover = g_resLoader.getImage(RID_IMG_COVER);
    return pImgCover;
}


///////////////////////////////////////////////////////////////////////////
/// \brief QHorizontalPaintHandler::QHorizontalPaintHandler
/// 效果看起来并不是那么好，可视区域太少，放弃了
QHorizontalPaintHandler::QHorizontalPaintHandler()
{

}

PaintHandlerType QHorizontalPaintHandler::type()
{
    return PHT_HORIZONTAL;
}

void QHorizontalPaintHandler::onPaint(QPainter *painter, QRect rc)
{
    QRect rcTabLine(rc);
    rcTabLine.setTop(rc.top() + m_szCoverMax.height() + 20);
    int maxHeight = m_szTabLineImg.height();
    if (maxHeight > rc.bottom() - rcTabLine.top() - 58)
        maxHeight = rc.bottom() - rcTabLine.top() - 58;
    rcTabLine.setBottom(rcTabLine.top() + maxHeight);

    if (!m_imgBg.isNull())
        painter->drawImage(rc, *m_imgBg);

    TABProject* pProj = TAB_INST;

    //cover
    //QRect rcCover(rc.left() + rc.width()/2 - m_szCoverMax.width()/2, rcTabLine.top() - m_szCoverMax.height() - 20,
    //                m_szCoverMax.width(), m_szCoverMax.height());
    int coverHeight = m_szCoverMax.height();
    QRect rcCover(rc.left() + (rc.width()>>1) - (coverHeight>>1),
                  rc.top()  + 20,
                  coverHeight,
                  coverHeight);
    paintCover(painter, rcCover);

    QBrush brFg(QColor(0,162,232,128));     //blue mask
    QBrush brBg(QColor(255,255,255,64));    //white bg

    //bg
    painter->fillRect(rcTabLine, brBg);

    //mid
    TabLinePtr m = pProj->getTabLineAt(m_iTabLine);
    QImagePtr img = pProj->getImage(m->strImg);
    QRect rcM = rcTabLine;
    rcM.setLeft(rcTabLine.left() + rcTabLine.width() / 2 - m->rcPos.width() / 2);
    rcM.setWidth(m->rcPos.width());
    //不够高，居中
    if (m->rcPos.height() < maxHeight)
    {
        rcM.adjust(0, (maxHeight - m->rcPos.height())/2, 0, 0);
        rcM.setHeight(m->rcPos.height());
    }
    //太高了，要缩小rcM
    else if (m->rcPos.height() > maxHeight)
    {
        double d = m->rcPos.height()*1.0/maxHeight; //>1
        double wdiff = (m->rcPos.width() - m->rcPos.width()/d)/2;  //
        rcM.setLeft(int(rcM.left() + wdiff));
        rcM.setRight(int(rcM.right() - wdiff));
    }
    painter->drawImage(rcM, *img, m->rcPos);

    //mask
    if (m_maskPercent > 0)
    {
        QRect rcMask=rcM;
        rcMask.setWidth(int(m_maskPercent * rcM.width()));
        //qDebug() << "rcMask" << rcMask;
        painter->fillRect(rcMask, brFg);
    }

    //left
    QRect rcL=rcTabLine;
    rcL.setRight(rcM.left());
    for (int i=m_iTabLine - 1; i>=0 && rcL.right() > 0; i--)
    {
        TabLinePtr l = pProj->getTabLineAt(i);
        img = pProj->getImage(l->strImg);
        rcL.setLeft(rcL.right() - l->rcPos.width());
        if (l->rcPos.height() < maxHeight)
        {
            rcL.adjust(0, (maxHeight - l->rcPos.height())/2, 0, 0);
            rcL.setHeight(l->rcPos.height());
        }
        //太高了，要缩小rcM
        else if (l->rcPos.height() > maxHeight)
        {
            double d = l->rcPos.height()*1.0/maxHeight; //>1
            double wdiff = (l->rcPos.width() - l->rcPos.width()/d);  //
            rcL.setLeft(int(rcL.left() + wdiff));
        }
        painter->drawImage(rcL, *img, l->rcPos);
        painter->fillRect(rcL, brFg);
        rcL.setRight(rcL.left());
    }

    //right
    QRect rcR=rcTabLine;
    rcR.setLeft(rcM.right());
    int tc = pProj->getTabLineCount();
    for (int i=m_iTabLine + 1; i<tc && rcR.left() < rcTabLine.right(); i++)
    {
        TabLinePtr r = pProj->getTabLineAt(i);
        img = pProj->getImage(r->strImg);
        rcR.setRight(rcR.left() + r->rcPos.width());
        if (r->rcPos.height() < maxHeight)
        {
            rcR.adjust(0, (maxHeight - r->rcPos.height())/2, 0, 0);
            rcR.setHeight(r->rcPos.height());
        }
        //太高了，要缩小rcM
        else if (r->rcPos.height() > maxHeight)
        {
            double d = r->rcPos.height()*1.0/maxHeight; //>1
            double wdiff = (r->rcPos.width() - r->rcPos.width()/d);  //
            rcR.setRight(int(rcR.right() - wdiff));
        }
        painter->drawImage(rcR, *img, r->rcPos);
        rcR.setLeft(rcR.right());
    }

    //fps
    if (m_ps && *m_ps == PS_PLAYING)
    {
        QString fps = QString::asprintf("fps %0.2f", double(updateFPS()));
        //painter->setFont(f);
        painter->drawText(QPoint(20,20), fps);
    }
}

void QHorizontalPaintHandler::init()
{
    IPaintHandler::init();
    m_szTabLineImg = TAB_INST->getTabLineSizeH();
}

void QHorizontalPaintHandler::calculateFixedItemRect(QRect rc)
{
    Q_UNUSED(rc);
    if (rc == m_rcLastPaint)
        return;

    QSize sz = TAB_INST->getTabLineSizeH(0);
    m_szFixedTabItem.setHeight(sz.height());
}

///////////////////////////////////////////////////////////////////////////
/// \brief QVerticalPaintHandler::QVerticalPaintHandler
///
QVerticalPaintHandler::QVerticalPaintHandler()
{

}

PaintHandlerType QVerticalPaintHandler::type()
{
    return PHT_VERTICAL;
}

void QVerticalPaintHandler::drawRoundImage(QPainter* painter, QRect rcPaint, QImagePtr img, QRect rcImg)
{
    QPainterPath pp;
    pp.addRoundedRect(rcPaint, 8, 8);
    painter->save();
    painter->setClipPath(pp);
    painter->drawImage(rcPaint, *img, rcImg);
    painter->restore();
}

void QVerticalPaintHandler::onPaint(QPainter *painter, QRect rc)
{
    if (!m_imgBg.isNull())
    {
        painter->fillRect(rc, m_clrBg);
        //painter->drawImage(rc, *m_imgBg);
        QRect rcBg;
        QImageUtil::getCenterPaintingRect(m_imgBg->size(), rc, rcBg, true);
        rcBg.moveTop(rc.top());
        painter->drawImage(rcBg, *m_imgBg);
    }

    calculateFixedItemRect(rc);

    TABProject* pProj = TAB_INST;

    //cover
    QRect rcCover(m_rcCover);
    //rcCover.setTop(rc.top() + rc.height() / 3 - m_szCoverMax.height() / 2);
    //rcCover.setRight(rc.left() + int(rc.width() * m_tabLineXPos));
    //rcCover.setLeft(rcCover.right() - m_szCoverMax.width());
    //rcCover.setBottom(rcCover.top() + m_szCoverMax.height());
    paintCover(painter, rcCover);

    //info
    QRect rcInfo(rcCover);
    rcInfo.adjust(0, m_szCoverMax.height(), 0, 0);
    paintInfo(painter, rcInfo);

    //QBrush brFg(QColor(0,162,232,128));     //black mask
    //QBrush brBg(QColor(255,255,255,64));    //white bg

    //封面右边，整块区域，20px padding
    const int padding = 20;
    QRect rcTabLine=m_rcTabLine;//(rcCover.right() + padding, rc.top() + padding, 0, 0);
    //rcTabLine.setRight(rc.right() - padding);
    //rcTabLine.setBottom(rc.bottom() - padding);

    //bg
    //painter->fillRect(rcTabLine, brBg);

    //mid
    TabLinePtr m = pProj->getTabLineAt(m_iTabLine);
    QImagePtr img;
    QSize sz;
    QRect rcM = rcTabLine;
    if (m)
    {
        if (m_ps && *m_ps == PS_FINISHED)
            painter->setOpacity(0.3);

        sz = getFixedImageSize(rcM, m->rcPos);
        rcM.setTop(rcCover.center().ry() - sz.height() / 2);
        //rcM.setTop(rcTabLine.center().ry() - sz.height() / 2);
        rcM.setWidth(sz.width());
        rcM.setHeight(sz.height());
        //可以通过控制rcM的top来产生滚动动画
        if (m_smoothScroll > 0 && m_smoothScroll < m_smoothScrollMax)
        {
            int dist = int(sz.height()*(m_smoothScroll*1.0/m_smoothScrollMax));
            rcM.adjust(0,-dist,0,-dist);
            m_smoothScroll++;
            qDebug() << "Smoothing: " << m_smoothScroll << rcM;
        }
        img = pProj->getImage(m->strImg);
        drawRoundImage(painter, rcM, img, m->rcPos);
        //painter->drawPath(pp);

        //mask
        QRect rcMask=rcM;
        rcMask.setTop(rcM.bottom());
        rcMask.setBottom(rcMask.top() + padding);
        paintProgress(painter, rcMask);

        rcM.setBottom(rcM.bottom() + m_numImgHeight);
    }

    painter->setOpacity(0.3);
    //top
    QRect rcL=rcTabLine;
    rcL.setBottom(rcM.top()-padding);
    for (int i=m_iTabLine - 1; i>=0 && rcL.bottom() > 0; i--)
    {
        TabLinePtr l = pProj->getTabLineAt(i);
        img = pProj->getImage(l->strImg);
        sz = getFixedImageSize(rcL, l->rcPos);
        rcL.setTop(rcL.bottom() - sz.height());
        rcL.setWidth(sz.width());

        drawRoundImage(painter, rcL, img, l->rcPos);
        //painter->fillRect(rcL, brFg);
        rcL.setBottom(rcL.top()-padding);
        rcL.setWidth(rcTabLine.width());
    }

    //bottom
    QRect rcR=rcTabLine;
    rcR.setTop(rcM.bottom());
    int tc = pProj->getTabLineCount();
    for (int i=m_iTabLine + 1; i<tc && rcR.top() < rcTabLine.bottom(); i++)
    {
        TabLinePtr r = pProj->getTabLineAt(i);
        img = pProj->getImage(r->strImg);
        sz = getFixedImageSize(rcR, r->rcPos);
        rcR.setBottom(rcR.top() + sz.height());
        rcR.adjust(0,padding,0,0);
        rcR.setWidth(sz.width());

        drawRoundImage(painter, rcR, img, r->rcPos);
        rcR.setTop(rcR.bottom());
        rcR.setWidth(rcTabLine.width());
    }

    painter->setOpacity(1.0);
    //fps
    if (m_ps && *m_ps == PS_PLAYING)
    {
        QString fps = QString::asprintf("fps %0.2f", double(updateFPS()));
        //painter->setFont(f);
        painter->drawText(QPoint(20,20), fps);
    }

    //
    if (m_smoothScroll >= m_smoothScrollMax)
    {
        m_iTabLine = m_iTargetTabLine;
        m_smoothScroll = 0;
    }
}

void QVerticalPaintHandler::init()
{
    IPaintHandler::init();
    m_szTabLineImg = TAB_INST->getTabLineSizeV();
}

void QVerticalPaintHandler::paintInfo(QPainter *painter, QRect rc)
{
    TABProject* proj = TAB_INST;
    TabLinePtr tl = proj->getTabLineAt(m_iTabLine);
    if (tl.isNull())
        return;

    double percent = m_maskPercent;
    if (m_ps && *m_ps == PS_FINISHED)
        percent = 1.0;

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);

    static const QFont f1("", 16, QFont::Bold);
    static const QFont f2("", 20, QFont::Bold);
    static const QFont f3("", 24, QFont::Bold);
    static const QPen pen(QColor(0xFF,0xFF,0xFF));
    static const int textPadding = 5;
    //先显示歌曲名字
    QRect rcName=rc;
    rcName.setHeight(34);
    painter->setFont(f3);
    painter->setPen(pen);
    painter->drawText(rcName, Qt::AlignCenter|Qt::AlignVCenter, proj->getMusicTitle());

    rc.setTop(rcName.bottom() + textPadding);
    //cover下面区域切成左右两边
    //左边标题，底部+右对齐
    //右边值，底部+左对齐
    const int infoHeight = 28;
    QRect rcL(rc);
    rcL.setWidth(rc.width()/2 - textPadding);
    rcL.setHeight(infoHeight);
    QRect rcR(rc);
    rcR.setLeft(rc.left()+rc.width()/2 + textPadding);
    rcR.setHeight(infoHeight);

    QString str;

    struct InfoMap
    {
        QString key;
        QString val;
    } ims[]=
    {
        {tr("Song Time:"), QStringUtil::double2MMSS(proj->getSecondOfThisSong())},
        {tr("Speed:"), QString::asprintf("%d", proj->getBeatPerMinuteAdjusted())},
        {tr("Sections:"), QString::asprintf("%d.%d", tl->sections, tl->additionalBeat)},
        {tr("Time:"), QString::asprintf("%.2f", proj->getSecondAtTabLine(m_iTabLine)*(1-percent))},
    };

    for (auto im : ims)
    {
        painter->setPen(pen);
        painter->setFont(f1);
        painter->drawText(rcL, Qt::AlignRight|Qt::AlignVCenter, im.key);
        //painter->drawRect(rcL);

        painter->setFont(f2);
        painter->drawText(rcR, Qt::AlignLeft|Qt::AlignVCenter, im.val);
        //painter->drawRect(rcR);

        rcL.moveTop(rcL.bottom());
        rcR.moveTop(rcR.bottom());
    }

    painter->restore();
}

void QVerticalPaintHandler::paintProgress(QPainter *painter, QRect rc)
{
    if (m_maskPercent < 0.01)
        return;

    double percent = m_maskPercent;
    if (m_ps && *m_ps == PS_FINISHED)
        percent = 1.0;

    TABProject* proj = TAB_INST;
    TabLinePtr tl = proj->getTabLineAt(m_iTabLine);
    if (tl.isNull())
        return;

    painter->save();

    ////一个section只接受1 2 4 8个指示图片
    QImagePtr img = g_resLoader.getImage(RID_IMG_TRIANGLE);
    QRect rcImg(rc.left(), rc.top(), img->width(), img->height());
    int left = int(rc.left() + (rc.width() - img->width()) * percent);
    rcImg.moveLeft(left);
    painter->drawImage(rcImg, *img, img->rect());

    painter->restore();
}

void QVerticalPaintHandler::calculateFixedItemRect(QRect rc)
{
    if (m_rcLastPaint.isValid() && rc == m_rcLastPaint)
        return;

    static const int lMargin=40, rMargin=40, tMargin=0, bMargin=0, itemPadding = 20;
    int l=rc.left() + lMargin, t=rc.top()+tMargin, w=rc.width() - lMargin - rMargin, h=rc.height()-tMargin-bMargin;
    QSize sz = TAB_INST->getTabLineFixedSizeV();
    if (m_szFixedTabItemUser.height() > 0)
    {
        int newWidth = int(sz.width() * (m_szFixedTabItemUser.height() * 1.0 / sz.height()));
        sz.setWidth(newWidth);
        sz.setHeight(m_szFixedTabItemUser.height());
    }

    int itemWidth = sz.width(), itemHeight = sz.height();
    int totalWidth = itemWidth + m_szCoverMax.width() + itemPadding;
    //封面+tabline最大宽度 比 窗口宽度小，则居中
    if (totalWidth < w)
    {
        int l1 = l + (w-totalWidth)/2;
        m_rcCover.setLeft(l1);
        m_rcCover.setTop(t + h / 3 - m_szCoverMax.height() / 2);
        m_rcCover.setRight(l1 + m_szCoverMax.width());
        m_rcCover.setBottom(m_rcCover.top() + m_szCoverMax.height());

        m_szFixedTabItem.setWidth(itemWidth);
        m_szFixedTabItem.setHeight(itemHeight);

        m_rcTabLine.setLeft(m_rcCover.right() + itemPadding);
        m_rcTabLine.setRight(m_rcTabLine.left() + itemWidth);
        m_rcTabLine.setTop(t);
        m_rcTabLine.setBottom(t+h);
    }
    else
    {
        m_rcCover.setLeft(l);
        m_rcCover.setTop(t + h / 3 - m_szCoverMax.height() / 2);
        m_rcCover.setRight(l + m_szCoverMax.width());
        m_rcCover.setBottom(m_rcCover.top() + m_szCoverMax.height());

        int w1 = w - m_szCoverMax.width() - itemPadding;
        int h1 = int(itemHeight/(itemWidth*1.0/w1));
        m_szFixedTabItem.setWidth(w1);
        m_szFixedTabItem.setHeight(h1);

        m_rcTabLine.setLeft(m_rcCover.right() + itemPadding);
        m_rcTabLine.setRight(m_rcTabLine.left() + w1);
        m_rcTabLine.setTop(t);
        m_rcTabLine.setBottom(t+h);
    }
}

QSize QVerticalPaintHandler::getFixedImageSize(QRect &rcPaint, const QRect &rcImg)
{
    int w=rcImg.width();
    int h=rcImg.height();
    int maxWidth = rcPaint.width();
    int maxHeight = m_szFixedTabItemUser.height();
    if (maxHeight <= 0)
        maxHeight = m_szFixedTabItem.height();

    //先等比缩放，对齐高度。垂直播放，高度优先
    if (h > maxHeight)
    {
        double dh = h * 1.0 / maxHeight;
        h = maxHeight;
        w = int(w / dh);
    }

    //图片比窗口要宽，则使用窗口宽度，但是要调整图片高度
    if (w > maxWidth)
    {
        double d = w * 1.0 / maxWidth;   //>1
        w = maxWidth;
        h =  int(h / d);

    }
    //窗口比图片宽，则使用图片宽度，就OK了
    else if (w < maxWidth)
    {
    }

    return QSize(w, h);
}
