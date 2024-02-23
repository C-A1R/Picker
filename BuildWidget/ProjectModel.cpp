#include "ProjectModel.h"

#include "SqlMgr.h"

#include <QMimeData>
#include <QSqlRecord>

ProjectModel::ProjectModel(QObject *parent)
    : QFileSystemModel(parent)
{
    setNameFilterDisables(false);
    setNameFilters(QStringList{"*.pdf"});
    connect(this, &ProjectModel::rootPathChanged, this, &ProjectModel::slot_onRootPathChanged);
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::ItemFlag::NoItemFlags;
    }
    if (index.column() == Columns::col_ResultHolder && !isDir(index))
    {
        return QAbstractItemModel::flags(index) ^ Qt::ItemIsEnabled;
    }
    return QAbstractItemModel::flags(index)
           | Qt::ItemIsUserCheckable
           | Qt::ItemIsDragEnabled
           | Qt::ItemIsDropEnabled;
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::CheckStateRole:
    {
        if (index.column() == Columns::col_Name)
            return QVariant(checkedItems.value(index.internalId(), Qt::Unchecked));
        if (index.column() == Columns::col_ResultHolder && isDir(index))
            return QVariant(resultHolderCheckstates.value(index.internalId(), Qt::Unchecked));
        break;
    }
    case Qt::DisplayRole:
    {
        if (index.column() == Columns::col_ResultHolder) return QVariant();
        break;
    }
    case Qt::BackgroundRole:
    {
        switch (data(index, ProjectItemRoles::StatusRole).toInt())
        {
        case Statuses::NOT_LISTED:  return checkedItems.value(index.internalId()) == Qt::Checked ? QColor(255, 237, 204, 200)
                                                                                                 : QColor(Qt::white);
        case Statuses::LISTED:      return QColor(100, 221, 23, 50);
        default:                    return QColor(Qt::white);
        }
        break;
    }
    case ProjectItemRoles::StatusRole:
    {
        return QVariant(itemStatuses.value(index.internalId(), Statuses::DEFAULT));
    }
    default: break;
    }

    return QFileSystemModel::data(index, role);
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == col_Name)
    {
        checkedItems[index.internalId()] = static_cast<Qt::CheckState>(value.toInt());
        checkItem(index);
        return true;
    }
    if (role == Qt::CheckStateRole && index.column() == col_ResultHolder)
    {
        const Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        if (state == Qt::Checked)
        {
            resetResultHolderCheckstates_Up(index);
            resetResultHolderCheckstates_Down(index);
        }
        resultHolderCheckstates[index.internalId()] = state;
        return true;
    }
    if (role == ProjectItemRoles::StatusRole)
    {
        itemStatuses[index.internalId()] = static_cast<Statuses>(value.toInt());
        return true;
    }
    return QFileSystemModel::setData(index, value, role);
}

Qt::DropActions ProjectModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

// bool ProjectModel::insertRows(int row, int count, const QModelIndex &parent)
// {
//     /// @todo
//     return true;
// }

int ProjectModel::columnCount(const QModelIndex &/*parent*/) const
{
    return Columns::col_Max;
}

const QSet<quintptr> &ProjectModel::getHiddenIndices() const
{
    return hiddenIndices;
}

const QList<quintptr> &ProjectModel::getOrders() const
{
    return orders;
}

QStringList ProjectModel::getResultHolders() const
{
    QStringList result;
    QString tmp;
    for (const quintptr indexId : orders)
    {
        if (resultHolderCheckstates.value(indexId, Qt::Unchecked) == Qt::Unchecked)
        {
            continue;
        }
        tmp = pathsById.value(indexId, QString());
        if (!tmp.isEmpty())
        {
            result.emplace_back(std::move(tmp));
        }
    }
    return result;
}

const QStringList ProjectModel::getCheckedPdfPaths() const
{
    QStringList result;
    QString tmp;
    for (const quintptr indexId : orders)
    {
        if (checkedItems.value(indexId, Qt::Unchecked) == Qt::Unchecked)
        {
            continue;
        }
        tmp = pathsById.value(indexId, QString());
        if (!tmp.isEmpty())
        {
            result.emplace_back(std::move(tmp));
        }
    }
    return result;
}

