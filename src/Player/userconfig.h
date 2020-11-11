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
    ~UserConfig();
    static UserConfig *instance();

    bool close();
    bool open();


    //
    bool setUserTpfConfig(UserTpfConfig& cfg);
    bool getUserTpfConfig(UserTpfConfig& cfg);
protected:
    QString getSavePath();
    bool createTpfConfigTableIfNotExists();
    bool isTableExists(QString tableName);
protected:
    QSqlDatabase m_db;
};

#define g_userCfg UserConfig::instance()

#endif // USERCONFIG_H
