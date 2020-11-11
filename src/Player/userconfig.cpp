#include "userconfig.h"
#include "commondefine.h"
#include "../Utils/utils.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

#define USER_CONFIG_FILE_NAME   "userconfig.db"
#define TPF_TABLE               "tpf_config"
#define TPF_PATH                "tpf"
#define TPF_FIXED_HEIGHT        "fixedHeight"
#define TPF_ADJUSTED_SPEED      "adjustSpeed"
#define TPF_LASTACCESS          "lastAccess"

#define TABLE_CHECK(expr) if (!(expr)) return false

UserConfig::UserConfig()
{
    open();
}

UserConfig::~UserConfig()
{
    close();
}

UserConfig *UserConfig::instance()
{
    static UserConfig cfg;
    return &cfg;
}

bool UserConfig::close()
{
    if (!m_db.open())
    {
        qDebug() << "Database is not open!";
        return false;
    }

    m_db.close();
    return true;
}

bool UserConfig::open()
{
    QString filePath = getSavePath();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filePath);
    if (!m_db.open())
    {
        qDebug() << "Cannot open database: " << filePath;
        return false;
    }

    //tpf config
    createTpfConfigTableIfNotExists();
    return true;
}

bool UserConfig::setUserTpfConfig(UserTpfConfig &cfg)
{
    if (!m_db.isOpen() || cfg.tpf.isEmpty())
        return false;

    if (!m_db.transaction())
        return false;

    QSqlQuery query(m_db);
    query.prepare("insert or replace into " TPF_TABLE " values(:tpf, :height, :speed, :time);");
    query.bindValue(0, cfg.tpf.toLower());
    query.bindValue(1, cfg.fixedHeight);
    query.bindValue(2, cfg.adjustedBpm);
    query.bindValue(3, QStringUtil::int64ToString(cfg.lastAccess));

    if (!query.exec())
    {
        qDebug() << query.lastError();
        m_db.rollback();
        return false;
    }
    m_db.commit();
    return true;
}



bool UserConfig::getUserTpfConfig(UserTpfConfig &cfg)
{
    if (!m_db.isOpen() || cfg.tpf.isEmpty())
        return false;

    QSqlQuery query(m_db);
    QString sql("select * from " TPF_TABLE " where " TPF_PATH " =\"" TPF_PATH "\";");
    if (query.exec(sql) && query.next())
    {
        cfg.adjustedBpm = query.record().value(TPF_ADJUSTED_SPEED).toInt();
        cfg.fixedHeight = query.record().value(TPF_FIXED_HEIGHT).toInt();
        cfg.lastAccess = QStringUtil::string2Int64(query.record().value(TPF_LASTACCESS).toString());
        return true;
    }

    return false;
}

QString UserConfig::getSavePath()
{
    return QDir::homePath() + PROJ_PATH_ROOT + USER_CONFIG_FILE_NAME;
}

bool UserConfig::createTpfConfigTableIfNotExists()
{
    if (!m_db.isOpen())
        return false;

    QString sqlCreateTable("create table " TPF_TABLE " ("
                  TPF_PATH " TEXT primary key,"
                  TPF_FIXED_HEIGHT " INTEGER,"
                  TPF_ADJUSTED_SPEED " INTEGER,"
                  TPF_LASTACCESS " TEXT);");

    if (!isTableExists(TPF_TABLE))
    {
        if (!m_db.transaction())
            return false;

        QSqlQuery query(m_db);
        if (!query.exec(sqlCreateTable))
        {
            qDebug() << query.lastError();
            m_db.rollback();
            return false;
        }
        m_db.commit();
    }
    return true;
}

bool UserConfig::isTableExists(QString tableName)
{
    QSqlQuery query(m_db);
    if (query.exec("select name from sqlite_master where name = \"" + tableName + "\""))
    {
        return query.next();
    }
    return false;
}
