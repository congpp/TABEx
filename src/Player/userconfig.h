#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <QObject>
#include <map>
#include <QSqlDatabase>

//用户对某个tpf做出的自定义配置
struct UserTpfConfig
{
    QString tpf;
    int fixedHeight = 0;    //播放时tabline的高度，0=自动计算
    int adjustedBpm = 0;    //速度调整
    qint64 lastAccess = 0;  //最后访问时间
};
typedef std::map<QString, UserTpfConfig> UserTpfConfigMap;

class UserConfig : public QObject
{
    Q_OBJECT
protected:
    UserConfig();
    UserConfig(UserConfig&);
public:
    static UserConfig *instance();

    bool save();
    bool open();


    //通过调整节拍器的每分钟节拍数来调整整首歌的速度
    int adjustTpfBpm(QString tpf, int bpm);
    int getTpfAdjustedBpm(QString tpf);

protected:
    QString getSavePath();
    bool createTpfConfigTableIfNotExists();
    bool isTableExists(QString tableName);
protected:
    QSqlDatabase m_db;
};

#endif // USERCONFIG_H
