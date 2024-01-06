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
                                       "(%2 INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL"
                                       ", %3 INTEGER(1) DEFAULT 0"
                                       ", %4 BOOL DEFAULT false"
                                       ", %5 TEXT);");
    return exec(sql.arg(ProjectFilesystemTable::tableName
                        , ProjectFilesystemTable::columns::id
                        , ProjectFilesystemTable::columns::printCheckstate
                        , ProjectFilesystemTable::columns::resultHolder
                        , ProjectFilesystemTable::columns::path));
}

bool SqlMgr::insertProjectElement(const Qt::CheckState print, const QString &path)
{
    const QString sql = QStringLiteral("INSERT INTO %1 (%2,%3) VALUES (%4,'%5')");
    return exec(sql.arg(ProjectFilesystemTable::tableName
                        , ProjectFilesystemTable::columns::printCheckstate
                        , ProjectFilesystemTable::columns::path)
                    .arg(print == Qt::Unchecked ? 0 : (print == Qt::PartiallyChecked ? 1 : 2))
                    .arg(path));
}

bool SqlMgr::readProjectElements(QList<QSqlRecord> &result)
{
    const QString sql = QStringLiteral("SELECT * FROM %1");
    return table(sql.arg(ProjectFilesystemTable::tableName), result);
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
        qDebug() << "Не удалось выполнить запрос" << sql;
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
