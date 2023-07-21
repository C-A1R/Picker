#include "ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent) : QFileSystemModel(parent)
{
    setNameFilterDisables(false);
    setNameFilters(QStringList() << "*.pdf");
    connect(this, &ProjectModel::signal_itemChecked, this, &ProjectModel::slot_onItemChecked);
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

void ProjectModel::slot_onItemChecked(const QModelIndex &index)
{
    Qt::CheckState state = checkedItems.value(index.internalId(), Qt::Unchecked);
    if (state == Qt::Checked || state == Qt::Unchecked)
    {
        for (int i = 0; i < rowCount(index); ++i)
        {
            const QModelIndex &child = this->index(i, 0, index);
            if (checkedItems.value(child.internalId(), Qt::Unchecked) != state)
            {
                setData(child, state, Qt::CheckStateRole);
            }
        }
    }
    int ch = 0;
    bool part = false;
    const QModelIndex &parent = index.parent();
    for (int i = 0; i < rowCount(parent); i++)
    {
        if (checkedItems.value(this->index(i, 0, parent).internalId(), Qt::Unchecked) != Qt::Unchecked)
        {
            ++ch;
        }
        if (checkedItems.value(this->index(i, 0, parent).internalId(), Qt::Unchecked) == Qt::PartiallyChecked)
        {
            part = true;
        }
    }

    if (part || (ch > 0 && ch < rowCount(parent)))
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
    else if (ch == rowCount(parent))
    {
        if (checkedItems.value(parent.internalId(), Qt::Unchecked) != Qt::Checked)
        {
            setData(parent, Qt::Checked, Qt::CheckStateRole);
        }
    }
    emit dataChanged(index, index);
}

bool ProjectModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.flags() & Qt::ItemNeverHasChildren)
    {
        return false;
    }
    return QDir(filePath(parent)).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() != 0;
}
