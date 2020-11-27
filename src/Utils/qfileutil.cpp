#include "qfileutil.h"

QFileUtil::QFileUtil()
{

}

QString QFileUtil::getFileName(QString strFullName)
{
    strFullName.replace('\\', '/');
    int idx = strFullName.lastIndexOf('/');
    if (idx < 0)
        return strFullName;

    return strFullName.right(strFullName.length() - idx - 1);
}

QString QFileUtil::getFileNameNoExt(QString strFullName)
{
    strFullName = getFileName(strFullName);
    if (!strFullName.isEmpty())
        strFullName = strFullName.left(strFullName.lastIndexOf('.'));
    return strFullName;
}

QString QFileUtil::getFilePath(QString strFullName)
{
    strFullName.replace('\\', '/');
    int idx = strFullName.lastIndexOf('/');
    if (idx < 0)
        return strFullName;

    return strFullName.left(idx + 1);
}

QString QFileUtil::getFileExt(QString strFileName)
{
    int idx = strFileName.lastIndexOf('.');
    if (idx < 0)
        return "";

    return strFileName.right(strFileName.length() - idx);
}
