#include "projecthistory.h"
#include "commondefine.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>

const QString HISTORY_FILE_PATH("history.json");

ProjectHistory::ProjectHistory()
{
    open();
}

ProjectHistory::~ProjectHistory()
{
    save();
}

bool ProjectHistory::add(QString projFile, QString uuid)
{
    projFile = projFile.toLower();
    for (auto it = m_history.begin(); it != m_history.end();)
    {
        if (it->filePath.compare(projFile, Qt::CaseInsensitive) == 0)
            it = m_history.erase(it);
        else
            it++;
    }

    ProjectHistoryInfo hi;
    hi.filePath = projFile;
    hi.uuid = uuid;
    m_history.push_front(hi);

    while (m_history.size() > 10)
        m_history.removeLast();

    return true;
}

bool ProjectHistory::save()
{
    if (m_history.isEmpty())
        return true;

    QString filePath = getSavePath();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;

    QJsonDocument jsDoc;
    QJsonArray arr;
    for (auto it : m_history)
    {
        QJsonObject obj;
        obj.insert("filePath", it.filePath);
        obj.insert("uuid", it.uuid);
        arr.append(obj);
    }
    jsDoc.setArray(arr);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << jsDoc.toJson(QJsonDocument::Indented);
    file.close();
    return true;
}

bool ProjectHistory::open()
{
    m_history.clear();

    QString filePath = getSavePath();
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsDoc = QJsonDocument::fromJson(data);
    if (jsDoc.isNull() || jsDoc.isEmpty())
        return false;

    QJsonArray arr = jsDoc.array();
    for (auto it : arr)
    {
        ProjectHistoryInfo hi;
        QJsonObject obj = it.toObject();
        hi.filePath = obj.take("filePath").toString();
        hi.uuid = obj.take("uuid").toString();
        m_history.push_back(hi);
    }

    return true;
}

ProjectHisotryInfoList ProjectHistory::getHistrory()
{
    return m_history;
}

QString ProjectHistory::getSavePath()
{
    return QDir::homePath() + PROJ_PATH_ROOT + "/" + HISTORY_FILE_PATH;
}
