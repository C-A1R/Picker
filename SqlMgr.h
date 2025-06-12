#ifndef SQLMGR_H
#define SQLMGR_H

#include <QSqlDatabase>

class SqlMgr
{
    const QString   dbFilename;
    QSqlDatabase    *db {nullptr};

public:
    struct InfoTable
    {
        static constexpr char const * const tableName{"app_info"};
        struct Columns
        {
            static constexpr char const * const version{"version"};
        };
    };
    struct ProjectFilesystemTable
    {
        static constexpr char const * const tableName{"project_filesystem"};
        struct Columns
        {
            static constexpr char const * const id{"id"};
            static constexpr char const * const parentId{"parent_id"};
            static constexpr char const * const order{"\"order\""};
            static constexpr char const * const printCheckstate{"print_checkstate"};
            static constexpr char const * const resultHolder{"result_holder"};
            static constexpr char const * const expanded{"expanded"};
            static constexpr char const * const path{"path"};
        };
    };

    SqlMgr(const QString &dbFilename);
    ~SqlMgr();

    [[nodiscard]] bool open();
    void close();

    [[nodiscard]] bool transaction();
    [[nodiscard]] bool commit();
    [[nodiscard]] bool rollback();

    [[nodiscard]] bool createPickerDb();
    [[nodiscard]] bool insertProjectElement(const qulonglong id, const qulonglong parentId, const double orderIndex, const Qt::CheckState print
                                            , const Qt::CheckState resultHolder, const bool expanded, const QString &path);
    [[nodiscard]] bool readProjectElements(QList<QSqlRecord> &result);

private:
    [[nodiscard]] bool exec(const QString &sql);
    [[nodiscard]] bool table(const QString &sql, QList<QSqlRecord> &result);

    [[nodiscard]] bool createInfoTable();
    [[nodiscard]] bool createProjectFilesystemTable();

};

#endif // SQLMGR_H
