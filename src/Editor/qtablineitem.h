#ifndef QTABLINEITEM_H
#define QTABLINEITEM_H
#include <QtWidgets>
#include "commondefine.h"

class QTabLineItem : public QPushButton
{
    Q_OBJECT
public:
    QTabLineItem(QWidget *parent = nullptr);

    void setTabLineInfo(TabLinePtr tl);
    TabLinePtr getTabLineInfo();

    void updateFixedSize(QSize sz);
    // QWidget interface
protected:
    //virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual bool hasHeightForWidth() const override;
    virtual int heightForWidth(int) const override;

private:
    TabLinePtr  m_tl;
    QImagePtr   m_img;
};

#endif // QTABLINEITEM_H
