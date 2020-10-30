#include "qtpf.h"
#include <QFile>
#include <QDir>
#include "../Utils/utils.h"

QTPF::QTPF()
{

}

bool QTPF::open(QString tpfFileName, QTPF::OpenFlag flag)
{
    m_of = flag;
    m_file.setFileName(tpfFileName);
    return m_file.open(flag == OF_READ ? QIODevice::ReadOnly : QIODevice::WriteOnly);
}

bool QTPF::appendFile(TPFHeaderType t, QString file)
{
    if (!m_file.isOpen())
        return false;

    m_file.seek(-1);
    TPFHeader tpfh = {};
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray data = f.readAll();
        tpfh.type = t;
        tpfh.length = data.size();
        m_file.write(reinterpret_cast<const char*>(&tpfh), sizeof(tpfh));
        m_file.write(data);
        return true;
    }

    return false;
}

bool QTPF::appendImage(QString file)
{
    if (!m_file.isOpen())
        return false;

    m_file.seek(-1);
    TPFHeader tpfh = {};
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray data = f.readAll();
        TPFImageHeader tpfih = {};
        QString strName = QFileUtil::getFileName(file);
        strcpy_s(tpfih.fileName, strName.toUtf8().data());
        tpfih.length = data.size();

        tpfh.type = TPFHT_IMAGES;
        tpfh.length = sizeof(tpfih);
        m_file.write(reinterpret_cast<const char*>(&tpfh), sizeof(tpfh));
        m_file.write(reinterpret_cast<const char*>(&tpfih), sizeof(tpfih));
        m_file.write(data);
        return true;
    }

    return false;
}

bool QTPF::releaseToDir(QString dirPath)
{
    if (!m_file.isOpen())
        return false;

    m_file.seek(0);
    TPFHeader tpfh = {};
    while(!m_file.atEnd())
    {
        m_file.read(reinterpret_cast<char*>(&tpfh), sizeof(tpfh));
        switch (tpfh.type)
        {
        case TPFHT_CONFIG:
        case TPFHT_SINGER_IMAGE:
        case TPFHT_COVER_IMAGE:
            if (!releaseFile(dirPath, tpfh))
                return false;
            break;
        case TPFHT_IMAGES:
            if (!releaseImage(dirPath, tpfh))
                return false;
            break;
        default:
            return false;
        }
    }
    return true;
}

bool QTPF::releaseFile(QString dirPath, TPFHeader tpfh)
{
    QString strFile;
    switch (tpfh.type)
    {
    case TPFHT_CONFIG:
    {
       strFile = dirPath + PROJ_PATH_JSON;
       break;
    }
    case TPFHT_SINGER_IMAGE:
    {
       strFile = dirPath + PROJ_PATH_SINGER;
       break;
    }
    case TPFHT_COVER_IMAGE:
    {
       strFile = dirPath + PROJ_PATH_COVER;
       break;
    }
    default:
        return false;
    }

    QByteArray data = m_file.read(tpfh.length);
    if (data.isEmpty())
        return false;

    QFile file(strFile);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(data);
    return true;
}

bool QTPF::releaseImage(QString dirPath, TPFHeader tpfh)
{
    if (tpfh.type != TPFHT_IMAGES || m_file.atEnd())
        return false;

    TPFImageHeader tpfih={};
    m_file.read(reinterpret_cast<char*>(&tpfih), sizeof(tpfih));
    QString fileName = QString::fromUtf8(tpfih.fileName);
    QByteArray data = m_file.read(tpfih.length);
    if (data.isEmpty() || fileName.isEmpty())
        return false;

    QFile file(dirPath + fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(data);
    return true;
}
