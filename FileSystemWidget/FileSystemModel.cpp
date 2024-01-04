#include "FileSystemModel.h"

FileSystemModel::FileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        if (selected.contains(index))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    return QFileSystemModel::data(index, role);
}

void FileSystemModel::slot_selectItem(const QModelIndex &index)
{
    if (!selected.removeOne(index))
    {
        selected.emplaceBack(index);
    }
}
