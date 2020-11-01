#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H
#include <QMap>
#include <QString>
#include "commondefine.h"

enum ResourceID
{
    RID_UNKNOWN = 0,

    RID_IMG_CD,
    RID_IMG_COVER,
    RID_IMG_TRIANGLE,
};

class ResourceLoader
{
public:
    ResourceLoader();

    QImagePtr getImage(ResourceID number);

    void loadImages();

    static ResourceLoader& instance();
protected:
    QMap<int, QImagePtr> m_imgs;
};

#define g_resLoader ResourceLoader::instance()

#endif // RESOURCELOADER_H
