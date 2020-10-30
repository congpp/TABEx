#ifndef QTABLINEWIDGET_H
#define QTABLINEWIDGET_H

#include <QtWidgets>
#include "commondefine.h"

class QTabLineWidget : public QListView
{
    Q_OBJECT
public:
    explicit QTabLineWidget(QWidget *parent = nullptr);

    void onContextMenuEvent(int iItem);

    TabLineList getTabLineFromClipboard();

    //获取当前选中的个数
    int getSelectedCount();
    int getFirstSelectedIndex();
    int getLastSelectedIndex();


protected:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

signals:

public slots:
    void onActionToggled(bool);
    void ensureVisible(int index);
    void selectItem(int index);
private:
    enum PopupMenuActionIndex
    {
        PMA_MOVE_LEFT,
        PMA_MOVE_RIGHT,
        PMA_COPY,
        PMA_CUT,
        PMA_PASTE,
        PMA_PASTE_BEFORE,
        PMA_PASTE_AFTER,
        PMA_DELETE,

        PMA_COUNT,
    };

    const QString PopupMenuActionName[PMA_COUNT] =
    {
        "Move Up",
        "Move Down",
        "Copy",
        "Cut",
        "Paste (append)",
        "Paste (before this)",
        "Paste (after this)",
        "Remove",
    };

    QList<QAction*> m_actions;
    int m_lastMenuClickedIndex = -1;
};

#endif // QTABLINEWIDGET_H