/// имя файла для сохранения списка (порядка сортировки)
QString ProjectModel::listFilePath() const
{
    return rootDirectory().absolutePath() + QDir::separator() + "picker.sqlite";
}

[[maybe_unused]] bool ProjectModel::scanForHiddenItems(const QDir &dir)
{
    const auto &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    const auto &pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files);
    const bool hasPdf = !pdfInfoList.isEmpty();
    if (dirInfoList.isEmpty())
    {
        return hasPdf;
    }

    bool foundPdf = false;
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        if (const QString &path{dirInfo.absoluteFilePath()}; !scanForHiddenItems(QDir(path)))
        {
            const QModelIndex &index = this->index(path, Columns::col_Name);
            if (index.isValid())
            {
                hiddenIndices << index.internalId();
            }
        }
        else
        {
            foundPdf = true;
        }
    }
    return hasPdf || foundPdf;
}

void ProjectModel::scanDefaultOrder(const QDir &dir)
{
    const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
    if (hiddenIndices.contains(index.internalId()))
    {
        return;
    }

    const QFileInfoList &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dir != this->rootDirectory())
    {
        const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
        orders.emplace_back(index.internalId());
        checkedItems[index.internalId()] = Qt::Checked;
        setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
        pathsById.emplace(index.internalId(), this->filePath(index));
    }
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        const QModelIndex &index = this->index(dirInfo.absoluteFilePath(), Columns::col_Name);
        if (hiddenIndices.contains(index.internalId()))
        {
            continue;
        }
        scanDefaultOrder(QDir(dirInfo.absoluteFilePath()));
    }
    const QFileInfoList pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        const QModelIndex &index = this->index(pdfInfo.absoluteFilePath(), Columns::col_Name);
        orders.emplace_back(index.internalId());
        checkedItems[index.internalId()] = Qt::Checked;
        setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
        pathsById.emplace(index.internalId(), this->filePath(index));
    }
}

/// читаем порядок из файла
bool ProjectModel::readOrderFromListFile()
{
    const QString dbFilename = listFilePath();
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

    QModelIndexList expanded;
    for (const QSqlRecord &rec : std::as_const(recs))
    {
        const QString path = rec.value(SqlMgr::ProjectFilesystemTable::Columns::path).toString();
        const int printCheckState = rec.value(SqlMgr::ProjectFilesystemTable::Columns::printCheckstate).toInt();
        const int resultHolder = rec.value(SqlMgr::ProjectFilesystemTable::Columns::resultHolder).toInt();
        if (path.isEmpty())
        {
            continue;
        }
        const QModelIndex index = this->index(path);
        if (!index.isValid())
        {
            //файл из списка был удален
            qDebug() << "listed element was removed:" << path;
            continue;
        }
        orders.emplace_back(index.internalId());
        checkedItems[index.internalId()] = printCheckState == 0 ? Qt::Unchecked : (printCheckState == 1 ? Qt::PartiallyChecked : Qt::Checked);
        resultHolderCheckstates[index.siblingAtColumn(Columns::col_ResultHolder).internalId()] = resultHolder == 0 ? Qt::Unchecked : Qt::Checked;
        setData(index, Statuses::LISTED, ProjectItemRoles::StatusRole);
        pathsById.emplace(index.internalId(), this->filePath(index));
        if (rec.value(SqlMgr::ProjectFilesystemTable::Columns::expanded).toBool())
        {
            expanded << index;
        }
    }
    emit signal_expand(expanded);

    QModelIndexList additionItems;
    scanFilesystem(rootDirectory(), additionItems);
    for (const QModelIndex &ind : std::as_const(additionItems))
    {
        checkItem(ind);//перепростановка галочек
    }
    return true;
}

