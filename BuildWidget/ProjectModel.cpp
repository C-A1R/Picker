#include "ProjectModel.h"
#include "SqlMgr.h"

#include <QSqlRecord>

ProjectModel::ProjectModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_unique<ProjectItem>(0, ""))
{
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::ItemFlag::NoItemFlags;
    }
    return QAbstractItemModel::flags(index)
           | Qt::ItemIsUserCheckable
           | Qt::ItemIsDragEnabled
           | Qt::ItemIsDropEnabled;
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
    if (parent.isValid() && parent.column() != 0)
        return 0;

    const ProjectItem *parentItem = parent.isValid() ? static_cast<const ProjectItem*>(parent.internalPointer())
                                                     : rootItem.get();
    int count = parentItem->childCount();
    return count;
}

int ProjectModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return Columns::MAX;
}

bool ProjectModel::hasChildren(const QModelIndex &parent) const
{
    ProjectItem *parentItem = parent.isValid() ? static_cast<ProjectItem*>(parent.internalPointer())
                                               : rootItem.get();
    return parentItem->childCount() > 0;
}

/// установить директорию проекта
bool ProjectModel::setProjectPath(const QString &rootPath)
{
    auto projectRootItem = std::unique_ptr<ProjectItem>(new ProjectItem(0, rootPath));
    if (!projectRootItem->exists())
    {
        return false;
    }
    rootItem = std::move(projectRootItem);
    return true;
}

/// загружаем элементы проекта из файла или из файловой системы
void ProjectModel::loadProjectItems()
{
    cleanup();
    if (!readFromFile())
    {
        double beginOrderIndex = rootItem->getOrderIndex();
        scanItem(rootItem.get(), beginOrderIndex);
    }
}

QString ProjectModel::projectDbFilePath() const
{
    return rootItem->getPath().absolutePath() + QDir::separator() + "picker.sqlite";
}

const ProjectItem *ProjectModel::getRootItem() const
{
    return rootItem.get();
}

/// читаем порядок из файла
bool ProjectModel::readFromFile()
{
    return false;
    // const QString dbFilename = projectDbFilePath();
    // if (!QFile::exists(dbFilename))
    // {
    //     return false;
    // }

    // SqlMgr sqlMgr(dbFilename);
    // if (!sqlMgr.open())
    // {
    //     qDebug("Can`t read primary order: file is busy");
    //     return false;
    // }

    // QList<QSqlRecord> recs;
    // if (!sqlMgr.readProjectElements(recs))
    // {
    //     qDebug("Can`t read primary order");
    // }
    // if (recs.empty())
    // {
    //     return true;
    // }

    // QModelIndexList expanded;
    // for (const QSqlRecord &rec : std::as_const(recs))
    // {
    //     const QString path = rec.value(SqlMgr::ProjectFilesystemTable::Columns::path).toString();
    //     const int printCheckState = rec.value(SqlMgr::ProjectFilesystemTable::Columns::printCheckstate).toInt();
    //     const int resultHolder = rec.value(SqlMgr::ProjectFilesystemTable::Columns::resultHolder).toInt();
    //     if (path.isEmpty())
    //     {
    //         continue;
    //     }
    //     const QModelIndex index = this->index(path);
    //     if (!index.isValid())
    //     {
    //         //файл из списка был удален
    //         qDebug() << "listed element was removed:" << path;
    //         continue;
    //     }
    //     // orders.emplace_back(index.internalId());
    //     checkedItems[index.internalId()] = printCheckState == 0 ? Qt::Unchecked : (printCheckState == 1 ? Qt::PartiallyChecked : Qt::Checked);
    //     resultHolders[index.siblingAtColumn(Columns::col_ResultHolder).internalId()] = resultHolder == 0 ? Qt::Unchecked : Qt::Checked;
    //     setData(index, Statuses::LISTED, ProjectItem::ItemRoles::StatusRole);
    //     // pathsById.emplace(index.internalId(), this->filePath(index));
    //     if (rec.value(SqlMgr::ProjectFilesystemTable::Columns::expanded).toBool())
    //     {
    //         expanded << index;
    //     }
    // }
    // emit signal_expand(expanded);

    // QModelIndexList additionItems;
    // scanFilesystem(rootDirectory(), additionItems);
    // for (const QModelIndex &ind : std::as_const(additionItems))
    // {
    //     checkItem(ind);//перепростановка галочек
    // }
    return true;
}

/// читаем файловую систему, ищем файлы, которых нет в списке
void ProjectModel::scanFilesystem(const QDir &dir, QModelIndexList &additionItems)
{
    // const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
    // if (hiddenIndices.contains(index.internalId()))
    // {
    //     return;
    // }

    // const QFileInfoList &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    // if (dir != this->rootDirectory())
    // {
    //     const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
    //     if (!orders.contains(index.internalId()))
    //     {
    //         additionItems.emplace_back(index);
    //         orders.emplace_back(index.internalId());
    //         setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
    //     }
    // }
    // for (const QFileInfo &dirInfo : dirInfoList)
    // {
    //     const QModelIndex &index = this->index(dirInfo.absoluteFilePath(), Columns::col_Name);
    //     if (hiddenIndices.contains(index.internalId()))
    //     {
    //         continue;
    //     }
    //     scanFilesystem(QDir(dirInfo.absoluteFilePath()), additionItems);
    // }
    // const QFileInfoList pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    // for (const QFileInfo &pdfInfo : pdfInfoList)
    // {
    //     const QModelIndex &index = this->index(pdfInfo.absoluteFilePath(), Columns::col_Name);
    //     if (!orders.contains(index.internalId()))
    //     {
    //         additionItems.emplace_back(index);
    //         orders.emplace_back(index.internalId());
    //         setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
    //         pathsById.emplace(index.internalId(), this->filePath(index));
    //     }
    // }
}



