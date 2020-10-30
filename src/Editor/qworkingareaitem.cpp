#include "qworkingareaitem.h"
#include "qtablineconfigdialog.h"
#include "../Project/tabproject.h"
#include "qtablinewidget.h"
#include "qappdata.h"

#include <QPainter>
#include <QDebug>
#include <QtGlobal>


QWorkingAreaItem::QWorkingAreaItem(QWidget *parent)
    : QWidget(parent)
    , m_szAnchor(10,10)
    , m_szControlPannel(200, 40)
{
    setMouseTracking(true);
    //setCheckable(true);

    QControlPannelLayout* pLayout = new QControlPannelLayout;
    pLayout->setFixedGeometry(QRect());
    this->setLayout(pLayout);

    QPushButton* pBtn = new QPushButton(tr("Add"), this);
    pBtn->setMaximumWidth(32);
    pBtn->setVisible(false);
    pBtn->setCheckable(false);
    pBtn->setStyleSheet("QPushButton {"
                        "  border: 1px solid #8f8f91;"
                        "  border-radius: 6px;"
                        "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);"
                        "  min-width: 80px;"
                        "}"
                        "QPushButton:pressed {"
                        "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                        "                                    stop: 0 #dadbde, stop: 1 #f6f7fa);"
                        "}"
                        "QPushButton:flat {"
                        "  border: none; /* no border for a flat push button */"
                        "}"
                        "QPushButton:default {"
                        "  border-color: navy; /* make the default button prominent */"
                        "}");
    connect(pBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonAddClicked(bool)));
    this->layout()->addWidget(pBtn);

    m_pBtnAdd = pBtn;

    //connect(this, SIGNAL(toggled(bool)), this, SLOT(onItemToggled(bool)));
}

QWorkingAreaItem::~QWorkingAreaItem()
{
}

bool QWorkingAreaItem::setImage(QString strImgPath)
{
    m_strImgPath = strImgPath;
    TABProject* pProj = TABProject::getInstance();
    m_img = pProj->getImage(strImgPath);

    return setImage(m_img);
}

bool QWorkingAreaItem::setImage(QImagePtr img)
{
    m_img = img;
    if (m_img)
    {
        m_rcSrc.setCoords(0,0,m_img->width(), m_img->height());
        return true;
    }
    return false;
}

void QWorkingAreaItem::setDrawAnchor(bool bDraw)
{
    m_bDrawAnchor = bDraw;
    QHBoxLayout* pLayout = static_cast<QHBoxLayout*>(this->layout());
    for (int i=0; i<pLayout->count(); ++i)
    {
        pLayout->itemAt(i)->widget()->setEnabled(true);
        pLayout->itemAt(i)->widget()->setVisible(m_bDrawAnchor);
    }
    pLayout->update();
    update();
}

void QWorkingAreaItem::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (m_img.isNull() || m_img->isNull())
        return;

    //if (!m_bDrawAnchor)
    //    setDrawAnchor(true);

    QPainter p(this);
    QRect rc = getImageDrawRect().toRect();
    p.drawImage(rc, *m_img.get());

    //initCropRect();

    if (!m_bDrawAnchor)
        return;

    QRect rcCropRect(m_rcCropRect.toRect());
    //绘制裁剪框
    QPen pen(QColor(0x0000FF));
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    p.drawRect(m_rcCropRect);

    //绘制裁剪框的锚点
    pen.setColor(QColor(0xFF0000));
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    QVector<QRect> rects;
    getAnchorRects(rects);
    p.drawRects(rects);

    p.save();

    //绘制裁剪框外的遮罩
    QBrush br(CLR_FG);
    QRegion rgn(rc);
    rgn = rgn.xored(QRegion(rcCropRect));
    p.setClipRegion(rgn);
    p.fillRect(rc, br);

    p.restore();

    //src
    p.setPen(0x666666);
    QString str = QStringUtil::rect2String(m_rcCropSrc.toRect());
    QRect boundingRect;
    p.drawText(rcCropRect, Qt::TextDontPrint, str, &boundingRect);
    boundingRect.moveLeft(rcCropRect.left() + rcCropRect.width() - boundingRect.width());
    boundingRect.moveTop(rcCropRect.top() + rcCropRect.height() - boundingRect.height());
    p.fillRect(boundingRect, br);
    p.drawText(boundingRect, str);
}

QRectF QWorkingAreaItem::getImageDrawRect()
{
    QRect rcPaint = this->rect();
    if (rcPaint.width() > m_img->width())
    {
        rcPaint.adjust((rcPaint.width() - m_img->width())/2,0,0,0);
        rcPaint.setWidth(m_img->width());
    }

    return rcPaint;
}

