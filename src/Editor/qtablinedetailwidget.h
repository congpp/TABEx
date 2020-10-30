#ifndef QTABLINEDETAILWIDGET_H
#define QTABLINEDETAILWIDGET_H

#include <QtWidgets>
#include "commondefine.h"

class QTabLineDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QTabLineDetailWidget(QWidget *parent = nullptr);

    void setTabLineInfo(TabLinePtr tl);
    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    QRect getImagePaintRect();
signals:
    void signalTabLineChanged();
public slots:

protected:
    TabLinePtr  m_tl;
    QImagePtr   m_img;
    enum MousePos
    {
        MP_L,
        MP_T,
        MP_R,
        MP_B,
        MP_NONE
    } m_mousePos = MP_NONE;

    bool m_bDragging = false;
    QRect  m_rcDragBegin;
    QPoint m_ptLastClick;
};

#endif // QTABLINEDETAILWIDGET_H
