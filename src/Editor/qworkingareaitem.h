#ifndef QWORKINGAREAITEM_H
#define QWORKINGAREAITEM_H
#include "qpreviewitem.h"
#include <QtWidgets>

//点击类型
enum HitType
{
    HT_LT,
    HT_MT,
    HT_RT,
    HT_LM,
    HT_RM,
    HT_LB,
    HT_MB,
    HT_RB,
    HT_ROTATE,
    HT_INSIDE,

    HT_NONE,
};

//要和HitType一一对应
enum AnchorIndex
{
    AI_LT,
    AI_MT,
    AI_RT,
    AI_LM,
    AI_RM,
    AI_LB,
    AI_MB,
    AI_RB,
    AI_ROTATE,

    AI_MAX
};

//正在操作的边
enum RectSideFlag
{
    RSF_NONE    = 0x00,
    RSF_LEFT    = 0x01,
    RSF_TOP     = 0x02,
    RSF_RIGHT   = 0x04,
    RSF_BOTTOM  = 0x08,
    RSF_ALL     = 0x0F,
};

class QControlPannelLayout : public QHBoxLayout
{
public:
    virtual void setGeometry(const QRect &rect) override
    {
        Q_UNUSED(rect);
        QHBoxLayout::setGeometry(m_rc);
    }

    void setFixedGeometry(const QRect &rc)
    {
        m_rc = rc;
    }

    QRect fixedGeometry(){return m_rc;}
private:
    QRect m_rc;
};

class QWorkingAreaItem : public QWidget
{
    Q_OBJECT
public:
    QWorkingAreaItem(QWidget *parent = nullptr);
    ~QWorkingAreaItem() override;

    bool setImage(QString strImgPath);
    bool setImage(QImagePtr img);

    void setDrawAnchor(bool bDraw);
    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

protected:
    QRectF getImageDrawRect();
    void getAnchorRects(QVector<QRect>& rects);
    int resizeCropRectFree(const QPoint &ptMouse);
    void updateControlPannel();
    void initCropRect();
    void adjustCropRect(QSize szOld);
public:
    QRect getCropSrcRect(int rsf);
    QRectF getCropRect();
    void setCropRect(QRectF rc);
signals:
    void signalAddTabLine(QString strImgPath, QRect rcImg);

    void signalCropRectChanged();
public slots:
    void onItemToggled(bool bChecked);
    void onButtonAddClicked(bool bChecked);

protected:
    bool        m_bDrawAnchor = false;      //是否显示anchor
    HitType     m_hitType;          //
    QString     m_strCursorStyle;   //鼠标样式
    bool        m_bDragging = false;//正在拖拽
    QPoint      m_ptDragBegin;      //拖拽开始的鼠标位点
    QRectF      m_rcDragBegin;      //拖拽开始的框框位置
    QRectF      m_rcCropRect;       //当前框框位置
    QRectF      m_rcCropRectMax;    //框框最大区域
    QSize       m_szAnchor;         //拖拽框框锚点的大小

    QRect       m_rcSrc;        //实际大小（图片的原始大小）
    QRectF      m_rcCropSrc;    //相对于m_rcSrc的位置
    QRectF      m_rcZoom;       //缩放的视图大小（图片的显示大小）

    //CControlUI*     m_pCropInfo;    //显示m_rcCropSrc的信息
    //QRect           m_rcCropInfoPadding = {};    //额外的padding
    //CControlUI*     m_pCropTarget;  //被Crop的控件
    //QSizeF      m_szRatio;      //框框的固定比例，大于0时生效
    QSize        m_szControlPannel;
    QPushButton* m_pBtnAdd;

    QString      m_strImgPath;
    QImagePtr    m_img;
};

#endif // QWORKINGAREAITEM_H
