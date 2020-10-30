#ifndef QIMAGEUTIL_H
#define QIMAGEUTIL_H
#include <QImage>

class QImageBitTool
{
public:
    QImageBitTool();

    ~QImageBitTool();

    //copy pixels to buffer 32bpp
    uchar* lockBits32Bpp(QImage& img);

    //write pixels to img
    bool   unlockBits32Bpp(QImage& img, uchar* pixels, uint pixelsize);

    uchar* getPixels();
    uint   getPixelSize();
protected:
    uchar* m_pixels;
    uint   m_pixelSize;
};


class QImageUtil
{
public:
    QImageUtil();

    static bool getCenterPaintingRect(const QSize& szImg, const QRect& rcPaint, QRect& rcCenter)
    {
        if (szImg.isNull() || rcPaint.isNull())
            return false;


        QRect rcWin = rcPaint;

        //const QRect rcWin(rc.left + m_rcCenterPadding.left, rc.top + m_rcCenterPadding.top,
        //    (REAL)rc.GetWidth() - m_rcCenterPadding.right - m_rcCenterPadding.left,
        //    (REAL)rc.GetHeight() - m_rcCenterPadding.bottom - m_rcCenterPadding.top);

        QRect rcImg(0, 0, szImg.width(), szImg.height());

        //包含, 居中即可
        if (rcWin.width() > rcImg.width() && rcWin.height() > rcImg.height())
        {
            rcImg.moveLeft((rcWin.width() - rcImg.width()) / 2 + rcWin.left());
            rcImg.moveTop((rcWin.height() - rcImg.height()) / 2 + rcWin.top());
        }
        else
        {
            //按高度缩小
            double fH = rcImg.height() * 1.0 / rcWin.height();
            rcImg.setHeight(rcWin.height());
            rcImg.setWidth(int(rcImg.width() / fH));

            //太宽了，再缩小宽度
            if (rcImg.width() > rcWin.width())
            {
                double fW = rcImg.width() * 1.0 / rcWin.width();
                rcImg.setWidth(rcWin.width());
                rcImg.setHeight(int(rcImg.height() / fW));
            }

            rcImg.moveLeft((rcWin.width() - rcImg.width()) / 2 + rcWin.left());
            rcImg.moveTop((rcWin.height() - rcImg.height()) / 2 + rcWin.top());
        }

        rcCenter = rcImg;
        return true;
    }
};

#endif // QIMAGEUTIL_H
