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
    if (role == Qt::BackgroundRole)
    {
        if (checkedItems.value(index.internalId()) == Qt::Checked)
        {
            return QColor(240, 178, 122);
        }
        return QColor(Qt::white);
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
        tmp = pdfPaths.value(indexId, QString());
        if (!tmp.isEmpty())
        {
            result.emplace_back(std::move(tmp));
        }
    }
    return result;
}

[[maybe_unused]] bool ProjectModel::scanForHiddenItems(const QDir &dir)
{
    const auto &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    const auto &pdfInfoList = dir.entryInfoList(QStringList() << "*.pdf", QDir::Files);
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
            const QModelIndex &index = this->index(path, 0);
            if (index.isValid())
            {
                hiddenIndexes << index.internalId();
            }
        }
        else
        {
            foundPdf = true;
        }
    }
    return hasPdf || foundPdf;
}

void ProjectModel::scanOrder(const QDir &dir)
{
    const QModelIndex &index = this->index(dir.absolutePath(), 0);
    if (hiddenIndexes.contains(index.internalId()))
    {
        return;
    }

    const QFileInfoList &dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dir != this->rootDirectory())
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
    const QFileInfoList pdfInfoList = dir.entryInfoList(QStringList{"*.pdf"}, QDir::Files, QDir::NoSort);
    for (const QFileInfo &pdfInfo : pdfInfoList)
    {
        const QModelIndex &index = this->index(pdfInfo.absoluteFilePath(), 0);
        orders.emplace_back(index.internalId());
        pdfPaths.emplace(index.internalId(), this->filePath(index));
    }
}

void ProjectModel::cleanup()
{
    checkedItems.clear();
    hiddenIndexes.clear();
    orders.clear();
    pdfPaths.clear();
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
    for (int j = draggeddIndicesIds.count() - 1; j >= 0; --j)
    {
        orders.insert(i, std::move(draggeddIndicesIds.at(j)));
    }
}

void ProjectModel::slot_setChecked(const QModelIndexList &selected, const bool checked)
{
    if (selected.isEmpty())
    {
        return;
    }
    for (const QModelIndex &index : selected)
    {
        setData(index, checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked, Qt::CheckStateRole);
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
    cleanup();
    scanForHiddenItems(rootDirectory());
    scanOrder(rootDirectory());
}
