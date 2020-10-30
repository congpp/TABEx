#include "tablinemodel.h"
#include "tabproject.h"

int TabLineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TAB_INST->getTabLineCount();
}

int TabLineModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TabLineModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    QVariant var;
    //TabLinePtr pTL = TAB_INST->getTabLineAt(index.row());
    var.setValue(index.row());
    return var;
}

void TabLineModel::notifyChanged(int index, int count)
{
    if (count == 0)
        return;

    QModelIndex idx1 = createIndex(index, 0);
    if (count == -1)
        emit dataChanged(idx1, createIndex(TAB_INST->getTabLineCount(),0));
    else if (count == 1)
        emit dataChanged(idx1, idx1);
    else if (count > 1)
        emit dataChanged(idx1, createIndex(std::min(index+count, TAB_INST->getTabLineCount()),0));
}
