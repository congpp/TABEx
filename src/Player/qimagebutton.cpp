#include "qimagebutton.h"
#include "../Project/tabproject.h"
#include <QPainter>

QImageButton::QImageButton(QWidget* parent)
    : QPushButton (parent)
{

}

bool QImageButton::setImage(QString strImgPath)
{
    QImagePtr pImg = TAB_INST->getImage(strImgPath);
    if (pImg.isNull())
        pImg = QImagePtr(new QImage(strImgPath));

    if (pImg.isNull())
        return false;

    m_strImgPath = strImgPath;
    m_img = pImg;

    return true;
}

QString QImageButton::getImagePath()
{
    return m_strImgPath;
}

void QImageButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    if (m_img.isNull() || m_img->isNull())
        return;

    QPainter p(this);
    QRect rc = this->rect();

    p.drawImage(rc, *m_img.get());
}
