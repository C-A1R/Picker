#include "ProjectModel.h"
#include "SqlMgr.h"

#include <QSqlRecord>

ProjectModel::ProjectModel(QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_shared<ProjectItem>(0, ""))
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

    const ProjectItem *parentItem = parent.isValid() ? static_cast<ProjectItem*>(parent.internalPointer())
                                                     : rootItem.get();
    if (const std::shared_ptr<const ProjectItem> &childItem = parentItem->child(row))
        return createIndex(row, column, childItem.get());
    return {};
}

QModelIndex ProjectModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<const ProjectItem*>(index.internalPointer());
    std::shared_ptr<const ProjectItem> parentItem = childItem->parentItem();
    if (!parentItem)
        return {};
    return parentItem != rootItem ? createIndex(parentItem->row(), 0, parentItem.get())
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
    auto parentItem = parent.isValid() ? static_cast<const ProjectItem*>(parent.internalPointer())
                                       : rootItem.get();
    return parentItem->childCount() > 0;
}

/// установить директорию проекта
bool ProjectModel::setProjectPath(const QString &rootPath)
{
    auto projectRootItem = std::make_shared<ProjectItem>(0, rootPath);
    if (!projectRootItem->exists())
    {
        return false;
    }
    rootItem = projectRootItem;
    return true;
}

/// загружаем элементы проекта из файла или из файловой системы
void ProjectModel::loadProjectItems()
{
    cleanup();
    if (!readFromDb())
    {
        double beginOrderIndex = rootItem->getOrderIndex();
        scanFilesystemItem(rootItem, beginOrderIndex);
        for (int i = 0; i < rootItem->childCount(); ++i)
        {
            setData(index(i, 0), Qt::Checked, Qt::CheckStateRole);
        }
    }
}

QString ProjectModel::projectDbFilePath() const
{
    return rootItem->getPath().absolutePath() + QDir::separator() + "picker.sqlite";
}

std::shared_ptr<const ProjectItem> ProjectModel::getRootItem() const
{
    return rootItem;
}

QStringList ProjectModel::getCheckedPdfPaths() const
{
    QStringList checked;
    getCheckedPdf(rootItem, checked);
    return checked;
}

QStringList ProjectModel::getResultHolderPaths() const
{
    QStringList checked;
    getResultHolders(rootItem, checked);
    return checked;
}

void ProjectModel::getCheckedPdf(const std::shared_ptr<const ProjectItem> &item, QStringList &result) const
{
    for (int i = 0; i < item->childCount(); ++i)
    {
        const std::shared_ptr<const ProjectItem> &child = item->child(i);
        if (!child)
            continue;

        const Qt::CheckState state = checkedItems.value(child->getId(), Qt::Unchecked);
        if (state == Qt::Unchecked)
            continue;

        if (child->isDir())
        {
            getCheckedPdf(child, result);
            continue;
        }

        if (state == Qt::Checked)
            result.append(child->getPath().absolutePath());
    }
}

void ProjectModel::getResultHolders(const std::shared_ptr<const ProjectItem> &item, QStringList &result) const
{
    for (int i = 0; i < item->childCount(); ++i)
    {
        const std::shared_ptr<const ProjectItem> &child = item->child(i);
        if (!child)
            continue;

        if (!child->isDir())
            continue;

        const Qt::CheckState state = resultHolders.value(child->getId(), Qt::Unchecked);
        if (state == Qt::Checked)
        {
            result.append(child->getPath().absolutePath());
            continue;
        }
        getResultHolders(child, result);
    }
}

///добавлять элемент в модель только этим методом
void ProjectModel::insertItem(const std::shared_ptr<ProjectItem> &item, std::shared_ptr<ProjectItem> parentItem)
{
    if (!parentItem)
    {
        parentItem = rootItem;
    }
    item->setParent(parentItem);
    parentItem->appendChild(item);
    itemPaths.insert(item->getPath().absolutePath(), item);
}

std::shared_ptr<ProjectItem> ProjectModel::findItem(const QModelIndex &index)
{
    return index.isValid() ? itemPaths.value(index.data(ProjectItem::Roles::ABS_PATH).toString(), nullptr) : rootItem;
}

