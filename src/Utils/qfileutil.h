#ifndef QFILEUTIL_H
#define QFILEUTIL_H
#include <QString>

class QFileUtil
{
public:
    QFileUtil();

    //get file name from file full path string
    static QString getFileName(QString strFullName);
    static QString getFileNameNoExt(QString strFullName);
    //get file path from file full path string, include the last /, e.g. "/temp/"
    static QString getFilePath(QString strFullName);
    //get extention from file name, include the ., e.g. ".png"
    static QString getFileExt(QString strFileName);
};

#endif // QFILEUTIL_H
