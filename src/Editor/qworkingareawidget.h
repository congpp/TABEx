#ifndef QWORKINGAREAWIDGET_H
#define QWORKINGAREAWIDGET_H

#include <QtWidgets>
#include "qworkingareaitem.h"

class QWorkingAreaWidget : public QListView
{
    Q_OBJECT
public:
    explicit QWorkingAreaWidget(QWidget *parent = nullptr);

signals:
    void itemChecked(QString strImgPath);
    void signalAddTabLine(QString strImgPath, QRect rcImg);
public slots:
    void onItemChecked(bool bChecked);
    void ensureVisible(int index);
protected:

    void wheelEvent(QWheelEvent *event) override;
    //virtual void scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint = EnsureVisible) override;
};

#endif // QWORKINGAREAWIDGET_H
