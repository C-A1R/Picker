#include "ProjectProxyModel.h"
#include "BuildWidget.h"

#include <QFileSystemModel>

ProjectProxyModel::ProjectProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool ProjectProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const model_type *source = static_cast<model_type *>(sourceModel());
    const QModelIndex &index = source->index(sourceRow, 0, sourceParent);
    return !source->getHiddenIndexes().contains(index.internalId());
}

bool ProjectProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const model_type *source = static_cast<model_type *>(sourceModel());
    const QList<quintptr> &orders = source->getOrders();
    return orders.indexOf(source_left.internalId()) < orders.indexOf(source_right.internalId());
}

void ProjectProxyModel::slot_dropped(const QModelIndex &droppedIndex, const QModelIndexList &draggedIndices)
{
    if (draggedIndices.isEmpty())
    {
        return;
    }
    QList<quintptr> draggedIndicesIds;
    std::transform(draggedIndices.cbegin(), draggedIndices.cend(), std::back_inserter(draggedIndicesIds),
                   [this](const QModelIndex &ind) -> quintptr
                   {
                       return mapToSource(ind).internalId();
                   });
    emit signal_dropped(droppedIndex.isValid() ? mapToSource(droppedIndex).internalId() : 0,
                        draggedIndicesIds);
}

void ProjectProxyModel::slot_setChecked(const QModelIndexList &selected, const bool checked)
{
    if (selected.isEmpty())
    {
        return;
    }
    QModelIndexList sourceIndices;
    std::transform(selected.cbegin(), selected.cend(), std::back_inserter(sourceIndices),
                   [this](const QModelIndex &ind) -> QModelIndex
                   {
                       return mapToSource(ind);
                   });
    emit signal_setChecked(sourceIndices, checked);
}