void QWorkingAreaItem::getAnchorRects(QVector<QRect> &vecAnchor)
{
    QRect rcLT, rcRT, rcLB, rcRB;
    QSize& szAnchor = m_szAnchor;
    QRect rc(m_rcCropRect.toRect());
    int nW2 = int(ceil(szAnchor.width() / 2.0));
    int nH2 = int(ceil(szAnchor.height() / 2.0));
    /*LT*/
    vecAnchor.push_back(QRect(rc.left() - nW2, rc.top() - nH2, szAnchor.width(), szAnchor.height()));
    /*LM*/
    vecAnchor.push_back(QRect(rc.left() + (rc.width() - szAnchor.width()) / 2, rc.top() - nH2, szAnchor.width(), szAnchor.height()));
    /*LR*/
    vecAnchor.push_back(QRect(rc.right() - nW2, rc.top() - nH2, szAnchor.width(), szAnchor.height()));

    /*ML*/
    vecAnchor.push_back(QRect(rc.left() - nW2, rc.top() + (rc.height() - szAnchor.height()) / 2, szAnchor.width(), szAnchor.height()));
    /*MR*/
    vecAnchor.push_back(QRect(rc.right() - nW2, rc.top() + (rc.height() - szAnchor.height()) / 2, szAnchor.width(), szAnchor.height()));

    /*LB*/
    vecAnchor.push_back(QRect(rc.left() - nW2, rc.bottom() - nH2, szAnchor.width(), szAnchor.height()));
    /*LM*/
    vecAnchor.push_back(QRect(rc.left() + (rc.width() - szAnchor.width()) / 2, rc.bottom() - nH2, szAnchor.width(), szAnchor.height()));
    /*LR*/
    vecAnchor.push_back(QRect(rc.right() - nW2, rc.bottom() - nH2, szAnchor.width(), szAnchor.height()));
}

