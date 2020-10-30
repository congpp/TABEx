#ifndef QIMAGEBUTTON_H
#define QIMAGEBUTTON_H

#include <QPushButton>
#include "commondefine.h"

class QImageButton: public QPushButton
{
    Q_OBJECT
public:
    QImageButton(QWidget* parent = nullptr);

    bool setImage(QString strImgPath);
    QString getImagePath();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

protected:
    QString m_strImgPath;
    QImagePtr m_img;
};

#endif // QIMAGEBUTTON_H
