#ifndef PAINTHANDLER_H
#define PAINTHANDLER_H
#include "commondefine.h"
#include "fps.h"
#include "qtimerex.h"
#include <QtWidgets>

enum PaintHandlerType
{
    PHT_UNKNOWN = 0,
    PHT_HORIZONTAL,
    PHT_VERTICAL,
};

enum PlayStatus
{
    PS_IDLE,
    PS_PAUSED,
    PS_PLAYING,
    PS_FINISHED,
};

class IPaintHandler : public QObject
{
    Q_OBJECT
public:
    IPaintHandler();
    virtual ~IPaintHandler(){}
    virtual void onPaint(QPainter* painter, QRect rc) = 0;
    virtual PaintHandlerType type();

    virtual void init();
    virtual void setPlayStatus(PlayStatus* ps);

    virtual void setTabLineSize(QSize szTL);
    virtual void setBackgroundImage(QImagePtr img, QColor clr);
    virtual void setTabLineIndex(int idx);
    virtual void setTabLineMaskPercent(double percent);

    virtual float updateFPS();

    static QImagePtr getCoverImage();
protected:
    virtual void paintCover(QPainter* painter, QRect rc);
    virtual void paintInfo(QPainter* painter, QRect rc);

    virtual void calculateFixedItemRect(QRect rc) = 0;
protected:
    QImagePtr m_imgBg;
    QColor    m_clrBg;
    QImagePtr m_imgCover;
    QImagePtr m_imgSinger;

    QFPS m_fps;
    PlayStatus* m_ps;

    QSize   m_szCoverMax;           //封面最大大小
    QSize   m_szTabLineImg;
    int     m_iTabLine = -1;        //当前索引
    double  m_maskPercent = 0;      //当前tabline播放的百分比
    float   m_lastFps = 0;
    int     m_coverAngle = 0;       //封面旋转的角度

    int     m_iTargetTabLine = -1;  //滚动时的目标索引
    int     m_smoothScroll = 0;     //当前平滑滚动帧
    const int m_smoothScrollMax = 4;//用多少帧来执行平滑滚动

    QRect   m_rcLastPaint;
    QSize   m_szFixedTabItem;       //计算得到的tabline的固定大小，防止有些特别大
    QRect   m_rcTabLine;            //计算得到的tabline的绘制区域
    QRect   m_rcCover;              //计算得到的封面位置
};

class QHorizontalPaintHandler : public IPaintHandler
{
public:
    QHorizontalPaintHandler();
    virtual PaintHandlerType type() override;
    virtual void onPaint(QPainter* painter, QRect rc) override;

    virtual void init() override;

protected:
    virtual void calculateFixedItemRect(QRect rc) override;

protected:
    double m_tabLineYPos = 0.5;
};

class QVerticalPaintHandler : public IPaintHandler
{
public:
    QVerticalPaintHandler();
    virtual PaintHandlerType type() override;
    virtual void onPaint(QPainter* painter, QRect rc) override;

    virtual void init() override;
protected:
    virtual void paintInfo(QPainter* painter, QRect rc) override;
    void paintProgress(QPainter* painter, QRect rc);

    virtual void calculateFixedItemRect(QRect rc) override;

    QSize getFixedImageSize(QRect& rcPaint, const QRect &rcImg);
    void drawRoundImage(QPainter *painter, QRect rcPaint, QImagePtr img, QRect rcImg);

private:
    double m_tabLineXPos = 0.4;
    int m_numImgHeight = 0;
    int m_numBgImgHeight = 24;
};

typedef QSharedPointer<IPaintHandler> PaintHandlerPtr;
#endif // PAINTHANDLER_H
