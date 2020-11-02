#ifndef PROJECTHISTORY_H
#define PROJECTHISTORY_H
#include <QString>
#include <QList>
#include <QAbstractTableModel>

struct ProjectHistoryInfo
{
    QString filePath;
    QString uuid;
    QString timeAccess;
    int adjustedBeat = 0;
};

typedef QList<ProjectHistoryInfo> ProjectHisotryInfoList;

class ProjectHistory  : public QAbstractTableModel
{
public:
    ProjectHistory();
    ~ProjectHistory() override;

    bool add(QString projFile, QString uuid, int adjustedBeat);

    bool save();
    bool open();

    ProjectHisotryInfoList getHistrory();

    bool getProjectHistory(QString projFile, ProjectHistoryInfo &hist);

    void notifyChanged(int index, int count=1);

public:
    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
protected:
    QString getSavePath();

protected:
    const int MAX_HISTORY_COUNT = 10;
    ProjectHisotryInfoList m_history;
};

#endif // PROJECTHISTORY_H
