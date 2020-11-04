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
    RID_IMG_WELCOME_BG,
    RID_IMG_WELCOME2_BG,
};

class ResourceLoader
{
public:
    ResourceLoader();

    QImagePtr getImage(ResourceID number);

    void loadImages();

    void registerExternalImage(QString key, QImagePtr img);
    QImagePtr findExternalImage(QString key);

    static ResourceLoader& instance();
protected:
    QMap<int, QImagePtr> m_imgs;
    QMap<QString, QImagePtr> m_extImgs;
};

#define g_resLoader ResourceLoader::instance()

#endif // RESOURCELOADER_H
