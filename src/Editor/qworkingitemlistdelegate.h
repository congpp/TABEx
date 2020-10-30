#ifndef QTABLINEITEMLISTDELEGATE_H
#define QTABLINEITEMLISTDELEGATE_H

#include <QtWidgets>

class QWorkingAreaItem;

class QWorkingItemListDelegate  : public QStyledItemDelegate
{

    Q_OBJECT
public:
    QWorkingItemListDelegate (QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;


    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

public slots:
    void slotCropRectChanged();
};


#endif // QTABLINEITEMLISTDELEGATE_H
