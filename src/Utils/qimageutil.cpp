#include "qimageutil.h"
#include <queue>
/////////////////////////////////////////////////////////////////////////////////
QImageBitTool::QImageBitTool()
    : m_pixels(nullptr)
    , m_pixelSize(0)
{
}

QImageBitTool::~QImageBitTool()
{
    if (m_pixels)
    {
        delete[] m_pixels;
        m_pixels = nullptr;
    }
}

uchar *QImageBitTool::lockBits32Bpp(QImage &img)
{
    if (img.isNull())
        return nullptr;

    int w=img.width();
    int h=img.height();
    int bytecount = w * h * 4;
    m_pixels = new uchar[size_t(bytecount)];

    uint* pPixel = reinterpret_cast<uint*>(m_pixels);
    for (int y=0; y<h; ++y)
    {
        for (int x=0; x<w; ++x)
        {
            QRgb px = img.pixel(x, y);
            *pPixel++ = px;
        }
    }

    m_pixelSize = uint(bytecount);
    return m_pixels;
}

bool QImageBitTool::unlockBits32Bpp(QImage &img, uchar *pixels, uint pixelsize)
{
    int w=img.width();
    int h=img.height();
    uint bytecount = uint(w * h * 4);
    if (bytecount != pixelsize)
        return false;

    uint* pPixel = reinterpret_cast<uint*>(pixels);
    for (int y=0; y<h; ++y)
    {
        for (int x=0; x<w; ++x)
        {
            QRgb px = *pPixel++;
            img.setPixel(x, y, px);
        }
    }

    return true;
}

uchar *QImageBitTool::getPixels()
{
    return m_pixels;
}

uint QImageBitTool::getPixelSize()
{
    return m_pixelSize;
}


/////////////////////////////////////////////////////////////////////////////////
QImageUtil::QImageUtil()
{

}


bool QImageUtil::getCenterPaintingRect(const QSize &szImg, const QRect &rcPaint, QRect &rcCenter, bool bZoomIn)
{
    if (szImg.isNull() || rcPaint.isNull())
        return false;


    QRect rcWin = rcPaint;
    int wWin = rcWin.width(), hWin = rcWin.height();
    int wImg = szImg.width(), hImg = szImg.height();
    int l=0, t=0, w=szImg.width(), h=szImg.height();
    QRect rcImg(0, 0, wImg, hImg);

    //包含, 居中即可
    if (wWin > wImg && hWin > hImg)
    {
        if (!bZoomIn)
        {
            l=(wWin - wImg) / 2 + rcWin.left();
            t=(hWin - hImg) / 2 + rcWin.top();
        }
        else
        {
            l=0;
            t=0;
            h=int(hImg*(wWin/w));
            w=wWin;
        }
    }
    else
    {
        //按高度缩小
        double fH = hImg * 1.0 / hWin;
        h = hWin;
        w = int(wImg / fH);

        //太宽了，再缩小宽度
        if (wImg > wWin)
        {
            double fW = wImg * 1.0 / wWin;
            w = wWin;
            h = int(hImg / fW);
        }

        l = (wWin - wImg) / 2 + rcWin.left();
        t = (hWin - hImg) / 2 + rcWin.top();
    }

    rcCenter = QRect(l, t, w, h);
    return true;
}

QColor QImageUtil::getMajorColorOfImage(QImage &img)
{
    QImageBitTool bit;
    quint32* pData = reinterpret_cast<quint32*>(bit.lockBits32Bpp(img));
    if (!pData)
        return QColor();

    std::map<quint32, quint32> m;
    int w=img.width();
    int h=img.height();
    for(int y=0; y<h; ++y)
    {
        for(int x=0; x<w; ++x)
        {
            m[*pData]++;
            pData++;
        }
    }

    qint64  maxVal=-1;
    quint32 val=0;
    for (auto it : m)
    {
        if (it.second > maxVal)
    {
            maxVal = it.second;
            val = it.first;
}
    }
    return QColor((val>>16)&0xFF, (val>>8)&0xFF, val&0xFF, (val>>24)&0xFF);
}

QImage *QImageUtil::getEdgeTransparentImage(QImage &img, QPoint ptCenter)
{
    QImageBitTool bit;
    uchar* pBits = bit.lockBits32Bpp(img);
    quint32* pData = reinterpret_cast<quint32*>(pBits);
    if (!pData)
        return nullptr;

    int w=img.width();
    int h=img.height();
    if (w < 3 || h < 3)
        return nullptr;

    //默认是图片中心
    double xCenter = w/2, yCenter=h/2;
    double r = std::min(w, h)/2;
    //指定了圆心，则使用
    if (!ptCenter.isNull())
    {
        xCenter = ptCenter.rx();
        yCenter = ptCenter.ry();
        r=std::max(w,h);
        double r4[4]={xCenter, yCenter, w-xCenter, h-yCenter};
        for (int i=0; i<4; ++i)
        {
            if (r4[i] == 0.0) continue;
            r=std::min(r, r4[i]);
        }
    }

    for(int y=0; y<h; ++y)
    {
        double b = (y-yCenter);
        b*=b;
        for(int x=0; x<w; ++x)
        {
            double a=(x-xCenter);
            double r1 = std::sqrt(a*a+b);
            if (r1 >= r)
                *pData &= 0x00FFFFFF;
            else
            {
                quint32 alpha = int(((*pData>>24)&0xFF) * (1-r1/r))&0xFF;
                *pData &= 0x00FFFFFF;
                *pData |= alpha<<24;
            }
            pData++;
        }
    }

    QImage* pImg = new QImage(w, h, QImage::Format_ARGB32);
    quint32 lineSize = quint32(w)*sizeof(quint32);
    for (int y=0; y<h; ++y)
    {
        uchar* pLine = pImg->scanLine(y);
        memcpy(pLine, pBits, lineSize);
        pBits+=lineSize;
    }
    return pImg;
}
