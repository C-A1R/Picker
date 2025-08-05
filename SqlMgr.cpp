#include "SqlMgr.h"

#include <QFile>
#include <QSqlQuery>
#include <QSqlRecord>

SqlMgr::SqlMgr(const QString &dbFilename)
    : dbFilename{dbFilename}
{
}

SqlMgr::~SqlMgr()
{
    close();
}

bool SqlMgr::open()
{
    db = new QSqlDatabase();
    *db = QSqlDatabase::addDatabase("QSQLITE");
    db->setDatabaseName(dbFilename);
    if (!db->open())
    {
        close();
        return false;
    }
    return true;
}

void SqlMgr::close()
{
    if (!db)
    {
        return;
    }
    if (db->isOpen())
    {
        db->close();
    }
    const QString conName = db->connectionName();
    delete db;
    db = nullptr;
    QSqlDatabase::removeDatabase(conName);
}

bool SqlMgr::exec(const QString &sql)
{
    bool success = false;
    if (!db)
    {
        return success;
    }

    QSqlQuery query(*db);
    if (!query.exec(sql))
    {
        qDebug() << "Не удалось выполнить запрос: " << sql;
    }
    else
    {
        success = true;
    }
    query.clear();
    query.finish();
    return success;
}

bool SqlMgr::transaction()
{
    return exec("BEGIN TRANSACTION;");
}

bool SqlMgr::commit()
{
    return exec("COMMIT;");
}

bool SqlMgr::rollback()
{
    return exec("ROLLBACK;");
}

bool SqlMgr::createPickerDb()
{
    if (!createInfoTable())
        return false;
    if (!createItemsTable())
        return false;
    if (!createLinksTable())
        return false;
    return true;
}

bool SqlMgr::insertItem(const qulonglong id, const qulonglong parentId, const double orderIndex, const Qt::CheckState print
                                  , const Qt::CheckState resultHolder, const bool expanded, const QString &localPath)
{
    const QString fields = QStringLiteral("%1,%2,\"%3\",%4,%5,%6,%7")
                                .arg(ItemsTable::Fields::id                  // 1
                                    , ItemsTable::Fields::parentId           // 2
                                    , ItemsTable::Fields::order              // 3
                                    , ItemsTable::Fields::printCheckstate    // 4
                                    , ItemsTable::Fields::resultHolder       // 5
                                    , ItemsTable::Fields::expanded           // 6
                                    , ItemsTable::Fields::localPath);        // 7

    const QString values = QStringLiteral("%1,%2,%3,%4,%5,%6,'%7'")
                    .arg(id)                                                                    // 1
                    .arg(parentId)                                                              // 2
                    .arg(orderIndex)                                                            // 3
                    .arg(print == Qt::Unchecked ? 0 : (print == Qt::PartiallyChecked ? 1 : 2))  // 4
                    .arg(resultHolder == Qt::Unchecked ? 0 : 1)                                 // 5
                    .arg(expanded)                                                              // 6
                    .arg(localPath);                                                            // 7

    return exec(QStringLiteral("INSERT INTO %1 (%2) VALUES (%3);").arg(ItemsTable::tableName, fields, values));
}

bool SqlMgr::insertLink(const qulonglong itemId, const QString &srcPath)
{
    const QString sql = QStringLiteral("INSERT INTO %1 (%2,%3) VALUES (%4,'%5');");
    return exec(sql.arg(LinksTable::tableName
                        , LinksTable::Fields::itemId
                        , LinksTable::Fields::srcPath)
                   .arg(itemId)
                   .arg(srcPath));
}

bool SqlMgr::readItems(QList<QSqlRecord> &result)
{
    const QString sql = QStringLiteral("SELECT * FROM %1 ORDER BY \"%2\"");
    return table(sql.arg(ItemsTable::tableName, ItemsTable::Fields::order), result);
}

bool SqlMgr::readLinks(QList<QSqlRecord> &result)
{
    const QString sql = QStringLiteral("SELECT * FROM %1");
    return table(sql.arg(LinksTable::tableName), result);
}

bool SqlMgr::table(const QString &sql, QList<QSqlRecord> &result)
{
    if (!db)
    {
        return false;
    }

    result.clear();
    QSqlQuery query(*db);
    if (!query.exec(sql))
    {
        qDebug() << "Не удалось выполнить запрос: " << sql;
        query.clear();
        query.finish();
        return false;
    }
    else
    {
        result.reserve(query.size());
        while (query.next())
        {
            result.append(query.record());
        }
    }
    query.clear();
    query.finish();
    return true;
}

bool SqlMgr::createInfoTable()
{
    QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS %1 (%2 VARCHAR);");
    if (!exec(sql.arg(InfoTable::tableName, InfoTable::Fields::version)))
        return false;
    sql = QStringLiteral("INSERT INTO %1 (%2) VALUES (%3);");
    return exec(sql.arg(InfoTable::tableName, InfoTable::Fields::version, QStringLiteral("'%1'").arg(APP_VERSION)));
}

bool SqlMgr::createItemsTable()
{
    const QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS %1"
                                       "(%2 INTEGER PRIMARY KEY NOT NULL"   //id
                                       ", %3 INTEGER"                       //parentId
                                       ", \"%4\" REAL"                      //order
                                       ", %5 INTEGER(1) DEFAULT 0"          //printCheckstate
                                       ", %6 BOOL DEFAULT false"            //resultHolder
                                       ", %7 BOOL DEFAULT false"            //expanded
                                       ", %8 TEXT);");                      //localPath
    return exec(sql.arg(ItemsTable::tableName                    // 1
                        , ItemsTable::Fields::id                 // 2
                        , ItemsTable::Fields::parentId           // 3
                        , ItemsTable::Fields::order              // 4
                        , ItemsTable::Fields::printCheckstate    // 5
                        , ItemsTable::Fields::resultHolder       // 6
                        , ItemsTable::Fields::expanded           // 7
                        , ItemsTable::Fields::localPath));       // 8
}

bool SqlMgr::createLinksTable()
{
    QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS %1 "
                                 "(%2 INTEGER PRIMARY KEY AUTOINCREMENT"    //id
                                 ", %3 INTEGER"                             //itemId
                                 ", %4 TEXT);");                            //srcPath
    return exec(sql.arg(LinksTable::tableName
                        , LinksTable::Fields::id
                        , LinksTable::Fields::itemId
                        , LinksTable::Fields::srcPath));
}