int QWorkingAreaItem::resizeCropRectFree(const QPoint& ptMouse)
{
    //ASSERT(m_szRatio.Width < 0.00001 && m_szRatio.Height < 0.00001);

    int rsf = RSF_NONE;
    int xDif = ptMouse.x() - m_ptDragBegin.x();
    int yDif = ptMouse.y() - m_ptDragBegin.y();
    switch (m_hitType)
    {
    case HT_LT:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(xDif, yDif, 0, 0);
        //m_rcCropRect.moveLeft(xDif);
        //m_rcCropRect.moveTop(yDif);
        if (m_rcCropRect.left() >= m_rcCropRect.right()) m_rcCropRect.setLeft(m_rcCropRect.right() - 1);
        if (m_rcCropRect.top() >= m_rcCropRect.bottom()) m_rcCropRect.setTop(m_rcCropRect.bottom() - 1);
        rsf |= RSF_LEFT | RSF_TOP;
        break;
    }
    case HT_MT:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(0, yDif, 0, 0);
        //m_rcCropRect.moveTop(yDif);
        if (m_rcCropRect.top() >= m_rcCropRect.bottom()) m_rcCropRect.setTop(m_rcCropRect.bottom() - 1);
        rsf |= RSF_TOP;
        break;
    }
    case HT_RT:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(0, yDif, xDif, 0);
        //m_rcCropRect.moveRight(xDif);
        //m_rcCropRect.moveTop(yDif);
        if (m_rcCropRect.right() <= m_rcCropRect.left()) m_rcCropRect.setRight(m_rcCropRect.left() + 1);
        if (m_rcCropRect.top() >= m_rcCropRect.bottom()) m_rcCropRect.setTop(m_rcCropRect.bottom() - 1);
        rsf |= RSF_RIGHT | RSF_TOP;
        break;
    }
    case HT_LM:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(xDif, 0, 0, 0);
        //m_rcCropRect.moveLeft(xDif);
        if (m_rcCropRect.left() >= m_rcCropRect.right()) m_rcCropRect.setLeft(m_rcCropRect.right() - 1);
        rsf |= RSF_LEFT;
        break;
    }
    case HT_RM:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(0, 0, xDif, 0);
        //m_rcCropRect.moveRight(xDif);
        if (m_rcCropRect.right() <= m_rcCropRect.left()) m_rcCropRect.setRight(m_rcCropRect.left() + 1);
        rsf |= RSF_RIGHT;
        break;
    }
    case HT_LB:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(xDif, 0, 0, yDif);
        //m_rcCropRect.moveLeft(xDif);
        //m_rcCropRect.moveBottom(yDif);
        if (m_rcCropRect.left() >= m_rcCropRect.right()) m_rcCropRect.setLeft(m_rcCropRect.right() - 1);
        if (m_rcCropRect.bottom() <= m_rcCropRect.top()) m_rcCropRect.setBottom(m_rcCropRect.top() + 1);
        rsf |= RSF_LEFT | RSF_BOTTOM;
        break;
    }
    case HT_MB:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(0, 0, 0, yDif);
        //m_rcCropRect.moveBottom(yDif);
        if (m_rcCropRect.bottom() <= m_rcCropRect.top()) m_rcCropRect.setBottom(m_rcCropRect.top() + 1);
        rsf |= RSF_BOTTOM;
        break;
    }
    case HT_RB:
    {
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.adjust(0, 0, xDif, yDif);
        //m_rcCropRect.moveRight(xDif);
        //m_rcCropRect.moveBottom(yDif);
        if (m_rcCropRect.right() <= m_rcCropRect.left()) m_rcCropRect.setRight(m_rcCropRect.left() + 1);
        if (m_rcCropRect.bottom() <= m_rcCropRect.top()) m_rcCropRect.setBottom(m_rcCropRect.top() + 1);
        rsf |= RSF_RIGHT | RSF_BOTTOM;
        break;
    }
    case HT_INSIDE:
    {
        if (xDif < 0 && m_rcDragBegin.left() + xDif < m_rcCropRectMax.left()) xDif = int(m_rcCropRectMax.left() - m_rcDragBegin.left());
        if (xDif > 0 && m_rcDragBegin.right() + xDif > m_rcCropRectMax.right()) xDif =int( m_rcCropRectMax.right() - m_rcDragBegin.right());
        if (yDif < 0 && m_rcDragBegin.top() + yDif < m_rcCropRectMax.top()) yDif = int(m_rcCropRectMax.top() - m_rcDragBegin.top());
        if (yDif > 0 && m_rcDragBegin.bottom() + yDif > m_rcCropRectMax.bottom()) yDif = int(m_rcCropRectMax.bottom() - m_rcDragBegin.bottom());
        m_rcCropRect = m_rcDragBegin;
        m_rcCropRect.translate(xDif, yDif);
        rsf = RSF_ALL;
        break;
    }
    default:
        break;
    }
    if (m_rcCropRect.left() < m_rcCropRectMax.left()) m_rcCropRect.setLeft(m_rcCropRectMax.left());
    if (m_rcCropRect.top() < m_rcCropRectMax.top()) m_rcCropRect.setTop(m_rcCropRectMax.top());
    if (m_rcCropRect.right() > m_rcCropRectMax.right()) m_rcCropRect.setRight(m_rcCropRectMax.right());
    if (m_rcCropRect.bottom() > m_rcCropRectMax.bottom()) m_rcCropRect.setBottom(m_rcCropRectMax.bottom());

    emit signalCropRectChanged();
    return rsf;
}

void QWorkingAreaItem::updateControlPannel()
{
    if (m_rcCropRect.isEmpty())
        return;

    QRect rc = m_rcCropRect.toRect();
    rc.setWidth(m_szControlPannel.width());
    rc.setHeight(m_szControlPannel.height());
    static_cast<QControlPannelLayout*>(this->layout())->setFixedGeometry(rc);

    //m_pBtnAdd->setText(QString::asprintf("Add (%d, %d, %d, %d)", rc.left(), rc.top(), rc.width(), rc.height()));
    //QRect rcSrc = getCropSrcRect(RSF_NONE);
    //m_pBtnAdd->setText(QString::asprintf("Add (%d, %d, %d, %d)", rcSrc.left(), rcSrc.top(), rcSrc.width(), rcSrc.height()));
    this->layout()->update();
}

