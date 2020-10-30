#include "qtablineitem.h"
#include "../Project/tabproject.h"

QTabLineItem::QTabLineItem(QWidget *parent)
    : QPushButton (parent)
{

}

void QTabLineItem::setTabLineInfo(TabLinePtr tl)
{
    m_tl = tl;
    m_img = TAB_INST->getImage(tl->strImg);
}

TabLinePtr QTabLineItem::getTabLineInfo()
{
    return m_tl;
}

void QTabLineItem::updateFixedSize(QSize sz)
{
    if (!m_tl->rcPos.isEmpty())
    {
        //根据宽度来调整高度
        float f = m_tl->rcPos.width() * 1.0f / sz.width();
        QSize szFixed;
        szFixed.setWidth(sz.width());
        szFixed.setHeight(int(m_tl->rcPos.height() / f));
        //this->setMinimumSize(szFixed);
        //this->setMaximumSize(szFixed);
        this->setFixedSize(szFixed);

        //float f = m_tl->rcPos.height() * 1.0f / sz.height();
        //QSize szFixed;
        //szFixed.setHeight(sz.height());
        //szFixed.setWidth(int(m_tl->rcPos.width() / f));
        ////this->setMinimumSize(szFixed);
        ////this->setMaximumSize(szFixed);
        //this->setFixedSize(szFixed);
        qDebug() << "QTabLineItem: " << szFixed << m_tl->rcPos << sz;
    }
}

void QTabLineItem::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    if (m_img.isNull() || m_img->isNull()) return;
    QRect rcSrc = m_tl->rcPos;
    if (rcSrc.isEmpty()) return;


    QPainter p(this);
    QRect rcDst = this->rect();
    p.drawImage(rcDst, *m_img.get(), rcSrc);
}

bool QTabLineItem::hasHeightForWidth() const
{
    return true;
}

int QTabLineItem::heightForWidth(int w) const
{
    if (!m_img.isNull() && !m_img->isNull())
    {
        float f = m_img->width() * 1.0f / w;
        int h = int(m_img->height() / f);
        qDebug() << w << "x" << h;
        return h;
    }

    return 0;
}
