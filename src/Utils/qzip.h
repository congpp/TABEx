#ifndef QZIP_H
#define QZIP_H
#include <QString>
#include <QByteArray>

/*
 * 仅压缩数据，不是标准的.zip格式
*/
class QZip
{
public:
    QZip();

    bool zipFile2Buffer(QString fileName, QByteArray& bufZipped);
    bool zipFile2File(QString fileName, QString fileNameZipped);
    int  zipBuffer(const QByteArray& bufSrc, QByteArray& bufZipped);

    bool unzipBuffer2File(const QByteArray& bufZipped, QString fileName);
    bool unzipFile2File(QString fileNameZipped, QString fileName);
    bool unzipBuffer(const QByteArray& bufZipped, QByteArray& buf);
};

#endif // QZIP_H