/// читаем файловую систему, ищем файлы, которых нет в списке
void ProjectModel::scanFilesystem(const QDir &dir, QModelIndexList &additionItems)
{
    const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
    if (hiddenIndices.contains(index.internalId()))
    {
        return;
    }

    const QFileInfoList &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dir != this->rootDirectory())
    {
        const QModelIndex &index = this->index(dir.absolutePath(), Columns::col_Name);
        if (!orders.contains(index.internalId()))
        {
            additionItems.emplace_back(index);
            orders.emplace_back(index.internalId());
            setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
        }
    }
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        const QModelIndex &index = this->index(dirInfo.absoluteFilePath(), Columns::col_Name);
        if (hiddenIndices.contains(index.internalId()))
        {
            continue;
        }
        scanFilesystem(QDir(dirInfo.absoluteFilePath()), additionItems);
    }
    const QFileInfoList pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        const QModelIndex &index = this->index(pdfInfo.absoluteFilePath(), Columns::col_Name);
        if (!orders.contains(index.internalId()))
        {
            additionItems.emplace_back(index);
            orders.emplace_back(index.internalId());
            setData(index, Statuses::NOT_LISTED, ProjectItemRoles::StatusRole);
            pathsById.emplace(index.internalId(), this->filePath(index));
        }
    }
}

void ProjectModel::cleanup()
{
    resultHolderCheckstates.clear();
    checkedItems.clear();
    hiddenIndices.clear();
    orders.clear();
    pathsById.clear();
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

void ProjectModel::slot_dropped(const quintptr droppedIndexId, const QList<quintptr> &draggeddIndicesIds)
{
    if (draggeddIndicesIds.isEmpty())
    {
        return;
    }
    for (const quintptr id : draggeddIndicesIds)
    {
        orders.removeOne(id);
    }
    const auto i = droppedIndexId != 0 ? orders.indexOf(droppedIndexId) : orders.size()/*to the end*/;
    for (int j = draggeddIndicesIds.count() - 1; j >= 0; --j)
    {
        orders.insert(i, std::move(draggeddIndicesIds.at(j)));
    }
}

void ProjectModel::slot_added(const quintptr droppedIndexId, const QString &fullPaths)
{
    qDebug() << "slot_added:" << fullPaths;
    if (fullPaths.isEmpty())
    {
        return;
    }

    const QStringList paths = fullPaths.split('*');
    const auto i = droppedIndexId != 0 ? orders.indexOf(droppedIndexId) : orders.size()/*to the end*/;
    const QModelIndex parent = index(rootPath());
    for (int j = paths.count() - 1; j >= 0; --j)
    {
        const int row = rowCount(parent) - 1;
        qDebug() << "insert:" << insertRow(row, parent);
        const QModelIndex &inserted = index(row, Columns::col_Name, parent);
        orders.insert(i, inserted.internalId());
    }
}

void ProjectModel::slot_setChecked(const QModelIndexList &selected, const Qt::CheckState checkState)
{
    if (selected.isEmpty())
    {
        return;
    }
    for (const QModelIndex &index : selected)
    {
        if (index.column() != Columns::col_Name)
        {
            continue;
        }
        setData(index, checkState, Qt::CheckStateRole);
    }
}

void ProjectModel::checkItem(const QModelIndex &index)
{
    Qt::CheckState state = checkedItems.value(index.internalId(), Qt::Unchecked);
    if (state == Qt::Checked || state == Qt::Unchecked)
    {
        for (int i = 0; i < rowCount(index); ++i)
        {
            const QModelIndex &child = this->index(i, Columns::col_Name, index);
            if (hiddenIndices.contains(child.internalId()))
            {
                continue;
            }
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
    if (!parent.isValid() || this->filePath(parent) == rootPath())
    {
        emit dataChanged(index, index);
        return;
    }
    for (int i = 0; i < rowCount(parent); i++)
    {
        const QModelIndex &child = this->index(i, Columns::col_Name, parent);
        if (hiddenIndices.contains(child.internalId()))
        {
            continue;
        }
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

void ProjectModel::slot_onRootPathChanged()
{
    cleanup();
    scanForHiddenItems(rootDirectory());
    if (!readOrderFromListFile())
    {
        scanDefaultOrder(rootDirectory());
    }
}
