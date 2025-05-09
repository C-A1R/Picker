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

/// установить и просканировать диреторию проекта
bool ProjectModel::setProjectPath(const QString &rootPath)
{
    auto projectRootItem = std::unique_ptr<ProjectItem>(new ProjectItem(rootPath));
    if (!projectRootItem->getInfo().exists())
    {
        return false;
    }
    cleanup();
    rootItem = std::move(projectRootItem);
    scanItem(rootItem.get());
    return true;
}

/// сканирует директорию и добавляет ей потомков, если:
/// * потомок - .pdf файл
/// * потомок - диретория, которая содержит .pdf файлы
bool ProjectModel::scanItem(ProjectItem *item)
{
    if (!item)
        return false;

    const QDir &itemDir = item->getPath();
    const QFileInfoList &dirInfoList = itemDir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    const QFileInfoList &pdfInfoList = itemDir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    const bool hasPdf = !pdfInfoList.isEmpty();

    bool foundPdf = false;
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(dirInfo.absoluteFilePath(), item));
        if (!scanItem(child.get()))
            continue;

        item->appendChild(std::move(child));
        foundPdf = true;
    }

    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(pdfInfo.absoluteFilePath(), item));
        item->appendChild(std::move(child));
    }

    return hasPdf || foundPdf;
}

void ProjectModel::checkItem(const QModelIndex &index)
{
    Qt::CheckState state = checkedItems.value(index.internalId(), Qt::Unchecked);
    if (state == Qt::Checked || state == Qt::Unchecked)
    {
        for (int i = 0; i < rowCount(index); ++i)
        {
            const QModelIndex &child = this->index(i, Columns::col_Name, index);
            if (checkedItems.value(child.internalId(), Qt::Unchecked) != state)
            {
                setData(child, state, Qt::CheckStateRole);
            }
        }
    }
    int ch = 0;
    int visible = 0;
    bool part = false;
    const QModelIndex &parent = index.parent();
    if (!parent.isValid())
    {
        emit dataChanged(index, index);
        return;
    }
    for (int i = 0; i < rowCount(parent); i++)
    {
        const QModelIndex &child = this->index(i, Columns::col_Name, parent);
        ++visible;
        if (checkedItems.value(child.internalId(), Qt::Unchecked) != Qt::Unchecked)
        {
            ++ch;
        }
        if (checkedItems.value(child.internalId(), Qt::Unchecked) == Qt::PartiallyChecked)
        {
            part = true;
        }
    }

    if (part || (ch > 0 && ch < visible))
    {
        setData(parent, Qt::PartiallyChecked, Qt::CheckStateRole);
    }
    else if (ch == 0)
    {
        if (checkedItems.value(parent.internalId(), Qt::Unchecked) != Qt::Unchecked)
        {
            setData(parent, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
    else if (ch == visible)
    {
        if (checkedItems.value(parent.internalId(), Qt::Unchecked) != Qt::Checked)
        {
            setData(parent, Qt::Checked, Qt::CheckStateRole);
        }
    }
    emit dataChanged(index, index);
}

void ProjectModel::cleanup()
{
    // resultHolderCheckstates.clear();
    checkedItems.clear();
    // hiddenIndices.clear();
    // orders.clear();
    // pathsById.clear();
}

QVariant ProjectModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid())
        return {};

    if (index.column() == Columns::col_Name)
    {
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            return QVariant(checkedItems.value(index.internalId(), Qt::Unchecked));
        }
        case Qt::DecorationRole:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            if (item->getInfo().isDir())
                return iconProvider.icon(QFileIconProvider::Folder);
            else
                return iconProvider.icon(QFileIconProvider::File);
            break;
        }
        case Qt::DisplayRole:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            return item->getPath().dirName();
        }
        default:
            break;
        }
    }
    else if (index.column() == col_ResultHolder)
    {
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            // return QVariant(resultHolderCheckstates.value(index.internalId(), Qt::Unchecked));
        }
        default:
            break;
        }
    }

    return {};
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == col_Name)
    {
        checkedItems[index.internalId()] = static_cast<Qt::CheckState>(value.toInt());
        checkItem(index);
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    return index.isValid() ? QAbstractItemModel::flags(index)
                           : Qt::ItemFlags(Qt::NoItemFlags);
}

void ProjectModel::slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
{
    if (selected.isEmpty()) {
        return;
    }
    for (const QModelIndex &index : selected) {
        if (index.column() != Columns::col_Name) {
            continue;
        }
        setData(index, checkState, Qt::CheckStateRole);
    }
}