QRect QWorkingAreaItem::getCropSrcRect(int rsf)
{
    //qDebug("QWorkingAreaItem::getCropSrcRect 0x%x", rsf);
    //qDebug() << "m_rcCropRect " << m_rcCropRect;
    //根据缩放比例，反推原始矩形位置
    if (rsf != RSF_NONE)
    {
        QRectF m_rcZoom = getImageDrawRect();
        double fZoom = 1.0 * m_img->width() / m_rcZoom.width();
        QRectF rcSrc = {};
        rcSrc.setLeft((m_rcCropRect.left() - round(m_rcZoom.left())) * fZoom);
        rcSrc.setLeft(qMax(rcSrc.left(), 0.0));
        rcSrc.setTop((m_rcCropRect.top() - round(m_rcZoom.top())) * fZoom);
        rcSrc.setTop(qMax(rcSrc.top(), 0.0));
        rcSrc.setRight(rcSrc.left() + m_rcCropRect.width() * fZoom);
        rcSrc.setRight(qMin(rcSrc.right(), m_rcSrc.right()+0.0));
        rcSrc.setBottom(rcSrc.top() + m_rcCropRect.height() * fZoom);
        rcSrc.setBottom(qMin(rcSrc.bottom(), m_rcSrc.bottom()+0.0));
        //qDebug() << "rcSrc" << rcSrc;

        if (rsf & RSF_LEFT)
            m_rcCropSrc.setLeft(rcSrc.left());
        if (rsf & RSF_TOP)
            m_rcCropSrc.setTop(rcSrc.top());
        if (rsf & RSF_RIGHT)
            m_rcCropSrc.setRight(rcSrc.right());
        if (rsf & RSF_BOTTOM)
            m_rcCropSrc.setBottom(rcSrc.bottom());
    }

    //qDebug() << "m_rcCropSrc " << m_rcCropSrc;
    return m_rcCropSrc.toRect();
}

QRectF QWorkingAreaItem::getCropRect()
{
    return m_rcCropRect;
}

void QWorkingAreaItem::setCropRect(QRectF rc)
{
    qDebug() << "setCropRect " << rc;
    m_rcCropRect = rc;
    getCropSrcRect(RSF_ALL);
    updateControlPannel();
}

void QWorkingAreaItem::onItemToggled(bool bChecked)
{
    Q_UNUSED(bChecked);
    //qDebug() << "QWorkingAreaItem::onToggled " << bChecked;
    //bChecked = m_bDrawAnchor;
    //QHBoxLayout* pLayout = static_cast<QHBoxLayout*>(this->layout());
    //for (int i=0; i<pLayout->count(); ++i)
    //{
    //    pLayout->itemAt(i)->widget()->setVisible(bChecked);
    //}
}

void QWorkingAreaItem::onButtonAddClicked(bool bChecked)
{
    Q_UNUSED(bChecked);
    //TODO get crop rect related to m_rcCropRect
    QRect rc = getCropSrcRect(RSF_NONE);
    TabLinePtr tl(new TabLine);
        tl->rcPos = rc;
        tl->strImg = m_strImgPath;
        tl->sections = 0;

    QTabLineConfigDialog dlg(this, TLCM_NEW);
    dlg.setTabLine(tl);
    dlg.setModal(true);
    dlg.show();
    if (QDialog::Accepted != dlg.exec())
        return;

    int idx = dlg.getNewTabLinePosition();
    if (idx == -1)
        TAB_INST->addTabLine(tl);
    else
        TAB_INST->insertTabLine(idx, tl);

    QTabLineWidget * pList = static_cast<QTabLineWidget*>(g_appData.findWidget(UI_NAME_TABLINE_LIST));
    if (pList)
    {
        pList->ensureVisible(idx);
        pList->selectItem(idx);
    }
}

void QWorkingAreaItem::mousePressEvent(QMouseEvent *event)
{
    if (m_rcCropSrc.isEmpty())
    {
        getCropSrcRect(RSF_ALL);
        updateControlPannel();
    }

    if (m_hitType != HT_NONE)
    {
        m_bDragging = true;
        m_ptDragBegin = event->pos();
        m_rcDragBegin = m_rcCropRect;
        m_rcCropRectMax = getImageDrawRect();
        //qDebug() << "Press: " << m_ptDragBegin << m_rcDragBegin;
    }

    QWidget::mousePressEvent(event);

    setDrawAnchor(true);
}

void QWorkingAreaItem::mouseReleaseEvent(QMouseEvent *event)
{
    m_bDragging = false;
    QWidget::mouseReleaseEvent(event);
}