/// читаем порядок из файла
bool ProjectModel::readFromDb()
{
    const QString dbFilename = projectDbFilePath();
    if (!QFile::exists(dbFilename))
    {
        return false;
    }

    SqlMgr sqlMgr(dbFilename);
    if (!sqlMgr.open())
    {
        qDebug("Can`t read primary order: file is busy");
        return false;
    }

    QList<QSqlRecord> recs;
    if (!sqlMgr.readProjectElements(recs))
    {
        qDebug("Can`t read primary order");
    }
    if (recs.empty())
    {
        return true;
    }

    QHash<qulonglong, std::shared_ptr<ProjectItem>> itemsById;
    itemsById.reserve(recs.size());
    itemsById.insert(rootItem->getId(), rootItem);
    double orderIndex = rootItem->getOrderIndex();

    QModelIndexList expanded;
    for (const QSqlRecord &rec : std::as_const(recs))
    {
        const QString path = rec.value(SqlMgr::ProjectFilesystemTable::Columns::path).toString();
        if (path.isEmpty())
        {
            continue;
        }

        const qulonglong id = rec.value(SqlMgr::ProjectFilesystemTable::Columns::id).toULongLong();

        auto parentItem = itemsById.value(rec.value(SqlMgr::ProjectFilesystemTable::Columns::parentId).toULongLong(), rootItem);
        auto item = std::make_shared<ProjectItem>(id, path, parentItem);
        if (!item->exists())
        {
            //файл из списка был удален
            qDebug() << "Item was removed:" << path;
            continue;
        }
        item->setOrderIndex(++orderIndex);//перенумерация порядка, чтобы отбросить дробную часть
        insertItem(item, parentItem);
        itemsById.insert(item->getId(), item);

        const int printCheckState = rec.value(SqlMgr::ProjectFilesystemTable::Columns::printCheckstate).toInt();
        const int resultHolder = rec.value(SqlMgr::ProjectFilesystemTable::Columns::resultHolder).toInt();
        checkedItems[id] = printCheckState == 0 ? Qt::Unchecked : (printCheckState == 1 ? Qt::PartiallyChecked : Qt::Checked);
        resultHolders[id] = resultHolder == 0 ? Qt::Unchecked : Qt::Checked;
        if (!item->isDir())
            itemStatuses[id] = Statuses::LISTED;

        if (rec.value(SqlMgr::ProjectFilesystemTable::Columns::expanded).toBool())
        {
            QModelIndex index = createIndex(parentItem->childCount() - 1, Columns::col_Name, item.get());
            expanded.emplaceBack(std::move(index));
        }

        idMax = idMax < id ? id : idMax;
    }
    emit signal_expand(expanded);

    // поиск файлов, отсутствующих в базе
    scanFilesystemItem(rootItem, orderIndex);
    return true;
}

/// сканирует директорию и добавляет ей потомков, если:
/// * потомок - .pdf файл
/// * потомок - диретория, которая содержит .pdf файлы
bool ProjectModel::scanFilesystemItem(const std::shared_ptr<ProjectItem> &item, double &orderIndex)
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
        const QString &childPath = dirInfo.absoluteFilePath();
        const qulonglong id = idMax + 1;
        auto child = std::make_shared<ProjectItem>(id, childPath, item);
        child->setOrderIndex(++orderIndex);
        if (!scanFilesystemItem(child, orderIndex))
            continue;
        if (!itemPaths.contains(childPath))
        {
            insertItem(child, item);
            idMax = id;
        }
        foundPdf = true;
    }

    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        const QString &childPath = pdfInfo.absoluteFilePath();
        const qulonglong id = idMax + 1;
        auto child = std::make_shared<ProjectItem>(id, childPath, item);
        child->setOrderIndex(++orderIndex);
        if (!itemPaths.contains(childPath))
        {
            insertItem(child, item);
            itemStatuses[child->getId()] = Statuses::NOT_LISTED;
            idMax = id;
        }
    }

    return hasPdf || foundPdf;
}

