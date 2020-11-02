#include "projecthistory.h"
#include "commondefine.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

const QString HISTORY_FILE_PATH("history.json");

ProjectHistory::ProjectHistory()
{
    open();
}

ProjectHistory::~ProjectHistory()
{
    save();
}

bool ProjectHistory::add(QString projFile, QString uuid, int adjustedBeat)
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
    hi.timeAccess = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    hi.adjustedBeat = adjustedBeat;
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
        obj.insert("timeAccess", it.timeAccess);
        obj.insert("adjustedBeat", it.adjustedBeat);
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
        hi.timeAccess = obj.take("timeAccess").toString();
        hi.adjustedBeat = obj.take("adjustedBeat").toInt();
        m_history.push_back(hi);
    }

    return true;
}

ProjectHisotryInfoList ProjectHistory::getHistrory()
{
    return m_history;
}

bool ProjectHistory::getProjectHistory(QString projFile, ProjectHistoryInfo& hist)
{
    for(auto it : m_history)
    {
        if (it.filePath.compare(projFile, Qt::CaseInsensitive) == 0)
        {
            hist = it;
            return true;
        }
    }

    return false;
}

int ProjectHistory::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int size = m_history.size();
    int col = columnCount(parent);
    return size / col + (size % col != 0);
}

int ProjectHistory::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant ProjectHistory::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    if (role == Qt::DisplayRole)
    {
        int col = index.column(), row = index.row();
        int colCount = columnCount(index);
        int index = row * colCount + col;
        if (index >= m_history.size())
            return QVariant();

        ProjectHistoryInfo phi = m_history.at(index);
        QStringList strs;
        strs << phi.filePath << phi.timeAccess;
        return QVariant(strs);

    }
    return QVariant();
}

Qt::ItemFlags ProjectHistory::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

QString ProjectHistory::getSavePath()
{
    return QDir::homePath() + PROJ_PATH_ROOT + "/" + HISTORY_FILE_PATH;
}
