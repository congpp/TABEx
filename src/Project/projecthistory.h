#ifndef PROJECTHISTORY_H
#define PROJECTHISTORY_H
#include <QString>
#include <QTime>

struct ProjectHistoryInfo
{
    QString filePath;
    QString uuid;
};

typedef QList<ProjectHistoryInfo> ProjectHisotryInfoList;

class ProjectHistory
{
public:
    ProjectHistory();
    ~ProjectHistory();

    bool add(QString projFile, QString uuid);

    bool save();
    bool open();

    ProjectHisotryInfoList getHistrory();

protected:
    QString getSavePath();

protected:
    const int MAX_HISTORY_COUNT = 10;
    ProjectHisotryInfoList m_history;
};

#endif // PROJECTHISTORY_H
