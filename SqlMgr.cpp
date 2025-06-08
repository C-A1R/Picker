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
    const QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS %1"
                                       "(%2 INTEGER PRIMARY KEY NOT NULL"   //id
                                       ", %3 INTEGER"                       //parentId
                                       ", %4 REAL"                          //order
                                       ", %5 INTEGER(1) DEFAULT 0"          //printCheckstate
                                       ", %6 BOOL DEFAULT false"            //resultHolder
                                       ", %7 BOOL DEFAULT false"            //expanded
                                       ", %8 TEXT);");                      //path
    return exec(sql.arg(ProjectFilesystemTable::tableName
                        , ProjectFilesystemTable::Columns::id
                        , ProjectFilesystemTable::Columns::parentId
                        , ProjectFilesystemTable::Columns::order
                        , ProjectFilesystemTable::Columns::printCheckstate
                        , ProjectFilesystemTable::Columns::resultHolder
                        , ProjectFilesystemTable::Columns::expanded
                        , ProjectFilesystemTable::Columns::path));
}

bool SqlMgr::insertProjectElement(const qulonglong id, const qulonglong parentId, const double orderIndex, const Qt::CheckState print
                                  , const Qt::CheckState resultHolder, const bool expanded, const QString &path)
{
    const QString sql = QStringLiteral("INSERT INTO %1 (%2,%3,%4,%5,%6,%7,%8) VALUES (%9,%10,%11,%12,%13,%14,'%15');");
    return exec(sql.arg(ProjectFilesystemTable::tableName
                        , ProjectFilesystemTable::Columns::id
                        , ProjectFilesystemTable::Columns::parentId
                        , ProjectFilesystemTable::Columns::order
                        , ProjectFilesystemTable::Columns::printCheckstate
                        , ProjectFilesystemTable::Columns::resultHolder
                        , ProjectFilesystemTable::Columns::expanded
                        , ProjectFilesystemTable::Columns::path)
                    .arg(id)
                    .arg(parentId)
                    .arg(orderIndex)
                    .arg(print == Qt::Unchecked ? 0 : (print == Qt::PartiallyChecked ? 1 : 2))
                    .arg(resultHolder == Qt::Unchecked ? 0 : 1)
                    .arg(expanded)
                    .arg(path));
}

bool SqlMgr::readProjectElements(QList<QSqlRecord> &result)
{
    const QString sql = QStringLiteral("SELECT * FROM %1 ORDER BY %2");
    return table(sql.arg(ProjectFilesystemTable::tableName, ProjectFilesystemTable::Columns::order), result);
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
