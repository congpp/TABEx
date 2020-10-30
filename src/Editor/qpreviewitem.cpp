#include "qpreviewitem.h"
#include "../Project/tabproject.h"
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>

QPreviewItem::QPreviewItem(QWidget *parent/* = nullptr*/)
    :QPushButton (parent)
{
    setAutoExclusive(true);
    setCheckable(true);
    setStyleSheet(
        "QPushButton {"
        "    border: 1px solid #8f8f91;"
        "    border-radius: 0px;"
        "    background-color: #8f8fF1;"
        "    min-width: 80px;"
        "}"
        ""
        "QPushButton:pressed {"
        "    background-color: #4f4fF1;"
        "}"
        ""
        "QPushButton:checked {"
        "    background-color: #Ff4f41;"
        "}"
        ""
        "QPushButton:flat {"
        "    border: none; /* no border for a flat push button */"
        "}"
        ""
        "QPushButton:default {"
        "    border-color: navy; /* make the default button prominent */"
        "}"
                );
}

bool QPreviewItem::setImage(QString strImgPath)
{
    m_strImgPath = strImgPath;
    TABProject* pProj = TABProject::getInstance();
    m_img = pProj->getImagePreview(strImgPath);
    return !m_img.isNull();
}

QString QPreviewItem::getImagePath()
{
    return m_strImgPath;
}

int QPreviewItem::getImageWidth()
{
    if (m_img.isNull())
        return 0;

    return m_img->width();
}

void QPreviewItem::updateFixedSize(QSize sz)
{
    if (!m_img.isNull() && !m_img->isNull())
    {
        float f = m_img->width() * 1.0f / sz.width();
        QSize szFixed;
        szFixed.setWidth(sz.width());
        szFixed.setHeight(int(m_img->height() / f));
        //this->setMinimumSize(szFixed);
        //this->setMaximumSize(szFixed);
        this->setFixedSize(szFixed);
        qDebug() << szFixed;
    }
}

void QPreviewItem::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    //updateFixedSize(event->size());
}

void QPreviewItem::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    if (m_img.isNull() || m_img->isNull())
        return;

    QPainter p(this);
    QRect rc = this->rect();
    rc.adjust(2,2,-2,-2);
    p.drawImage(rc, *m_img.get());

    if (this->isChecked())
    {
        QBrush br(QColor(64, 64, 128, 128));
        p.fillRect(rc, br);
    }
}

bool QPreviewItem::hasHeightForWidth() const
{
    return true;
}

int QPreviewItem::heightForWidth(int w) const
{
    if (!m_img.isNull() && !m_img->isNull())
    {
        float f = m_img->width() * 1.0f / w;
        int h = int(m_img->height() / f);
        qDebug() << w << "x" << h;
        return h;
    }

    return this->minimumHeight();
}
