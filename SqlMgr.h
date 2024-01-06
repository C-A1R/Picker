#ifndef SQLMGR_H
#define SQLMGR_H

#include <QSqlDatabase>

class SqlMgr
{
    const QString dbFilename;
    QSqlDatabase *db{nullptr};

public:
    struct ProjectFilesystemTable
    {
        static constexpr char const * const tableName{"project_filesystem"};
        struct columns
        {
            static constexpr char const * const id{"id"};
            static constexpr char const * const printCheckstate{"print_checkstate"};
            static constexpr char const * const resultHolder{"result_holder"};
            static constexpr char const * const path{"path"};
        };
    };

    SqlMgr(const QString &dbFilename);
    ~SqlMgr();

    bool open();
    void close();

    bool transaction();
    bool commit();
    bool rollback();

    bool createPickerDb();
    bool insertProjectElement(const Qt::CheckState print, const QString &path);
    bool readProjectElements(QList<QSqlRecord> &result);

private:
    bool exec(const QString &sql);
    bool table(const QString &sql, QList<QSqlRecord> &result);

};

#endif // SQLMGR_H
