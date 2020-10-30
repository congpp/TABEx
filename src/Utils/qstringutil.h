#ifndef QSTRINGUTIL_H
#define QSTRINGUTIL_H
#include <QString>
#include <QRect>

class QStringUtil
{
public:
    QStringUtil();

    //fromat rc to string "l t w h"
    static QString rect2String(QRect rc);
    //construct a qrect from string "l t w h"
    static QRect   string2Rect(QString str);
};

#endif // QSTRINGUTIL_H
