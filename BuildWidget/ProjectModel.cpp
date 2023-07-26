#include "ProjectModel.h"

#include <QMimeData>

ProjectModel::ProjectModel(QObject *parent) : QFileSystemModel(parent)
{
    setNameFilterDisables(false);
    setNameFilters(QStringList{"*.pdf"});
    connect(this, &ProjectModel::signal_itemChecked, this, &ProjectModel::slot_onItemChecked);
    connect(this, &ProjectModel::rootPathChanged, this, &ProjectModel::slot_onRootPathChanged);
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

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        return QVariant(checkedItems.value(index.internalId(), Qt::Unchecked));
    }
    return QFileSystemModel::data(index, role);
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        checkedItems[index.internalId()] = static_cast<Qt::CheckState>(value.toInt());
        emit signal_itemChecked(index);
        return true;
    }
    return QFileSystemModel::setData(index, value, role);
}

Qt::DropActions ProjectModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

const QSet<quintptr> &ProjectModel::getHiddenIndexes() const
{
    return hiddenIndexes;
}

const QList<quintptr> &ProjectModel::getOrders() const
{
    return orders;
}

void ProjectModel::scanForHiddenItems(const QDir &dir)
{
    if (bool hasPdf = !dir.isEmpty(QDir::Files); !hasPdf)//pdf name filter has already turned on
    {
        const QModelIndex &index = this->index(dir.absolutePath(), 0);
        if (index.isValid())
        {
            hiddenIndexes << index.internalId();
        }
    }
    const auto &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dirInfoList.isEmpty())
    {
        return;
    }
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        scanForHiddenItems(QDir(dirInfo.absoluteFilePath()));
    }
}

void ProjectModel::scanOrder(const QDir &dir)
{
    const QFileInfoList dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dirInfoList.isEmpty())
    {
        return;
    }
    {
        const QModelIndex &index = this->index(dir.absolutePath(), 0);
        orders.emplace_back(index.internalId());
    }
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        const QModelIndex &index = this->index(dirInfo.absoluteFilePath(), 0);
        if (hiddenIndexes.contains(index.internalId()))
        {
            continue;
        }
        scanOrder(QDir(dirInfo.absoluteFilePath()));
    }
    const QFileInfoList pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files);
    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        const QModelIndex &index = this->index(pdfInfo.absoluteFilePath(), 0);
        orders.emplace_back(index.internalId());
    }
}

void ProjectModel::slot_dropped(const quintptr droppedIndexId, const QList<quintptr> draggeddIndicesIds)
{
    if (draggeddIndicesIds.isEmpty())
    {
        return;
    }
    for (const quintptr id : draggeddIndicesIds)
    {
        orders.removeOne(id);
    }
    auto i = droppedIndexId != 0 ? orders.indexOf(droppedIndexId) : orders.size()/*to the end*/;
    for (const quintptr id : draggeddIndicesIds)
    {
        orders.insert(i, std::move(id));
    }
}

void ProjectModel::slot_onItemChecked(const QModelIndex &index)
{
    Qt::CheckState state = checkedItems.value(index.internalId(), Qt::Unchecked);
    if (state == Qt::Checked || state == Qt::Unchecked)
    {
        for (int i = 0; i < rowCount(index); ++i)
        {
            const QModelIndex &child = this->index(i, 0, index);
            if (hiddenIndexes.contains(child.internalId()))
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
    for (int i = 0; i < rowCount(parent); i++)
    {
        const QModelIndex &child = this->index(i, 0, parent);
        if (hiddenIndexes.contains(child.internalId()))
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
    hiddenIndexes.clear();
    scanForHiddenItems(rootDirectory());
    orders.clear();
    scanOrder(rootDirectory());
}
