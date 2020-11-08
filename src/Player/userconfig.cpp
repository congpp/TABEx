#include "userconfig.h"
#include "commondefine.h"
#include "../Utils/utils.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

#define USER_CONFIG_FILE_NAME   "userconfig.db"
#define TPF_TABLE               "tpf_config"
#define TPF_ID                  "id"
#define TPF_PATH                "tpf"
#define TPF_FIXED_HEIGHT        "fixedHeight"
#define TPF_ADJUSTED_SPEED      "adjustSpeed"
#define TPF_LASTACCESS          "lastAccess"

#define TABLE_CHECK(expr) if (!(expr)) return false

UserConfig::UserConfig()
{

}

UserConfig *UserConfig::instance()
{
    static UserConfig cfg;
    return &cfg;
}

bool UserConfig::save()
{
    QString filePath = getSavePath();
    if (!QFile::exists(filePath))
        return false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    if (!db.open())
    {
        qDebug() << "Cannot open database: " << filePath;
        return false;
    }

    //tpf config
    createTpfConfigTableIfNotExists();

    return true;
}

bool UserConfig::open()
{
    QString filePath = getSavePath();
    if (!QFile::exists(filePath))
        return false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    if (!db.open())
    {
        qDebug() << "Cannot open database: " << filePath;
        return false;
    }

    //tpf config
    createTpfConfigTableIfNotExists();
    return true;
}

int UserConfig::adjustTpfBpm(QString tpf, int bpm)
{
    return bpm;
}

int UserConfig::getTpfAdjustedBpm(QString tpf)
{
    if (!m_db.isOpen())
        return 0;

    QSqlQuery query(m_db);
    QString sql("select " TPF_ADJUSTED_SPEED " from " TPF_TABLE " where " TPF_PATH " = ");
    sql.append(tpf);
    sql.append(';');

    if (query.exec(sql))
    {
        return query.record().value(TPF_ADJUSTED_SPEED).toInt();
    }

    return 0;
}

QString UserConfig::getSavePath()
{
    return QDir::homePath() + PROJ_PATH_ROOT + "/" + USER_CONFIG_FILE_NAME;
}

bool UserConfig::createTpfConfigTableIfNotExists()
{
    QString sql("create table " TPF_TABLE "("
                  TPF_ID " INTEGER primary key,"
                  TPF_PATH " TEXT,"
                  TPF_FIXED_HEIGHT " INTEGER,"
                  TPF_ADJUSTED_SPEED " INTEGER,"
                  TPF_LASTACCESS " INTEGER);");
    TABLE_CHECK(isTableExists(TPF_TABLE));

    return true;
}

bool UserConfig::isTableExists(QString tableName)
{
    QSqlQuery query(m_db);
    if (query.exec("select * from sqlite_master where name = \"" + tableName + "\""))
    {
        return query.record().count() > 0;
    }
    return false;
}