/// сканирует директорию и добавляет ей потомков, если:
/// * потомок - .pdf файл
/// * потомок - диретория, которая содержит .pdf файлы
bool ProjectModel::scanItem(ProjectItem *item, double &orderIndex)
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
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(++idMax, dirInfo.absoluteFilePath(), item));
        child->setOrderIndex(++orderIndex);
        if (!scanItem(child.get(), orderIndex))
            continue;
        item->appendChild(std::move(child));
        foundPdf = true;
    }

    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        auto child = std::unique_ptr<ProjectItem>(new ProjectItem(++idMax, pdfInfo.absoluteFilePath(), item));
        child->setOrderIndex(++orderIndex);
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
    idMax = 0;
    checkedItems.clear();
    resultHolders.clear();
    // pathsById.clear();
}

void ProjectModel::resetResultHolderCheckstates_Up(const QModelIndex &index)
{
    const QModelIndex &parent = index.parent();
    if (!parent.isValid())
    {
        return;
    }
    const QModelIndex sibling = parent.siblingAtColumn(col_ResultHolder);
    setData(sibling, Qt::Unchecked, Qt::CheckStateRole);
    emit dataChanged(sibling, sibling);
    resetResultHolderCheckstates_Up(sibling);
}

void ProjectModel::resetResultHolderCheckstates_Down(const QModelIndex &index)
{
    auto name_ind = index.siblingAtColumn(Columns::col_Name);
    for (int i = 0; i < rowCount(name_ind); ++i)
    {
        const QModelIndex &name_child = this->index(i, Columns::col_Name, name_ind);
        const QModelIndex &ch = name_child.siblingAtColumn(Columns::col_ResultHolder);
        setData(ch, Qt::Unchecked, Qt::CheckStateRole);
        emit dataChanged(ch, ch);
        resetResultHolderCheckstates_Down(ch);
    }
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
            if (item->isDir())
                return iconProvider.icon(QFileIconProvider::Folder);
            else
                return iconProvider.icon(QFileIconProvider::File);
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
            const auto item = static_cast<const ProjectItem*>(index.internalPointer());
            if (item->isDir())
                return QVariant(resultHolders.value(index.internalId(), Qt::Unchecked));
            break;
        }
        case Qt::DisplayRole:
        {
            return {};
        }
        default:
            break;
        }
    }

    return {};
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() == col_Name)
    {
        if (role == Qt::CheckStateRole)
        {
            checkedItems[index.internalId()] = static_cast<Qt::CheckState>(value.toInt());
            checkItem(index);
            return true;
        }
    }
    else if (index.column() == col_ResultHolder)
    {
        if (role == Qt::CheckStateRole)
        {
            const Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
            if (state == Qt::Checked)
            {
                resetResultHolderCheckstates_Up(index);
                resetResultHolderCheckstates_Down(index);
            }
            resultHolders[index.internalId()] = state;
            return true;
        }
    }
    return {};
}

void ProjectModel::slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
{
    if (selected.isEmpty())
        return;

    for (const QModelIndex &index : selected)
    {
        if (index.column() != Columns::col_Name)
            continue;
        setData(index, checkState, Qt::CheckStateRole);
    }
}

void ProjectModel::slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
{
    if (draggedIndices.isEmpty())
    {
        return;
    }

    auto droppedItem = static_cast<ProjectItem*>(droppedIndex.internalPointer());
    auto beforeDroppedItem = static_cast<ProjectItem*>(droppedIndex.siblingAtRow(droppedIndex.row() - 1).internalPointer());
    ProjectItem *parentItem = nullptr;
    double newOrderIndex = 0.0;
    double orderStep = 0.0;
    if (!droppedItem && !beforeDroppedItem)
    {
        const QModelIndex &parentIndex = draggedIndices.first().parent();
        parentItem = parentIndex.isValid() ? static_cast<ProjectItem*>(parentIndex.internalPointer())
                                           : rootItem.get();
        newOrderIndex = parentItem->child(parentItem->childCount() - 1)->getOrderIndex();
        orderStep = 1.0;
    }
    else
    {
        const QModelIndex &parentIndex = droppedIndex.parent();
        parentItem = parentIndex.isValid() ? static_cast<ProjectItem*>(parentIndex.internalPointer())
                                           : rootItem.get();
        newOrderIndex = beforeDroppedItem->getOrderIndex();
        orderStep = (droppedItem->getOrderIndex() - beforeDroppedItem->getOrderIndex()) / (draggedIndices.count() + 1);
    }
    newOrderIndex += orderStep;

    emit layoutAboutToBeChanged();
    for (const QModelIndex &index : draggedIndices)
    {
        if (index.column() != Columns::col_Name)
            continue;
        auto item = static_cast<ProjectItem*>(index.internalPointer());
        if (!item)
            continue;
        item->setOrderIndex(newOrderIndex);
        newOrderIndex += orderStep;
        qDebug() << "dragged:" << item->getPath().dirName() << "new_order:"  << item->getOrderIndex();
    }

    if (parentItem)
    {
        parentItem->sortChildren(); // sort parent item children by order index;
    }
    emit layoutChanged();
}
