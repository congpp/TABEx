#include "qappdata.h"

QAppData g_appData;

QAppData::QAppData()
{

}

void QAppData::registerWidget(QString strName, QWidget *pWidget)
{
    if (strName.isEmpty() || !pWidget)
        return;

    m_widgetMap[strName] = pWidget;
}

QWidget *QAppData::findWidget(QString strName)
{
    auto it = m_widgetMap.find(strName);
    if (it == m_widgetMap.end())
        return nullptr;

    return it.value();
}
