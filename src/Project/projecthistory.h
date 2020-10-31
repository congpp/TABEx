#ifndef PROJECTHISTORY_H
#define PROJECTHISTORY_H
#include <QString>
#include <QList>

struct ProjectHistoryInfo
{
    QString filePath;
    QString uuid;
    QString timeAccess;
    int adjustedBeat = 0;
};

typedef QList<ProjectHistoryInfo> ProjectHisotryInfoList;

class ProjectHistory
{
public:
    ProjectHistory();
    ~ProjectHistory();

    bool add(QString projFile, QString uuid, int adjustedBeat);

    bool save();
    bool open();

    ProjectHisotryInfoList getHistrory();

    bool getProjectHistory(QString projFile, ProjectHistoryInfo &hist);
protected:
    QString getSavePath();

protected:
    const int MAX_HISTORY_COUNT = 10;
    ProjectHisotryInfoList m_history;
};

#endif // PROJECTHISTORY_H
