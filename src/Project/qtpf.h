#ifndef QTPF_H
#define QTPF_H
/*
 * 用于加载工程文件.tpf (Tab Project File)
 * .tpf 是自定义的一种trunk格式
*/

#include "commondefine.h"
#include <QFile>

enum TPFHeaderType
{
    TPFHT_CONFIG,       //config.json
    TPFHT_SINGER_IMAGE, //singer.png
    TPFHT_COVER_IMAGE,  //cover.png
    TPFHT_IMAGES,       //image header
};

#pragma pack(push)
#pragma pack(1)
struct TPFHeader
{
    int type;       //TPFHeaderType
    int length;     //data length
    int version;    //version
    int unused[5];  //unused
};

struct TPFImageHeader
{
    char fileName[64];  //filename, 1.png 2.png...
    int  length;        //image file length
};

#pragma pack(pop)

class QTPF
{
public:
    enum OpenFlag
    {
        OF_READ,
        OF_WRITE,
    };

    QTPF();
    bool open(QString tpfFileName, OpenFlag flag=OF_READ);
    bool appendFile(TPFHeaderType t, QString file);
    bool appendImage(QString file);

    bool releaseToDir(QString dirPath);

    static QImagePtr getProjectCoverImage(QString tpf);
protected:
    bool releaseFile(QString dirPath, TPFHeader tpfh);
    bool releaseImage(QString dirPath, TPFHeader tpfh);
    //bool readFile(TPFHeaderType t, QByteArray& data);
    //bool readImage(QString imgName, QByteArray& data);
protected:
    OpenFlag m_of;
    QFile m_file;
};

#endif // QTPF_H
