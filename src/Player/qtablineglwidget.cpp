﻿#include "qtablineglwidget.h"
#include "resourceloader.h"
#include "gaussianBlur.h"
#include "../Project/tabproject.h"
#include "../Utils/utils.h"
#include <QtWidgets>

QTabLineGLWidget::QTabLineGLWidget(QWidget *parent)
    //: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    : QOpenGLWidget(parent)
    //: QWidget (parent)
    , m_thread(this)
{
    m_iTabLine = 0;
    setAutoFillBackground(true);
    connect(&m_thread, SIGNAL(finished()), this, SLOT(slotOnThreadFinished()));

    m_pTimerPlay = new QTimer(this);
    connect(m_pTimerPlay, &QTimer::timeout, this, &QTabLineGLWidget::animatePlay);

    //m_paintHandler.reset(new QHorizontalPaintHandler);
    m_paintHandler.reset(new QVerticalPaintHandler);
    m_paintHandler->setPlayStatus(&m_playStatus);
}

QTabLineGLWidget::~QTabLineGLWidget()
{
    if (m_pTimerPlay) delete m_pTimerPlay;
}

bool QTabLineGLWidget::init()
{
    if (m_threadStatus != TLS_UNKNOWN)
        return false;

    m_playStatus = PS_IDLE;
    m_thread.m_tlStatus = TLS_INIT;
    m_paintHandler->init();

    m_pImgCover = IPaintHandler::getCoverImage();

    if (m_pImgCover)
    {
        m_thread.m_imgCoverBlur.reset(new QImage);
        *m_thread.m_imgCoverBlur = m_pImgCover->convertToFormat(QImage::Format_ARGB32);
        m_thread.start();
    }
    else
    {
        //m_thread.finished(0);
        slotOnThreadFinished();
    }

    return true;
}

void QTabLineGLWidget::reset()
{
    m_pTimerPlay->stop();
    m_iTabLine = 0;
    m_playStatus = PS_IDLE;
    m_threadStatus = TLS_UNKNOWN;
    Q_ASSERT(m_thread.isRunning() == false);
}

bool QTabLineGLWidget::play()
{
    if (m_playStatus == PS_FINISHED)
    {
        m_iTabLine = 0;
        m_paintHandler->setTabLineIndex(0);
    }

    m_playStatus = PS_PLAYING;
    startTimerPlayNext();
    return true;
}

bool QTabLineGLWidget::pause()
{
    m_pTimerPlay->stop();
    m_timer.pause();
    m_playStatus = PS_PAUSED;
    return true;
}

PlayStatus QTabLineGLWidget::getPlayStatus()
{
    return m_playStatus;
}

bool QTabLineGLWidget::setPlayingTabLine(int iTabLine, bool bEmitSignal)
{
    if (iTabLine < 0)
        iTabLine = 0;

    if (iTabLine >= TAB_INST->getTabLineCount())
    {
        pause();
        m_playStatus = PS_FINISHED;
        if (bEmitSignal)
            emit signalTabLineChanged(-1);
        //防止不是100%
        m_paintHandler->setTabLineMaskPercent(1.0);
        repaint();
        return true;
    }

    if (m_playStatus == PS_FINISHED)
        m_playStatus = PS_IDLE;

    m_iTabLine = iTabLine;
    m_paintHandler->setTabLineIndex(m_iTabLine);
    repaint();

    startTimerPlayNext();

    if (bEmitSignal)
        emit signalTabLineChanged(iTabLine);
    return true;
}

int QTabLineGLWidget::getPlayingTabLine()
{
    return m_iTabLine;
}

bool QTabLineGLWidget::playNext(int iVal)
{
    m_iTimeCurrent = 0;
    return setPlayingTabLine(m_iTabLine+iVal, true);
}

bool QTabLineGLWidget::playPrev(int iVal)
{
    m_iTimeCurrent = 0;

    //播放完成，然后往上滚一下滚轮，应该选中最后一个tabLine
    if (m_playStatus == PS_FINISHED && m_iTabLine == TAB_INST->getTabLineCount() - 1 && iVal == 1)
        iVal = 0;

    return setPlayingTabLine(m_iTabLine-iVal, true);
}

void QTabLineGLWidget::setFPS(int fps)
{
    m_iFPS = fps;
    m_iFPS = std::min(m_iFPS, 100);
    m_iFPS = std::max(m_iFPS, 1);
}

int QTabLineGLWidget::getFPS()
{
    return m_iFPS;
}

void QTabLineGLWidget::speedUp()
{
    //m_iSpeedIndex++;
    adjustSpeed();
    repaint();
}

void QTabLineGLWidget::slowDown()
{
    //m_iSpeedIndex--;
    adjustSpeed();
    repaint();
}

void QTabLineGLWidget::resetSpeed()
{
    //m_iSpeedIndex = 0;
    adjustSpeed();
    repaint();
}

void QTabLineGLWidget::animatePlay()
{
    qint64 iElapsed = m_timer.elapsed();
    if (m_iTimeCurrent == m_iTimeTotal || iElapsed >= m_iTimeTotal)
    {
        qDebug() << "animate elapsed: " << m_iTimeCurrent << "/" << m_iTimeTotal << " " << iElapsed;
        setPlayingTabLine(m_iTabLine + 1, true);
    }
    else if (iElapsed + m_pTimerPlay->interval() > m_iTimeTotal)
    {
        m_pTimerPlay->stop();
        m_iTimeCurrent = m_iTimeTotal;
        m_pTimerPlay->start(int(m_iTimeTotal - iElapsed));
    }
    else
    {
        m_iTimeCurrent = iElapsed;
    }

    repaint();
}

