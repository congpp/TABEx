#ifndef QPAINTERUTIL_H
#define QPAINTERUTIL_H
#include <QPainter>
#include <QImage>

class QPainterUtil
{
public:
    QPainterUtil();
    static bool paintScale9(QPainter* painter, QImage& img, QRect rcPaint, int r);
};

#endif // QPAINTERUTIL_H
