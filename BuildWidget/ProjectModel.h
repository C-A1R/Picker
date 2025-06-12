#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "ProjectItem.h"
#include "Enums.h"

#include <QAbstractItemModel>

/**
 * @brief The ProjectModel class
 * Модель для отображения файловой системы проекта
 * Содержит в себе элементы ProjectItem
 */
class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT

    std::shared_ptr<ProjectItem>        rootItem;

    QFileIconProvider                   iconProvider;
    QHash<qulonglong, Qt::CheckState>   checkedItems;
    QHash<qulonglong, Qt::CheckState>   resultHolders;
    QHash<qulonglong, Statuses>         itemStatuses;
    QHash<QString, std::shared_ptr<ProjectItem>>       itemPaths;

    qulonglong idMax = 0;

    const QHash<Statuses, QColor> statusColors =
    {
        {Statuses::DEFAULT,     QColor(Qt::transparent)},
        {Statuses::LISTED,      QColor(100, 221, 23, 50)},
        {Statuses::NOT_LISTED,  QColor(255, 237, 204, 200)}
    };

public:
    Q_DISABLE_COPY_MOVE(ProjectModel)

    explicit ProjectModel(QObject *parent = nullptr);
    ~ProjectModel() override = default;

    QVariant data(const QModelIndex &index, const int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(const int row, const int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool setProjectPath(const QString &rootPath);
    void loadProjectItems();

    QString projectDbFilePath() const;
    std::shared_ptr<const ProjectItem> getRootItem() const;
    QStringList getCheckedPdfPaths() const;
    QStringList getResultHolderPaths() const;

private:
    bool readFromDb();
    [[maybe_unused]] bool scanFilesystemItem(const std::shared_ptr<ProjectItem> &item, double &orderIndex);
    void checkItem(const QModelIndex &index);
    void cleanup();
    void resetResultHolderCheckstates_Up(const QModelIndex &index);
    void resetResultHolderCheckstates_Down(const QModelIndex &index);
    void getCheckedPdf(const std::shared_ptr<const ProjectItem> &item, QStringList &result) const;
    void getResultHolders(const std::shared_ptr<const ProjectItem> &item, QStringList &result) const;

    void insertItem(const std::shared_ptr<ProjectItem> &item, std::shared_ptr<ProjectItem> parentItem = nullptr);

signals:
    void signal_expand(const QModelIndexList &);

public slots:
    void slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState);
    void slot_dropped(const QModelIndex &dropRootIndex, const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices);
};

#endif // PROJECTMODEL_H
