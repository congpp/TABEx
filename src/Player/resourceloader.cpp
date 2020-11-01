#include "resourceloader.h"

ResourceLoader::ResourceLoader()
{
}

QImagePtr ResourceLoader::getImage(ResourceID id)
{
    return m_imgs[id];
}

void ResourceLoader::loadImages()
{
    if (!m_imgs.isEmpty())
        return;

    QImagePtr ptrImg;

    ptrImg.reset(new QImage(":/image/resource/image/cd.png"));
    m_imgs[RID_IMG_CD] = ptrImg;

    ptrImg.reset(new QImage(":/image/resource/image/cover.png"));
    m_imgs[RID_IMG_COVER] = ptrImg;

    ptrImg.reset(new QImage(":/image/resource/image/triangle.png"));
    m_imgs[RID_IMG_TRIANGLE] = ptrImg;

}

ResourceLoader &ResourceLoader::instance()
{
    static ResourceLoader resLoader;
    return resLoader;
}
