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
        static constexpr char const * const tableName{"_info"};
        struct Fields
        {
            static constexpr char const * const version{"version"};
        };
    };
    struct ItemsTable
    {
        static constexpr char const * const tableName{"items"};
        struct Fields
        {
            static constexpr char const * const id{"id"};
            static constexpr char const * const parentId{"parent_id"};
            static constexpr char const * const order{"order"};
            static constexpr char const * const printCheckstate{"print_checkstate"};
            static constexpr char const * const resultHolder{"result_holder"};
            static constexpr char const * const expanded{"expanded"};
            static constexpr char const * const localPath{"local_path"};
        };
    };
    struct LinksTable
    {
        static constexpr char const * const tableName{"links"};
        struct Fields
        {
            static constexpr char const * const id{"id"};
            static constexpr char const * const itemId{"item_id"};
            static constexpr char const * const srcPath{"src_path"};
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
    [[nodiscard]] bool insertItem(const qulonglong id, const qulonglong parentId, const double orderIndex, const Qt::CheckState print
                                            , const Qt::CheckState resultHolder, const bool expanded, const QString &localPath);
    [[nodiscard]] bool insertLink(const qulonglong itemId, const QString &srcPath);
    [[nodiscard]] bool readItems(QList<QSqlRecord> &result);
    [[nodiscard]] bool readLinks(QList<QSqlRecord> &result);

private:
    [[nodiscard]] bool exec(const QString &sql);
    [[nodiscard]] bool table(const QString &sql, QList<QSqlRecord> &result);

    [[nodiscard]] bool createInfoTable();
    [[nodiscard]] bool createItemsTable();
    [[nodiscard]] bool createLinksTable();

};

#endif // SQLMGR_H
