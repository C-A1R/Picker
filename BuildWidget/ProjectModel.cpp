#include "ProjectModel.h"

ProjectModel::ProjectModel(QObject *parent) : QFileSystemModel(parent)
{
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::ItemFlag::NoItemFlags;
    }
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index)|Qt::ItemIsUserCheckable;
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole)
    {
        return Qt::CheckState::Unchecked;
    }
    return QFileSystemModel::data(index, role);
}
