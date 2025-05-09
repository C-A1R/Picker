#include "ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_unique<ProjectItem>("root"))
{
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    ProjectItem *parentItem = parent.isValid() ? static_cast<ProjectItem*>(parent.internalPointer())
                                               : rootItem.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex ProjectModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<ProjectItem*>(index.internalPointer());
    ProjectItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get() ? createIndex(parentItem->row(), 0, parentItem)
                                        : QModelIndex{};
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const ProjectItem *parentItem = parent.isValid() ? static_cast<const ProjectItem*>(parent.internalPointer())
                                                     : rootItem.get();

    return parentItem->childCount();
}

int ProjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return Columns::MAX;
}

void ProjectModel::setProjectPath(const QString &rootPath)
{
    auto projectRootItem = std::unique_ptr<ProjectItem>(new ProjectItem(rootPath));
    if (!projectRootItem->getInfo().exists())
    {
        return;
    }
    rootItem = std::move(projectRootItem);
    scanItem(rootItem.get());
}

void ProjectModel::scanItem(ProjectItem *item)
{
    if (!item)
        return;

    const QDir &itemDir = item->getPath();
    const QFileInfoList &dirInfoList = itemDir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(dirInfo.absoluteFilePath(), item));
        scanItem(child.get());
        item->appendChild(std::move(child));
    }

    const QFileInfoList pdfInfoList = itemDir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(pdfInfo.absoluteFilePath(), item));
        item->appendChild(std::move(child));
    }
}

QVariant ProjectModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid())
        return {};

    const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
    return item->data(index.column(), role);
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    return index.isValid() ? QAbstractItemModel::flags(index)
                           : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant ProjectModel::headerData(const int section, Qt::Orientation orientation, const int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole ? rootItem->data(section, role)
                                                                    : QVariant{};
}
