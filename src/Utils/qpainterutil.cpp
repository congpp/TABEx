#include "qpainterutil.h"

QPainterUtil::QPainterUtil()
{

}

bool QPainterUtil::paintScale9(QPainter* painter, QImage& img, QRect rcPaint, int r)
{
    if (!painter)
        return false;

    if (img.isNull() || rcPaint.isEmpty())
        return true;

    int l = rcPaint.left(), t = rcPaint.top(), w = rcPaint.width(), h = rcPaint.height();
    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    if (r == 0)
    {
        painter->drawImage(rcPaint, img, img.rect());
    }
    else
    {
        int imgW = img.width(), imgH = img.height();
        int wExpand = imgW - r - r, hExpand = imgH - r - r;
        wExpand = std::max(1, wExpand);
        hExpand = std::max(1, hExpand);
        QRect srcLT(0, 0, r, r),
            srcRT(imgW - r, 0, r, r),
            srcLB(0, imgH - r, r, r),
            srcRB(imgW - r, imgH - r, r, r),
            srcL(0, r, r, imgH - r - r),
            srcT(r, 0, wExpand, r),
            srcR(imgW - r, r, r, hExpand),
            srcB(r, imgH - r, wExpand, r),
            srcC(r, r, wExpand, hExpand);
        QRect tarLT(l, t, r, r),
            tarRT(l + w - r, t, r, r),
            tarLB(l, t + h - r, r, r),
            tarRB(l + w - r, t + h - r, r, r),
            tarL(l, t + r, r, h - r - r),
            tarT(l + r, t, w - r - r, r),
            tarR(l + w - r, t + r, r, h - r - r),
            tarB(l + r, t + h - r, w - r - r, r),
            tarC(l + r, t + r, w - r - r, h - r - r);
        painter->drawImage(tarLT, img, srcLT);
        painter->drawImage(tarRT, img, srcRT);
        painter->drawImage(tarLB, img, srcLB);
        painter->drawImage(tarRB, img, srcRB);
        painter->drawImage(tarL, img, srcL);
        painter->drawImage(tarT, img, srcT);
        painter->drawImage(tarR, img, srcR);
        painter->drawImage(tarB, img, srcB);
        painter->drawImage(tarC, img, srcC);
    }
    painter->restore();
    return true;
}
