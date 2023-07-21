#include "ProjectProxyModel.h"

#include <QFileSystemModel>

ProjectProxyModel::ProjectProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

//bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
//{
//    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
//    QFileDevice::Permissions permissions = static_cast<QFileDevice::Permissions>(index.data(QFileSystemModel::FilePermissions).toInt());
//    return permissions.testFlag(QFileDevice::WriteUser); // Ok if user can write
//}

