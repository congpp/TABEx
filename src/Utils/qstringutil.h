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
    //double的秒转成分:秒的格式
    //e.g. 62.0 => 01:02
    static QString double2MMSS(double t);
    //
    static QString int64ToString(qint64 n);
    static qint64  string2Int64(QString str);
};

#endif // QSTRINGUTIL_H
