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

bool ProjectProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const ProjectModel *source = static_cast<ProjectModel *>(sourceModel());
    const QList<quintptr> &orders = source->getOrders();
    return orders.indexOf(source_left.internalId()) < orders.indexOf(source_right.internalId());
}

void ProjectProxyModel::slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
{
    QList<quintptr> draggedIndicesIds;
    std::transform(draggedIndices.cbegin(), draggedIndices.cend(), std::back_inserter(draggedIndicesIds),
                   [this](const QModelIndex &ind) -> quintptr
                   {
                       return mapToSource(ind).internalId();
                   });
    emit signal_dropped(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0,
                        draggedIndicesIds);
}
