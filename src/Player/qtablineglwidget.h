#ifndef QTABLINEGLWIDGET_H
#define QTABLINEGLWIDGET_H
#include "commondefine.h"
#include "fps.h"
#include "qtimerex.h"
#include "painthandler.h"
#include <QOpenGLWidget>
#include <QtWidgets>

enum TabLineStatus
{
    TLS_UNKNOWN,
    TLS_INIT,
    TLS_BG_LOADING,
    TLS_BG_LOADED,
    TLS_TABLINE_LOADING,
    TLS_TABLINE_LOADED,
    TLS_DONE,
};

//to control the drawing
struct RelativePos
{
    double posL = 0;    //left percent
    double posT = 0;    //top percent
    double posW = 0;    //width percent
    double posH = 0;    //height percent
};

class QTabLineGLWidget;
class QTabLineThread : public QThread
{
    Q_OBJECT
public:
    QTabLineThread(QTabLineGLWidget* pParent);
    ~QTabLineThread() override;
    // QThread interface
protected:
    virtual void run() override;
    void reset();
public:
    QTabLineGLWidget* m_pParent;
    QImagePtr m_imgCoverBlur;  //blur the cover using gaussian blur
    //QImage m_imgTabLine;    //draw all tab line in to this image
    //uchar* m_pixels = nullptr;

    TabLineStatus m_tlStatus = TLS_UNKNOWN;
};

//For animate tab line
class QTabLineGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    QTabLineGLWidget(QWidget *parent = nullptr);
    ~QTabLineGLWidget() override;

    bool init();
    void reset();
    bool play();
    bool pause();
    PlayStatus getPlayStatus();
    bool setPlayingTabLine(int iTabLine, bool bEmitSignal=false);
    int getPlayingTabLine();

    bool playNext(int iVal=1);
    bool playPrev(int iVal=1);

    //并不是那么准确，只是让界面动起来而已
    void setFPS(int fps);
    int  getFPS();

    void speedUp();
    void slowDown();
    void resetSpeed();
signals:
    void signalTabLineChanged(int iTabLine);

public slots:
    void animatePlay();
    void slotOnThreadFinished();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    //virtual void wheelEvent(QWheelEvent *event) override;

    void startTimerPlayNext();

    void adjustSpeed();
private:
    friend class QTabLineThread;
    int m_iTabLine = 0;
    QImagePtr m_pImgCover;  //image from project

    QTabLineThread m_thread;
    TabLineStatus  m_threadStatus = TLS_UNKNOWN;

    int m_iFPS = 20;
    qint64 m_iTimeTotal = 0;
    qint64 m_iTimeCurrent = 0;
    QTimer *m_pTimerPlay = nullptr;
    PlayStatus m_playStatus = PS_IDLE;

    //QFPS m_fps;
    QTimerEx m_timer;  //用于校正时间 m_pTimerPlay

    float m_tabLineYPos = 0.5f;

    QSize m_szTabLineImg;

    int m_iSpeedIndex = 0;

    PaintHandlerPtr m_paintHandler;
};

#endif // QTABLINEGLWIDGET_H
