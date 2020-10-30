#ifndef TABLINEMODEL_H
#define TABLINEMODEL_H

#include "commondefine.h"
#include <QAbstractListModel>

class TabLineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void notifyChanged(int index, int count=1);
};


#endif // TABLINEMODEL_H