void QWorkingAreaItem::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bDragging)
    {
        int rsf = RSF_NONE;
        rsf = resizeCropRectFree(event->pos());
        //qDebug() << "Dragging: " << event->pos() << m_rcCropRect << m_rcCropRectMax;
        //qDebug("rsf = 0x%x", rsf);
        getCropSrcRect(rsf);
        updateControlPannel();
        repaint();
    }
    else
    {
        /*
        #define IDC_ARROW           MAKEINTRESOURCE(32512)
        #define IDC_IBEAM           MAKEINTRESOURCE(32513)
        #define IDC_WAIT            MAKEINTRESOURCE(32514)
        #define IDC_CROSS           MAKEINTRESOURCE(32515)
        #define IDC_UPARROW         MAKEINTRESOURCE(32516)
        #define IDC_SIZE            MAKEINTRESOURCE(32640)
        #define IDC_ICON            MAKEINTRESOURCE(32641)
        #define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
        #define IDC_SIZENESW        MAKEINTRESOURCE(32643)
        #define IDC_SIZEWE          MAKEINTRESOURCE(32644)
        #define IDC_SIZENS          MAKEINTRESOURCE(32645)
        #define IDC_SIZEALL         MAKEINTRESOURCE(32646)
        */
        Qt::CursorShape cursorShape = Qt::CursorShape::ArrowCursor;
        const static Qt::CursorShape strCursortyle[] =
        {
            Qt::SizeFDiagCursor, Qt::SizeVerCursor, Qt::SizeBDiagCursor,
            Qt::SizeHorCursor,               Qt::SizeHorCursor,
            Qt::SizeBDiagCursor, Qt::SizeVerCursor, Qt::SizeFDiagCursor
        };
        bool bHitAnchor = false;
        QPoint pt = event->pos();
        QVector<QRect> vecAnchor;
        getAnchorRects(vecAnchor);
        for (int i=0; i<vecAnchor.size(); ++i)
        {
            //放大HitTest区域，方便鼠标操作
            const int nExpand = 5;
            QRect rcHitTest = vecAnchor[i];
            rcHitTest.adjust(-nExpand, -nExpand, nExpand * 2, nExpand * 2);
            if (rcHitTest.contains(pt))
            {
                cursorShape = strCursortyle[i];
                bHitAnchor = true;
                m_hitType = HitType(i);
                break;
            }
        }

        //if (bHitAnchor && m_szRatio.width() * m_szRatio.height() > 0.00001 && (m_hitType == HT_MT || m_hitType == HT_LM || m_hitType == HT_RM || m_hitType == HT_MB))
        //    bHitAnchor = false;

        if (!bHitAnchor && m_rcCropRect.contains(event->pos()))
        {
            cursorShape = Qt::SizeAllCursor;
            m_hitType = HT_INSIDE;
        }
        else if (!bHitAnchor)
        {
            cursorShape = Qt::ArrowCursor;
            m_hitType = HT_NONE;
        }
        setCursor(cursorShape);

        //qDebug() << "Moving: " << cursorShape;
    }
    QWidget::mouseMoveEvent(event);
}

void QWorkingAreaItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    QRect rc = rect();
    int move = event->pos().ry() - rc.top();
    double newTop = move - m_rcCropRect.height()/2;
    if (newTop < rc.top())
        newTop = rc.top();
    double newBottom = move + m_rcCropRect.height()/2;
    if (newBottom > rc.bottom())
        newBottom = rc.bottom();
    m_rcCropRect.setTop(newTop);
    m_rcCropRect.setBottom(newBottom);
    getCropSrcRect(RSF_TOP|RSF_BOTTOM);
    updateControlPannel();

    setDrawAnchor(true);

    emit signalCropRectChanged();
}


void QWorkingAreaItem::initCropRect()
{
    if (m_rcCropRect.isEmpty())
    {
        QRectF rc = getImageDrawRect();
        m_rcCropRect.setLeft(rc.left() + 10);
        m_rcCropRect.setTop(rc.top() + 10);
        m_rcCropRect.setBottom(m_rcCropRect.top() + 100);
        m_rcCropRect.setRight(rc.right() - 10);
        getCropSrcRect(RSF_ALL);
        updateControlPannel();
    }
}

void QWorkingAreaItem::adjustCropRect(QSize szOld)
{
    //
    if (!szOld.isValid())
        return;

    //根据m_rcCropSrc，反向调整m_rcCropRect
    QRectF rcPaint = getImageDrawRect();
    double d = m_rcSrc.width() * 1.0 / rcPaint.width();
    double left   = rcPaint.left() + m_rcCropSrc.left() / d;
    double top    = rcPaint.top() + m_rcCropSrc.top() / d;
    double right  = left + m_rcCropSrc.width() / d;
    double bottom = top + m_rcCropSrc.height() / d;
    m_rcCropRect.setLeft(left);
    m_rcCropRect.setTop(top);
    m_rcCropRect.setRight(right);
    m_rcCropRect.setBottom(bottom);
    updateControlPannel();

    //emit signalCropRectChanged();
}

void QWorkingAreaItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_rcCropRect.isEmpty())
        initCropRect();
    else
        adjustCropRect(event->oldSize());
}

void QWorkingAreaItem::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    initCropRect();
}

