#ifndef QPREVIEWITEM_H
#define QPREVIEWITEM_H
#include <QPushButton>
#include "commondefine.h"

class QPreviewItem : public QPushButton
{
    Q_OBJECT
public:
    QPreviewItem(QWidget *parent = nullptr);

    virtual bool setImage(QString strImgPath);
    QString getImagePath();

    int getImageWidth();

    void updateFixedSize(QSize sz);

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual bool hasHeightForWidth() const override;
    virtual int heightForWidth(int) const override;

protected:
    QString m_strImgPath;
    QImagePtr m_img;
};

#endif // QPREVIEWITEM_H
