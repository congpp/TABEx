#include "qimageutil.h"

QImageUtil::QImageUtil()
{

}

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
