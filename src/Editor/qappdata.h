#ifndef QAPPDATA_H
#define QAPPDATA_H
#include <QMap>
#include <QtWidgets>

static const QString UI_NAME_MAINWINDOW("mainWindow");
static const QString UI_NAME_PREVIEW_LIST("previewList");
static const QString UI_NAME_WORKING_LIST("workingList");
static const QString UI_NAME_TABLINE_LIST("tabLineList");

class QAppData
{
public:
    QAppData();

    void registerWidget(QString strName, QWidget* pWidget);
    QWidget *findWidget(QString strName);
private:
    QMap<QString, QWidget*> m_widgetMap;
};

extern QAppData g_appData;

#endif // QAPPDATA_H
