#ifndef QPREVIEWWIDGET_H
#define QPREVIEWWIDGET_H

#include <QtWidgets>
#include <QHBoxLayout>
#include <QResizeEvent>

enum PopupMenuActionIndex
{
    PMA_ADD_IMG,
    PMA_INSERT_BEFORE,
    PMA_INSERT_AFTER,
    PMA_DELETE,
    PMA_MOVE_UP,
    PMA_MOVE_DOWN,

    PMA_COUNT,
};

static const QString PopupMenuActionName[PMA_COUNT] =
{
    "Add image",
    "Insert image before this",
    "Insert image after this",
    "Remove",
    "Move up",
    "Move down"
};

class QPreviewWidget : public QListView
{
    Q_OBJECT
public:
    explicit QPreviewWidget(QWidget *parent = nullptr);
signals:
    void itemChecked(int index);
public slots:
    //void onItemChecked(bool bChecked);
    void onActionToggled(bool);
    // QWidget interface
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;


private:
    QList<QAction*> m_actions;
    int m_lastMenuClickedIndex = -1;
};

#endif // QPREVIEWWIDGET_H
