#include "ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent) : QFileSystemModel(parent)
{
    setNameFilterDisables(false);
    setNameFilters(QStringList() << "*.pdf");
    connect(this, &ProjectModel::signal_itemChecked, this, &ProjectModel::slot_onItemChecked);
    connect(this, &ProjectModel::rootPathChanged, this, &ProjectModel::slot_onRootPathChanged);
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::ItemFlag::NoItemFlags;
    }
    return QAbstractItemModel::flags(index)| Qt::ItemIsUserCheckable;
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

const QSet<qint64> &ProjectModel::getHiddenIndexes() const
{
    return hiddenIndexes;
}

void ProjectModel::scanPdfItems(const QDir &dir)
{
    if (bool hasPdf = !dir.isEmpty(QDir::Files); !hasPdf)//pdf name filter has already turned on
    {
        const QModelIndex &index = this->index(dir.absolutePath(), 0);
        if (index.isValid())
        {
            hiddenIndexes << index.internalId();
        }
    }
    const auto dirInfoList = dir.entryInfoList(QStringList(), QDir::NoDotAndDotDot | QDir::Dirs);
    if (dirInfoList.isEmpty())
    {
        return;
    }
    for (const QFileInfo &dirInfo : dirInfoList)
    {
        scanPdfItems(QDir(dirInfo.absoluteFilePath()));
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
    scanPdfItems(rootDirectory());
}
