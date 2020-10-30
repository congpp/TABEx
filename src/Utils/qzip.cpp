#include "qzip.h"
#include <QFile>

#ifdef _MSC_VER //just for MSVC
#   pragma warning (push)
#   pragma warning (disable: 4996)
#   include "zlib-1.2.11/zlib.h"
#   pragma warning (pop)
#else
#   include "zlib-1.2.11/zlib.h"
#endif

const uint BUFFER_SIZE_MAX = 1u * 1024 * 1024 * 1024;

QZip::QZip()
{

}

bool QZip::zipFile2Buffer(QString fileName, QByteArray &bufZipped)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    //too big
    if (file.size() > BUFFER_SIZE_MAX)
        return false;

    QByteArray fileData = file.readAll();
    if (fileData.isEmpty())
        return false;

    return zipBuffer(fileData, bufZipped) > 0;
}

bool QZip::zipFile2File(QString fileName, QString fileNameZipped)
{
    //QFile fileIn(fileName), fileOut(fileNameZipped);
    //if (!fileIn.open(QIODevice::ReadOnly) || !fileOut.open(QIODevice::WriteOnly))
    //    return false;
    //
    //const int BLOCK_SIZE = 16 * 1024 * 1024;    //16M
    //QByteArray bufZipped;
    //while(!fileIn.atEnd())
    //{
    //    QByteArray fileData = fileIn.read(BLOCK_SIZE);
    //    if (fileData.isEmpty())
    //        return false;
    //
    //    if (zipBuffer(fileData, bufZipped) <= 0)
    //        return false;
    //
    //    fileOut.write(bufZipped.data(), bufZipped.size());
    //}
    char mode[4]={"wb6"};
    file_compress(fileName.toLatin1().data(), mode, fileNameZipped.toLatin1().data());
    return true;
}

int QZip::zipBuffer(const QByteArray &bufSrc, QByteArray &bufZipped)
{
    uLong fileSize = uLong(bufSrc.size());
    uLong len=compressBound(fileSize);
    bufZipped.resize(int(len));
    int err = compress(reinterpret_cast<Bytef*>(bufZipped.data()), &len, reinterpret_cast<const Bytef*>(bufSrc.data()), fileSize);
    if (err != Z_OK)
        return 0;

    return int(len);
}

bool QZip::unzipBuffer2File(const QByteArray &bufZipped, QString fileName)
{
    //int err = uncompress(uncompr, &uncomprLen, compr, comprLen);
    return true;
}

bool QZip::unzipFile2File(QString fileNameZipped, QString fileName)
{
    return true;
}

bool QZip::unzipBuffer(const QByteArray &bufZipped, QByteArray &buf)
{
    return true;
}
