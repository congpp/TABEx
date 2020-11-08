#include "qstringutil.h"
#include <QTextStream>

QStringUtil::QStringUtil()
{

}

QString QStringUtil::rect2String(QRect rc)
{
    return QString::asprintf("%d %d %d %d", rc.left(), rc.top(), rc.width(), rc.height());
}

QRect QStringUtil::string2Rect(QString str)
{
    int l, t, w, h;
    QTextStream ts(&str);
    ts >> l >> t >> w >> h;
    return QRect(l, t, w, h);
}

QString QStringUtil::double2MMSS(double t)
{
    int i = int(t);
    return QString::asprintf("%02d:%02d", i/60, i%60);
}

QString QStringUtil::int64ToString(qint64 n)
{
    return QString::asprintf("%lld", n);
}

qint64 QStringUtil::string2Int64(QString str)
{
    qint64 n = 0;
    sscanf_s(str.toLocal8Bit().data(), "%lld", &n);
    return n;
}