void QTabLineGLWidget::slotOnThreadFinished()
{
    m_threadStatus = TLS_DONE;
    if (TAB_INST->getTabLineCount() > 0)
        m_paintHandler->setTabLineIndex(0);
    repaint();
}

void QTabLineGLWidget::slogOnTabLineHeightChanged(int h)
{
    m_paintHandler->setFixedTabLineSize(QSize(0, h));
    update();
}

void QTabLineGLWidget::paintEvent(QPaintEvent *event)
{
    //QGLWidget::paintEvent(event);
    static const QSize szLoading(60,60);
    static const QSize szCover(200, 200);

    Q_UNUSED(event);
    QPainter painter;
    painter.begin(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    QRect rc = this->rect();
    QRect rcLoading(rc.left() + rc.width()/2 - szLoading.width()/2, rc.top() + rc.height()/2 - szLoading.height()/2,
                    szLoading.width(), szLoading.height());

    QRect rcTabLine(rc);
    rcTabLine.setTop(rc.top() + int(rc.height() * m_tabLineYPos));
    int maxHeight = m_szTabLineImg.height();
    if (maxHeight > rc.bottom() - rcTabLine.top() - 58)
        maxHeight = rc.bottom() - rcTabLine.top() - 58;
    rcTabLine.setBottom(rcTabLine.top() + maxHeight);

    QRect rcCover(rc.left() + rc.width()/2 - szCover.width()/2, rcTabLine.top() - szCover.height() - 20,
                    szCover.width(), szCover.height());
    switch (m_threadStatus)
    {
    case TLS_UNKNOWN:
    case TLS_INIT:
    case TLS_BG_LOADING:
    case TLS_BG_LOADED:
    case TLS_TABLINE_LOADING:
    {
        QImagePtr img = g_resLoader.getImage(RID_IMG_WELCOME2_BG);
        QRect rcCenter;
        QImageUtil::getCenterPaintingRect(img->size(), rc, rcCenter);
        rcCenter.moveTop(rc.top());
        QColor clr(76, 81, 82);
        painter.fillRect(rc, clr);
        painter.drawImage(rcCenter, *img, img->rect());
        break;
    }
    case TLS_TABLINE_LOADED:
    case TLS_DONE:
    {
        m_paintHandler->setBackgroundImage(m_thread.m_imgCoverBlur, m_thread.m_clrCoverMajor);
        m_paintHandler->setTabLineMaskPercent(m_iTimeTotal==0 ? 0 : m_iTimeCurrent*1.0/m_iTimeTotal);
        m_paintHandler->onPaint(&painter, rc);

        break;
    }
    }

    painter.end();
}

//void QTabLineGLWidget::wheelEvent(QWheelEvent *event)
//{
//    event->accept();
//    int delta = event->delta();
//    if (delta > 0)
//        playNext();
//    else
//        playPrev();
//}

void QTabLineGLWidget::startTimerPlayNext()
{
    adjustSpeed();

    m_pTimerPlay->stop();
    if(m_playStatus != PS_PAUSED)
    {
        //m_iTimeTotal = tick;
        m_iTimeCurrent = 0;
        m_timer.restart();
    }
    else
    {
        m_timer.resume();
    }

    if (m_playStatus == PS_PLAYING)
    {
        m_pTimerPlay->start(1000/m_iFPS);
    }
}

void QTabLineGLWidget::adjustSpeed()
{
    //m_iSpeedIndex = TAB_INST->adjustSpeed(m_iSpeedIndex);
    double currTotal = TAB_INST->getSecondAtTabLine(m_iTabLine) * 1000;;
    double lastTotal = m_iTimeTotal;
    m_iTimeTotal = int(currTotal);
    m_iTimeCurrent = int(m_iTimeCurrent / (lastTotal/currTotal));
    qDebug() << "adjust speed: " << m_iTimeCurrent << "/" << m_iTimeTotal;
}


QTabLineThread::QTabLineThread(QTabLineGLWidget *pParent)
    : m_pParent(pParent)
{

}

QTabLineThread::~QTabLineThread()
{
    reset();
}

void QTabLineThread::run()
{
    if (!m_pParent)
        return;

    reset();

    m_tlStatus = TLS_BG_LOADING;

    //int thisScreen = QApplication::desktop()->screenNumber(m_pParent);
    //QSize szScreen = QGuiApplication::screens().at(thisScreen)->size();
    //qDebug() << szScreen;

    ImageFilter::CGaussBlurFilter filter;
    filter.SetSigma(10);
    QImage imgScreen = m_imgCoverBlur->copy();

    QImageBitTool bitScreen;
    uchar* pSrc = bitScreen.lockBits32Bpp(*m_imgCoverBlur);
    size_t byteCount = bitScreen.getPixelSize();
    uchar *pDst = new uchar[byteCount];
    filter.Filter(pSrc, pDst, imgScreen.width(), imgScreen.height(), 32);
    //filter.Filter(pSrc, pDst, imgScreen.width(), imgScreen.height(), 32);

    bitScreen.unlockBits32Bpp(imgScreen, pDst, byteCount);

    delete [] pDst;

    QPoint pt(imgScreen.width()/2, 0);
    m_imgCoverBlur.reset(QImageUtil::getEdgeTransparentImage(imgScreen, pt));
    //m_imgCoverBlur.save("d:/dst.png", "PNG");
    m_clrCoverMajor = QImageUtil::getMajorColorOfImage(imgScreen);

    qDebug() << m_clrCoverMajor;

    m_tlStatus = TLS_BG_LOADED;

    m_tlStatus = TLS_DONE;
}

void QTabLineThread::reset()
{
    //if (m_pixels)
    //    delete [] m_pixels;
    //m_pixels = nullptr;
    m_tlStatus = TLS_UNKNOWN;
}
