#include "qtablinedetailwidget.h"
#include "../Project/tabproject.h"
#include "../Utils/utils.h"

#include <QPainter>

QTabLineDetailWidget::QTabLineDetailWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void QTabLineDetailWidget::setTabLineInfo(TabLinePtr tl)
{
    m_tl = tl;
    m_img = TAB_INST->getImage(tl->strImg);
    update();
}


QRect QTabLineDetailWidget::getImagePaintRect()
{
    QRect rcDst = rect();
    QRect rcSrc = m_tl->rcPos;

    QRect rcCenter;
    QImageUtil::getCenterPaintingRect(rcSrc.size(), rcDst, rcCenter);
    //qDebug() << "Img: " << rcSrc.size() << " Dst: " << rcDst << " Center: " << rcCenter;
    return rcCenter;
}

void QTabLineDetailWidget::paintEvent(QPaintEvent *event)
{
    if (m_tl.isNull())
        return QWidget::paintEvent(event);

    //draw the image at the center
    QRect rcCenter = getImagePaintRect();

    QPainter p;
    p.begin(this);
    p.drawImage(rcCenter, *m_img, m_tl->rcPos);


    //播放时的总高度，绘制个背景作为参考
    QBrush br(QColor(0xFF,0xFF,0xFF,0x4F));
    int h = TAB_INST->getTabLineSizeH().height();
    if (h < m_tl->rcPos.height())
        h = m_tl->rcPos.height();
    double dh = rcCenter.height() * 1.0 / m_tl->rcPos.height() * h;
    QRectF rcBg(rcCenter.left(), rcCenter.top() - (dh - rcCenter.height())/2,
               rcCenter.width(), h);
    p.fillRect(rcBg, br);

    //绘制三条线作为参考
    QPen pen(QColor(0,0x7F,0xFF,200));
    pen.setWidth(1);
    p.setPen(pen);

    double pos[3]={0.1,0.5,0.9};
    for (auto it : pos)
    {
        QPointF pt1(rcBg.left(), rcBg.top()+rcBg.height()*it), pt2(rcBg.right(), rcBg.top()+rcBg.height()*it);
        p.drawLine(pt1, pt2);
    }
    p.end();

}


void QTabLineDetailWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    const static Qt::CursorShape cs[] =
    {
        Qt::SizeHorCursor,
        Qt::SizeVerCursor,
        Qt::SizeHorCursor,
        Qt::SizeVerCursor,
    };

    if (!m_bDragging)
    {
        const int w=4;
        QRect rcImg = getImagePaintRect();
        QRect rc[] =
        {
            QRect(rcImg.left()-w, rcImg.top(), w+w, rcImg.height()),
            QRect(rcImg.left(), rcImg.top()-w, rcImg.width(), w+w),
            QRect(rcImg.right()-w, rcImg.top(), w+w, rcImg.height()),
            QRect(rcImg.left(), rcImg.bottom()-w, rcImg.width(), w+w)
        };

        for (int i=0; i<4; ++i)
        {
            if (rc[i].contains(event->pos()))
            {
                m_mousePos = MousePos(i);
                setCursor(cs[i]);
                return;
            }
        }

        m_mousePos = MP_NONE;
        setCursor(Qt::ArrowCursor);
    }
    else
    {
        QSize sz = m_img->size();
        int xDif = event->pos().rx() -  m_ptLastClick.rx();
        int yDif = event->pos().ry() -  m_ptLastClick.ry();
        switch (m_mousePos)
        {
        case MP_L:
            m_tl->rcPos.setLeft(m_rcDragBegin.left()+xDif);
            if (m_tl->rcPos.left() < 0) m_tl->rcPos.setLeft(0);
            if (m_tl->rcPos.left() > m_tl->rcPos.right() - 20) m_tl->rcPos.setLeft(m_tl->rcPos.right() - 20);
            break;
        case MP_T:
            m_tl->rcPos.setTop(m_rcDragBegin.top()+yDif);
            if (m_tl->rcPos.top() < 0) m_tl->rcPos.setTop(0);
            if (m_tl->rcPos.top() > m_tl->rcPos.bottom() - 20) m_tl->rcPos.setTop(m_tl->rcPos.bottom() - 20);
            break;
        case MP_R:
            m_tl->rcPos.setRight(m_rcDragBegin.right()+xDif);
            if (m_tl->rcPos.right() > sz.width()) m_tl->rcPos.setRight(sz.width());
            if (m_tl->rcPos.left() > m_tl->rcPos.right() - 20) m_tl->rcPos.setRight(m_tl->rcPos.left() + 20);
            break;
        case MP_B:
            m_tl->rcPos.setBottom(m_rcDragBegin.bottom()+yDif);
            if (m_tl->rcPos.bottom() > sz.height()) m_tl->rcPos.setBottom(sz.height());
            if (m_tl->rcPos.top() > m_tl->rcPos.bottom() - 20) m_tl->rcPos.setBottom(m_tl->rcPos.top() + 20);
            break;
        default:
            return;
        }
        update();
    }
}

void QTabLineDetailWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_mousePos != MP_NONE)
    {
        m_ptLastClick = event->pos();
        m_rcDragBegin = m_tl->rcPos;
        m_bDragging = true;
    }

    event->accept();
}

void QTabLineDetailWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_bDragging = false;
    event->accept();
}

void QTabLineDetailWidget::wheelEvent(QWheelEvent *event)
{
    event->accept();
    if (m_mousePos == MP_NONE || m_tl.isNull())
        return;


    QSize sz = m_img->size();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        switch (m_mousePos)
        {
        case MP_L:
            m_tl->rcPos.setLeft(m_tl->rcPos.left()-numSteps.ry());
            if (m_tl->rcPos.left() < 0) m_tl->rcPos.setLeft(0);
            if (m_tl->rcPos.left() > m_tl->rcPos.right() - 20) m_tl->rcPos.setLeft(m_tl->rcPos.right() - 20);
            break;
        case MP_T:
            m_tl->rcPos.setTop(m_tl->rcPos.top()-numSteps.ry());
            if (m_tl->rcPos.top() < 0) m_tl->rcPos.setTop(0);
            if (m_tl->rcPos.top() > m_tl->rcPos.bottom() - 20) m_tl->rcPos.setTop(m_tl->rcPos.bottom() - 20);
            break;
        case MP_R:
            m_tl->rcPos.setRight(m_tl->rcPos.right()-numSteps.ry());
            if (m_tl->rcPos.right() > sz.width()) m_tl->rcPos.setRight(sz.width());
            if (m_tl->rcPos.left() > m_tl->rcPos.right() - 20) m_tl->rcPos.setRight(m_tl->rcPos.left() + 20);
            break;
        case MP_B:
            m_tl->rcPos.setBottom(m_tl->rcPos.bottom()-numSteps.ry());
            if (m_tl->rcPos.bottom() > sz.height()) m_tl->rcPos.setBottom(sz.height());
            if (m_tl->rcPos.top() > m_tl->rcPos.bottom() - 20) m_tl->rcPos.setBottom(m_tl->rcPos.top() + 20);
            break;
        default:
            return;
        }
    }

    emit signalTabLineChanged();
    repaint();
}
