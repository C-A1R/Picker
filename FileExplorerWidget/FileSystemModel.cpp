#include "FileSystemModel.h"

#include "FileSystemView.h"

FileSystemModel::FileSystemModel(const FileSystemView * const view, QObject *parent)
    : QFileSystemModel(parent)
    , view{view}
{
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole && view->getSelected().contains(index))
        return QColor(Qt::red);
    if (index.column() == Columns::col_LastModified && role == Qt::DisplayRole)
        return fileInfo(index).lastModified().toString("dd.MM.yyyy hh:mm");
    return QFileSystemModel::data(index, role);
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return Columns::col_Max;
}
