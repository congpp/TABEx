#include "painthandler.h"
#include "../Project/tabproject.h"

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
    case PS_FINISHED:
        m_fps.stop();
        break;
    case PS_PLAYING:
        m_fps.start();
        break;
    }
}

void IPaintHandler::setTabLineSize(QSize szTL)
{
    m_szTabLineImg = szTL;
}

void IPaintHandler::setBackgroundImage(QImagePtr img)
{
    m_imgBg = img;
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

    static QImage cdOri(":/image/resouce/image/cd.png");
    static QImage cd;
    static QImagePtr lastCover;
    QImagePtr imgCover = getCoverImage();
    if (lastCover != imgCover)
    {
        cd = cdOri.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QPainter pcd;
        pcd.begin(&cd);
        pcd.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        QPainterPath pp1, pp2;
        pp1.addEllipse(QPoint(160,160),130,130);
        pp2.addEllipse(QPoint(160,160),40,40);
        pcd.setClipPath(pp1.subtracted(pp2));
        pcd.setOpacity(0.5);
        if (!imgCover.isNull())
        {
            pcd.drawImage(cd.rect(), *imgCover);
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
        pImgCover.reset(new QImage(":/image/resouce/image/cover.png"));
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
        painter->drawImage(rc, *m_imgBg);

    TABProject* pProj = TAB_INST;

    //cover
    QRect rcCover;
    rcCover.setTop(rc.top() + rc.height() / 3 - m_szCoverMax.height() / 2);
    rcCover.setRight(rc.left() + int(rc.width() * m_tabLineXPos));
    rcCover.setLeft(rcCover.right() - m_szCoverMax.width());
    rcCover.setBottom(rcCover.top() + m_szCoverMax.height());
    paintCover(painter, rcCover);

    //info
    QRect rcInfo = rcCover;
    rcInfo.adjust(0, m_szCoverMax.height(), 0, 0);
    paintInfo(painter, rcInfo);

    QBrush brFg(QColor(0,162,232,128));     //black mask
    QBrush brBg(QColor(255,255,255,64));    //white bg

    //封面右边，整块区域，20px padding
    const int padding = 20;
    QRect rcTabLine(rcCover.right() + padding, rc.top() + padding, 0, 0);
    rcTabLine.setRight(rc.right() - padding);
    rcTabLine.setBottom(rc.bottom() - padding);

    //bg
    //painter->fillRect(rcTabLine, brBg);

    //mid
    TabLinePtr m = pProj->getTabLineAt(m_iTabLine);
    QImagePtr img;
    QSize sz;
    QRect rcM = rcTabLine;
    if (m)
    {
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
        if (m_maskPercent > 0 || m_smoothScroll > 0)
        {
            QRect rcMask=rcM;
            if (m_smoothScroll == 0)
                rcMask.setWidth(int(m_maskPercent * rcM.width()));
            //qDebug() << "rcMask" << rcMask;
            painter->fillRect(rcMask, brFg);
        }

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
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    QFont f1("Consolas", 16); f1.setBold(true);
    QFont f2("Consolas", 28); f2.setBold(true);
    QRect rcText;
    QString str;
    int l, t, w, h;
    w = 60;
    h = 40;
    l = rc.center().rx() - w;
    t = rc.top();
    painter->setFont(f1);
    painter->drawText(l, t, w, h, Qt::AlignRight|Qt::AlignVCenter, tr("BPM:"), &rcText);

    str = QString::asprintf("%d", TAB_INST->getBeatPerMinuteAdjusted());
    l = rcText.right() + 20;
    painter->setFont(f2);
    painter->drawText(l, t, w, h, Qt::AlignLeft|Qt::AlignVCenter, str, &rcText);
    painter->restore();
}

QSize QVerticalPaintHandler::getFixedImageSize(QRect &rcPaint, const QRect &rcImg)
{
    int w=rcImg.width();
    int h=rcImg.height();
    int maxWidth = rcPaint.width();

    //图片比窗口要宽，则使用窗口宽度，但是要调整图片高度
    if (w > maxWidth)
    {
        double d = w * 1.0 / maxWidth;   //>1
        w = maxWidth;
        h =  int(rcImg.height() / d);

    }
    //窗口比图片宽，则使用图片宽度，就OK了
    else if (rcImg.width() < maxWidth)
    {
    }

    return QSize(w, h);
}