void ProjectModel::checkItem(const QModelIndex &index)
{
    auto itemId = [this](const QModelIndex &index) -> qulonglong
    {
        return data(index, ProjectItem::Roles::ID).toULongLong();
    };

    Qt::CheckState state = checkedItems.value(itemId(index), Qt::Unchecked);
    if (state == Qt::Checked || state == Qt::Unchecked)
    {
        for (int i = 0; i < rowCount(index); ++i)
        {
            const QModelIndex &child = this->index(i, Columns::col_Name, index);
            if (checkedItems.value(itemId(child), Qt::Unchecked) != state)
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
        if (checkedItems.value(itemId(child), Qt::Unchecked) != Qt::Unchecked)
        {
            ++ch;
        }
        if (checkedItems.value(itemId(child), Qt::Unchecked) == Qt::PartiallyChecked)
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
        if (checkedItems.value(itemId(parent), Qt::Unchecked) != Qt::Unchecked)
        {
            setData(parent, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
    else if (ch == visible)
    {
        if (checkedItems.value(itemId(parent), Qt::Unchecked) != Qt::Checked)
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
    itemPaths.clear();
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
            return checkedItems.value(data(index, ProjectItem::Roles::ID).toULongLong(), Qt::Unchecked);
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
        case Qt::BackgroundRole:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            const Statuses status = itemStatuses.value(item->getId(), Statuses::DEFAULT);
            if (status == Statuses::NOT_LISTED && checkedItems.value(item->getId()) != Qt::Checked)
            {
                return QVariant{};
            }
            return statusColors[status];
        }
        case ProjectItem::Roles::ID:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            return item->getId();
        }
        case ProjectItem::Roles::STATUS:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            return itemStatuses.value(item->getId(), Statuses::DEFAULT);
        }
        case ProjectItem::Roles::ABS_PATH:
        {
            const auto *item = static_cast<const ProjectItem*>(index.internalPointer());
            return item->getPath().absolutePath();
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
                return resultHolders.value(item->getId(), Qt::Unchecked);
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
            checkedItems[data(index, ProjectItem::Roles::ID).toULongLong()] = static_cast<Qt::CheckState>(value.toInt());
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
            auto item = static_cast<const ProjectItem*>(index.internalPointer());
            resultHolders[item->getId()] = state;
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

std::tuple<double, double> ProjectModel::newOrder(const std::shared_ptr<const ProjectItem> parentItem, const QModelIndex &droppedIndex, const int draggedCount)
{
    if (!parentItem || !draggedCount)
        return std::make_tuple(0.0, 0.0);

    auto droppedItem = static_cast<ProjectItem*>(droppedIndex.internalPointer());
    auto beforeDroppedItem = static_cast<ProjectItem*>(droppedIndex.siblingAtRow(droppedIndex.row() - 1).internalPointer());
    double newOrder = 0.0;
    double orderStep = 0.0;
    if (!droppedItem && !beforeDroppedItem)
    {
        //переместили в конец
        newOrder = parentItem->child(parentItem->childCount() - 1)->getOrderIndex();
        orderStep = 1.0;
    }
    else
    {
        if (!droppedItem)
            return std::make_tuple(0.0, 0.0);

        newOrder = beforeDroppedItem ? beforeDroppedItem->getOrderIndex()
                                     : parentItem->getOrderIndex();
        orderStep = (droppedItem->getOrderIndex() - newOrder) / (draggedCount + 1);
    }
    newOrder += orderStep;

    return std::make_tuple(newOrder, orderStep);
}

void ProjectModel::slot_dropped(const QModelIndex &dropRootIndex,const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
{
    if (draggedIndices.isEmpty())
        return;

    const std::shared_ptr<ProjectItem> parentItem = findItem(dropRootIndex);
    if (!parentItem)
        return;

    auto no = newOrder(parentItem, droppedIndex, draggedIndices.count());
    double newOrder = 0.0;
    double orderStep = 0.0;
    std::tie(newOrder, orderStep) = no;
    if (newOrder == 0.0 && orderStep == 0.0)
        return;

    for (const QModelIndex &index : draggedIndices)
    {
        if (index.column() != Columns::col_Name)
            continue;
        const std::shared_ptr<ProjectItem> item = findItem(index);
        if (!item)
            continue;
        item->setOrderIndex(newOrder);
        newOrder += orderStep;
        if (parentItem != item->parentItem())
        {
            item->parentItem()->removeChild(item->getId());
            insertItem(item, parentItem);
        }
        qDebug() << "dragged:" << item->getPath().dirName() << "new_order:"  << item->getOrderIndex();
    }

    emit layoutAboutToBeChanged();
    parentItem->sortChildren(); // sort parent item children by order index;
    emit layoutChanged();
}

void ProjectModel::slot_added(const QModelIndex &dropRootIndex, const QModelIndex droppedIndex, const QString &fullPaths)
{
    if (fullPaths.isEmpty())
        return;
    qDebug() << "slot_added:" << fullPaths;

    const std::shared_ptr<ProjectItem> parentItem = findItem(dropRootIndex);
    if (!parentItem)
        return;

    const QStringList paths = fullPaths.split('*');

    auto no = newOrder(parentItem, droppedIndex, paths.count());
    double newOrder = 0.0;
    double orderStep = 0.0;
    std::tie(newOrder, orderStep) = no;
    if (newOrder == 0.0 && orderStep == 0.0)
        return;

    for (const QString &path : paths)
    {
        auto newItem = std::make_shared<ProjectItem>(++idMax, path, parentItem);
        if (!newItem->exists())
            continue;
        if (newItem->isDir() || !newItem->getPath().dirName().endsWith(".pdf", Qt::CaseInsensitive))
            continue;
        if (itemPaths.contains(newItem->getPath().absolutePath()))
            continue;

        newItem->setOrderIndex(newOrder);
        newOrder += orderStep;
        insertItem(newItem, parentItem);
    }

    emit layoutAboutToBeChanged();
    parentItem->sortChildren(); // sort parent item children by order index;
    emit layoutChanged();
}
