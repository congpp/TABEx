#include "ImageModel.h"
#include "tabproject.h"

int PreviewImageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return TAB_INST->getImageCount();
}

int PreviewImageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant PreviewImageModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    //int row = index.row();
    //int col = index.column();

    switch (role) {
    case Qt::DisplayRole:
        //if (row == 0 && col == 1) return QString("<--left");
        //if (row == 1 && col == 1) return QString("right-->");
        //
        //return QString("Row%1, Column%2")
        //        .arg(row + 1)
        //        .arg(col +1);
        break;
    case Qt::FontRole:
        //if (row == 0 && col == 0) { //change font only for cell(0,0)
        //    QFont boldFont;
        //    boldFont.setBold(true);
        //    return boldFont;
        //}
        break;
    case Qt::BackgroundRole:
        //if (row == 1 && col == 2)  //change background only for cell(1,2)
        //    return QBrush(Qt::red);
        break;
    case Qt::TextAlignmentRole:
        //if (row == 1 && col == 1) //change text alignment only for cell(1,1)
        //    return int(Qt::AlignRight | Qt::AlignVCenter);
        break;
    case Qt::CheckStateRole:
        //if (row == 1 && col == 0) //add a checkbox to cell(1,0)
        //    return Qt::Checked;
        break;
    }
    return QVariant();
}

void PreviewImageModel::notifyChanged(int index, int count)
{
    if (count == 0)
        return;

    QModelIndex idx1 = createIndex(index, 0);
    if (count == -1)
        emit dataChanged(idx1, createIndex(TAB_INST->getImageCount(),0));
    else if (count == 1)
        emit dataChanged(idx1, idx1);
    else if (count > 1)
        emit dataChanged(idx1, createIndex(std::min(index+count, TAB_INST->getImageCount()),0));
}

Qt::ItemFlags PreviewImageModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return  Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return  Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool ImageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    if (role == Qt::EditRole)
    {
        if (r >= m_cropRects.size())
        {
            if (r >= 1000)
                return false;
            m_cropRects.resize(r+1);
        }

        m_cropRects[r]=value.toRectF();
    }
    return true;
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if (role == Qt::DisplayRole)
    {
        if (index.row() >= m_cropRects.size())
            return QRectF();

        return m_cropRects.at(index.row());
    }

    return QVariant();
}
