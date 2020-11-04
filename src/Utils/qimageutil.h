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
    //获取图片居中等比绘制在rcPaint里面的相对区域rcCenter
    static bool getCenterPaintingRect(const QSize& szImg, const QRect& rcPaint, QRect& rcCenter, bool bZoomIn=false);
    //获取图片出现最多的颜色
    static QColor getMajorColorOfImage(QImage &img);
    //构建中心到边沿逐渐透明的图片
    static QImage* getEdgeTransparentImage(QImage &img, QPoint ptCenter=QPoint());
};

#endif // QIMAGEUTIL_H
