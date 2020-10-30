#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include "commondefine.h"
#include <QAbstractListModel>

class PreviewImageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void notifyChanged(int index, int count=1);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};


class ImageModel : public PreviewImageModel
{
    Q_OBJECT
public:
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant data(const QModelIndex &index, int role) const override;

protected:
    QVector<QRectF> m_cropRects;
};
#endif // IMAGEMODEL_H
