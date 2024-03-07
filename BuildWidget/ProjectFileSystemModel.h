#ifndef PROJECTFILESYSTEMMODEL_H
#define PROJECTFILESYSTEMMODEL_H

#include <QFileSystemModel>

class ProjectFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

    class Item
    {
        const QString   m_absolutePath;
        QList<Item>     m_children;
    public:
        Item(const QString &absolutePath) : m_absolutePath{absolutePath} {}

    };

    enum ProjectItemRoles
    {
        StatusRole = Qt::UserRole
    };

    enum Statuses
    {
        DEFAULT = 0,
        LISTED,
        NOT_LISTED
    };

    QHash<quintptr, Qt::CheckState>     resultHolderCheckstates;
    QHash<quintptr, Qt::CheckState>     checkedItems;
    QHash<quintptr, Statuses>           itemStatuses;
    QSet<quintptr>                      hiddenIndices;
    QList<quintptr>                     orders;
    QHash<quintptr, QString>            pathsById;

public:
    enum Columns
    {
        col_Name,
        col_Size,
        col_Type,
        col_DateModified,
        col_ResultHolder,

        col_Max
    };

    ProjectFileSystemModel(QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::DropActions supportedDropActions() const override;
    // bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    const QSet<quintptr> &getHiddenIndices() const;
    const QList<quintptr> &getOrders() const;
    QStringList getResultHolders() const;
    const QStringList getCheckedPdfPaths() const;
    QString listFilePath() const;

private:
    [[maybe_unused]] bool scanForHiddenItems(const QDir &dir);
    void scanDefaultOrder(const QDir &dir);
    bool readOrderFromListFile();
    void scanFilesystem(const QDir &dir, QModelIndexList &additionItems);
    void cleanup();
    void resetResultHolderCheckstates_Up(const QModelIndex &index);
    void resetResultHolderCheckstates_Down(const QModelIndex &index);
    void checkItem(const QModelIndex& index);

signals:
    void signal_expand(const QModelIndexList &);

public slots:
    void slot_dropped(const quintptr droppedIndexId, const QList<quintptr> &draggeddIndicesIds);
    void slot_added(const quintptr droppedIndexId, const QString &fullPaths);
    void slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState);

protected slots:
    void slot_onRootPathChanged();
};

#endif // PROJECTFILESYSTEMMODEL_H
