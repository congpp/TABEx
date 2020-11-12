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

    ptrImg.reset(new QImage(":/image/resource/image/welcome.png"));
    m_imgs[RID_IMG_WELCOME_BG] = ptrImg;

    ptrImg.reset(new QImage(":/image/resource/image/welcome2.png"));
    m_imgs[RID_IMG_WELCOME2_BG] = ptrImg;

    ptrImg.reset(new QImage(":/image/resource/image/shadow.png"));
    m_imgs[RID_IMG_SHADOW] = ptrImg;
}

void ResourceLoader::registerExternalImage(QString key, QImagePtr img)
{
    if (img.isNull())
        return;

    m_extImgs[key] = img;
}

QImagePtr ResourceLoader::findExternalImage(QString key)
{
    auto it = m_extImgs.find(key);
    if (it == m_extImgs.end())
        return nullptr;

    return it.value();
}

ResourceLoader &ResourceLoader::instance()
{
    static ResourceLoader resLoader;
    return resLoader;
}
