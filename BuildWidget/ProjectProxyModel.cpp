#include "ProjectProxyModel.h"

#include <QFileSystemModel>

#include "ProjectModel.h"

ProjectProxyModel::ProjectProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const ProjectModel *source = static_cast<ProjectModel *>(sourceModel());
    const QModelIndex &index = source->index(sourceRow, 0, sourceParent);
    return !source->getHiddenIndexes().contains(index.internalId());
}
