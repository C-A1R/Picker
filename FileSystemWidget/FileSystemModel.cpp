#include "FileSystemModel.h"

#include "FileSystemListView.h"

FileSystemModel::FileSystemModel(const FileSystemListView * const view, QObject *parent)
    : QFileSystemModel(parent), view{view}
{
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        if (view->getSelected().contains(index))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    return QFileSystemModel::data(index, role);
}
